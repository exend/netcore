#include "server_impl.hpp"
#include "accept_handler_impl.hpp"

using namespace std;
using namespace c357::base::concurrent;
using namespace c357::net::core;

server_impl::server_impl(const app_proto_hndl_reg &r, const executor_sptr &e)
    : cancel_src()
    , hndl(new accept_handler_impl(r, e, cancel_src.token()))
    , all_sockets_closed(false)
{
	for (uint16_t port : r.ports())
		sockets.emplace_back(port);
}

server_impl::~server_impl()
{
	stop();
}

void server_impl::start()
{
	unique_lock lock(mtx);
	if (all_sockets_closed)
		return;
	for (server_socket &socket : sockets)
		socket.start_accepting(hndl);
	all_sockets_closed = false;
}

void server_impl::stop()
{
	unique_lock lock(mtx, try_to_lock);
	if (!lock.owns_lock())
		return;
	cancel_src.cancel();
	for (server_socket &socket : sockets)
		socket.close();
	all_sockets_closed = true;
	all_sockets_closed.notify_all();
}

void server_impl::wait()
{
	all_sockets_closed.wait(false);
}
