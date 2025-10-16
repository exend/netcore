#ifndef C357_NET_CORE_SERVER_SOCKET_HPP
#define C357_NET_CORE_SERVER_SOCKET_HPP

#include <netinet/in.h>
#include <cstdint>
#include <atomic>
#include <memory>
#include <thread>

namespace c357::net::core {

class connection;
class accept_handler;

class server_socket final {
public:
	class error;

	server_socket(uint16_t port);
	server_socket(const server_socket &) = delete;
	virtual ~server_socket();
	server_socket &operator=(const server_socket &) = delete;
	uint16_t port() const noexcept;
	virtual void close();
	bool is_closed() const noexcept;
	std::shared_ptr<connection> accept();
	void start_accepting(std::shared_ptr<accept_handler> hndl_ptr);

private:
	static const int closed;
	uint16_t port_;
	sockaddr_in addr;
	std::atomic_int listen_fd;
	std::atomic_int wakeup_fds[2];
	std::mutex close_mtx;
	std::atomic_bool stop;
	std::atomic_bool accept_loop_finished;
	std::thread accept_thread;
	void init();
	void close_pipe();
	void wakeup();
	void accept_loop(std::shared_ptr<accept_handler>);
};

}

#endif /* C357_NET_CORE_SERVER_SOCKET_HPP */
