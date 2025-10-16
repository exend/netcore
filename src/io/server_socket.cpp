#include <c357/net/core/io/server_socket.hpp>
#include <c357/net/core/io/socket_stream.hpp>
#include <c357/net/core/io/accept_handler.hpp>
#include <c357/net/core/connection.hpp>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <system_error>

using namespace c357::net::core;
using namespace std;

class server_socket::error : public system_error {
public:
	error(int errcode, const char *what)
	    : system_error(errcode, generic_category(), what)
	{ }
	error(const char *what)
	    : error(errno, what)
	{ }
};

const int server_socket::closed = -1;

inline sockaddr_in any_addr(uint16_t port)
{
	sockaddr_in addr { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	return addr;
}

inline int socket()
{
	int result = ::socket(AF_INET, SOCK_STREAM, 0);
	if (result == -1)
		throw server_socket::error("socket");
	return result;
}

inline void setsockopt(int sockfd)
{
	int optval = 1;
	if (::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
		throw server_socket::error("setsockopt");
}

inline int fcntl(int fd, int cmd, int arg)
{
	int result = ::fcntl(fd, cmd, arg, 0);
	if (result == -1)
		throw server_socket::error("fcntl");
	return result;
}

static int getsockerr(int sockfd)
{
	int err = 1;
	if (::setsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, sizeof(err)))
		throw server_socket::error("setsockopt");
	return err;
}

inline void bind(int sockfd, const sockaddr_in &addr_in)
{
	auto addr = reinterpret_cast<const sockaddr *>(&addr_in);
	if (::bind(sockfd, addr, sizeof(sockaddr_in)))
		throw server_socket::error("bind");
}

inline void listen(int sockfd)
{
	if (::listen(sockfd, SOMAXCONN))
		throw server_socket::error("listen");
}

inline void pipe(std::atomic_int (&fds)[2])
{
	int tmp_fds[2];
	if (::pipe(tmp_fds))
		throw server_socket::error("pipe");
	fds[0] = tmp_fds[0];
	fds[1] = tmp_fds[1];
}

inline int poll(pollfd (&pfds)[2])
{
	int result = poll(static_cast<pollfd *>(pfds), 2, -1);
	if (result == -1)
		throw server_socket::error("poll");
	return result;
}

inline shared_ptr<connection> accept(int sockfd)
{
	sockaddr_in addr;
	socklen_t addr_size = sizeof(addr);
	int fd = accept(sockfd, (sockaddr *)&addr, &addr_size);
	if (fd == -1)
		throw server_socket::error("accept");
	ip_address ip(addr.sin_addr);
	uint16_t port = ntohs(addr.sin_port);
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
	return make_shared<connection>(ip, port, make_shared<socket_stream>(fd));
}

server_socket::server_socket(uint16_t port)
    : port_(port)
    , addr(any_addr(port))
    , listen_fd(closed)
	, stop(false)
	, accept_loop_finished(true)
{
	pipe(wakeup_fds);
}

server_socket::~server_socket()
{
	close();
	close_pipe();
}

uint16_t server_socket::port() const noexcept
{
	return port_;
}

void server_socket::close()
{
	unique_lock lock(close_mtx, try_to_lock);
	if (!lock.owns_lock() || is_closed())
		return;
	stop = true;
	wakeup();
	int lfd = listen_fd.exchange(closed);
	if (lfd != closed)
		while (::close(lfd) == -1 && errno == EINTR)
			continue;
	if (!accept_loop_finished)
		accept_loop_finished.wait(false);
}

bool server_socket::is_closed() const noexcept
{
	return listen_fd == closed;
}

shared_ptr<connection> server_socket::accept()
{
	if (is_closed())
		init();
	return ::accept(listen_fd);
}

void server_socket::start_accepting(shared_ptr<accept_handler> hndl)
{
	if (!is_closed())
		return;
	if (!accept_loop_finished)
		accept_loop_finished.wait(false);
	accept_thread = thread(&server_socket::accept_loop, this, hndl);
	accept_thread.detach();
}

void server_socket::init()
{
	listen_fd = ::socket();
	setsockopt(listen_fd);
	fcntl(listen_fd, F_SETFL, O_NONBLOCK);
	::bind(listen_fd, addr);
	listen(listen_fd);
}

void server_socket::close_pipe()
{
	for (atomic_int &fd : wakeup_fds) {
		if (fd != closed) {
			while (::close(fd) == -1 && errno == EINTR)
				continue;
			fd = closed;
		}
	}
}

void server_socket::wakeup()
{
	int fd = wakeup_fds[1];
	if (fd != closed) {
		const char buf { 0 };
		write(fd, &buf, 1);
	}
}

void server_socket::accept_loop(shared_ptr<accept_handler> phndl)
{
	accept_loop_finished = false;
	stop = false;
	if (!phndl)
		return;
	if (is_closed())
		init();
	constexpr short err_flags = POLLERR | POLLHUP | POLLNVAL;
	accept_handler &rhndl = *phndl;
	pollfd fds[2];
	pollfd &listen_pfd = fds[0];
	pollfd &wakeup_pfd = fds[1];
	listen_pfd.fd = listen_fd;
	listen_pfd.events = POLLIN;
	wakeup_pfd.fd = wakeup_fds[0];
	wakeup_pfd.events = POLLIN;
	try {
		while (!stop) {
			if (!poll(fds))
				continue;
			bool stop = this->stop;
			if (stop)
				break;
			if ((listen_pfd.revents | wakeup_pfd.revents) & err_flags) {
				bool is_listen_err = listen_pfd.revents & err_flags;
				int fd = is_listen_err ? listen_fd.load() : wakeup_fds[0].load();
				int err = getsockerr(fd);
				throw error(err, "poll");
			}
			if (wakeup_pfd.revents & POLLIN) {
				char buf;
				read(wakeup_fds[0], &buf, 1);
				continue;
			}
			if (listen_pfd.revents & POLLIN && !stop)
				rhndl.handle(*this, ::accept(listen_fd));
		}
		accept_loop_finished = true;
		accept_loop_finished.notify_all();
		rhndl.handle_finish(*this);
	} catch (exception &e) {
		accept_loop_finished = true;
		accept_loop_finished.notify_all();
		close();
		rhndl.handle(*this, current_exception());
	}
}
