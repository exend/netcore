#ifndef C357_NET_CORE_SERVER_HPP
#define C357_NET_CORE_SERVER_HPP

#include <c357/base/concurrent/executor.hpp>
#include <c357/net/core/app_protocol_handler_registry.hpp>
#include <memory>

namespace c357::net::core {

class server_impl;

/// @brief High-level server managing protocol handlers and connections.
class server {
	using executor_sptr = base::concurrent::executor_sptr;

public:
	virtual ~server() = default;

	/// @brief Constructs a server using a handler registry.
	server(const app_proto_hndl_reg &);

	/// @brief Constructs a server with a handler registry and a custom executor.
	server(const app_proto_hndl_reg &, const executor_sptr &);

	server(const server &) = delete;
	server &operator=(const server &) = delete;

	/// @brief Starts the server and begins accepting connections.
	virtual void start();

	/// @brief Stops accepting new connections and shuts down active ones.
	virtual void stop();

	/// @brief Blocks until the server has fully stopped,
	/// including closing all sockets and terminating listening.
	virtual void wait();

private:
	std::shared_ptr<server_impl> pimpl;
};

}

#endif /* C357_NET_CORE_SERVER_HPP */
