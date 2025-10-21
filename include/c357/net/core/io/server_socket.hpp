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

/// @brief TCP server socket accepting incoming connections.
class server_socket {
public:
	class error;

	server_socket(const server_socket &) = delete;

	/// @brief Constructs a listening socket bound to the given port.
	server_socket(uint16_t port);

	virtual ~server_socket();

	server_socket &operator=(const server_socket &) = delete;

	/// @brief Returns the port the socket is bound to.
	uint16_t port() const noexcept;

	/// @brief Closes the socket.
	virtual void close();

	/// @brief Returns true if the socket is closed.
	bool is_closed() const noexcept;

	/// @brief Waits for and accepts a single incoming connection.
	std::shared_ptr<connection> accept();

	/// @brief Starts asynchronous accepting with a handler.
	void start_accepting(std::shared_ptr<accept_handler> hndl);

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
