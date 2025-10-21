#include "accept_handler_impl.hpp"
#include <c357/net/core/io/server_socket.hpp>
#include <c357/net/core/connection.hpp>

using namespace std;
using namespace c357::base::concurrent;
using namespace c357::net::core;

accept_handler_impl::accept_handler_impl(
    const app_proto_hndl_reg &r,
    const executor_sptr &e,
    const cancellation_token &t)
    : reg(r)
    , exec(e)
    , token(t)
{ }

void accept_hndl_impl::handle(
    server_socket &socket,
    shared_ptr<connection> conn) noexcept
{
	app_proto_hndl_sptr hndl;
	try {
		hndl = reg.get_handler(socket.port());
		if (!hndl)
			return socket.close();
		cancellation_token token = this->token;
		exec->execute([=] {
			hndl->handle(conn, token);
		});
	} catch (...) {
		if (hndl)
			exec->execute([=] {
				hndl->handle(current_exception());
			});
	}
}

void accept_hndl_impl::handle(server_socket &socket, exception_ptr eptr)
{
	app_proto_hndl_sptr hndl;
	try {
		hndl = reg.get_handler(socket.port());
		rethrow_exception(eptr);
	} catch (...) {
		if (hndl)
			exec->execute([=] {
				hndl->handle(current_exception());
			});
	}
}

void accept_hndl_impl::handle_finish(server_socket &socket)
{ }
