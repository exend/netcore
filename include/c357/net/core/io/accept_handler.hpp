#ifndef C357_NET_CORE_ACCEPT_HANDLER_HPP
#define C357_NET_CORE_ACCEPT_HANDLER_HPP

#include <functional>
#include <memory>
#include <exception>

namespace c357::net::core {

class connection;
class server_socket;

struct accept_handler {
	virtual ~accept_handler() = default;
	virtual void handle(server_socket &, std::shared_ptr<connection>) noexcept = 0;
	virtual void handle(server_socket &, std::exception_ptr) = 0;
	virtual void handle_finish(server_socket &) = 0;
};

typedef std::shared_ptr<accept_handler> accept_hndl_sptr;
}

#endif /* C357_NET_CORE_ACCEPT_HANDLER_HPP */
