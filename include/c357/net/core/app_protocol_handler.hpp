#ifndef C357_NET_CORE_APP_PROTOCOL_HANDLER_HPP
#define C357_NET_CORE_APP_PROTOCOL_HANDLER_HPP

#include <c357/base/concurrent/cancellation_token.hpp>
#include <memory>

namespace c357::net::core {

class connection;

struct app_protocol_handler {
	virtual ~app_protocol_handler() = default;
	virtual void handle(
	    std::shared_ptr<connection>,
	    base::concurrent::cancellation_token)
	    = 0;
	virtual void handle(std::exception_ptr) = 0;
};

typedef std::shared_ptr<app_protocol_handler> app_proto_hndl_sptr;

}

#endif /* C357_NET_CORE_APP_PROTOCOL_HANDLER_HPP */
