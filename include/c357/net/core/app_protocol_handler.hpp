#ifndef C357_NET_CORE_APP_PROTOCOL_HANDLER_HPP
#define C357_NET_CORE_APP_PROTOCOL_HANDLER_HPP

#include <c357/base/concurrent/cancellation_token.hpp>
#include <memory>

namespace c357::net::core {

class connection;

/// @brief Interface for handling application-level protocol logic.
struct app_protocol_handler {
	virtual ~app_protocol_handler() = default;

	/// @brief Called to process a connection.
	/// @param conn Connection instance.
	/// @param token Cancellation token to control operation.
	virtual void handle(
	    std::shared_ptr<connection> conn,
	    base::concurrent::cancellation_token token)
	    = 0;

	/// @brief Called when an exception occurs.
	virtual void handle(std::exception_ptr) = 0;
};

typedef std::shared_ptr<app_protocol_handler> app_proto_hndl_sptr;

}

#endif /* C357_NET_CORE_APP_PROTOCOL_HANDLER_HPP */
