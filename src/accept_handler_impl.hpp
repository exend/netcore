#ifndef C357_NET_CORE_ACCEPT_HANDLER_IMPL_HPP
#define C357_NET_CORE_ACCEPT_HANDLER_IMPL_HPP

#include <c357/base/concurrent/executor.hpp>
#include <c357/net/core/io/accept_handler.hpp>
#include <c357/net/core/app_protocol_handler_registry.hpp>
#include <exception>

namespace c357::net::core {

struct accept_handler_impl : public accept_handler {
	accept_handler_impl(
	    const app_proto_hndl_reg &,
	    const base::concurrent::executor_sptr &,
	    const base::concurrent::cancellation_token &);
	void handle(server_socket &, std::shared_ptr<connection>) noexcept override;
	void handle(server_socket &, std::exception_ptr) override;
	void handle_finish(server_socket &) override;

private:
	const app_proto_hndl_reg reg;
	base::concurrent::executor_sptr exec;
	base::concurrent::cancellation_token token;
};

typedef accept_handler_impl accept_hndl_impl;

}

#endif /* C357_NET_CORE_ACCEPT_HANDLER_IMPL_HPP */
