#ifndef C357_NET_CORE_ACCEPT_HANDLER_HPP
#define C357_NET_CORE_ACCEPT_HANDLER_HPP

#include <functional>
#include <memory>
#include <exception>

namespace c357::net::core {

class connection;
class server_socket;

 /// @brief Interface for handling incoming connections and accept events.
struct accept_handler {
	virtual ~accept_handler() = default;

	/// @brief Called when a new connection is accepted.
	/// @param socket Reference to the server socket.
	/// @param conn Accepted connection.
	virtual void handle(
	    server_socket &socket,
	    std::shared_ptr<connection> conn) noexcept
	    = 0;

	/// @brief Called when an exception occurs during accept.
	virtual void handle(server_socket &, std::exception_ptr) = 0;

	/// @brief Called when accepting loop has finished.
	virtual void handle_finish(server_socket &) = 0;
};

typedef std::shared_ptr<accept_handler> accept_hndl_sptr;

}

#endif /* C357_NET_CORE_ACCEPT_HANDLER_HPP */
