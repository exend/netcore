#include "server_impl.hpp"
#include <c357/base/concurrent/thread_pool.hpp>
#include <c357/net/core/server.hpp>

using namespace std;
using namespace c357::base::concurrent;
using namespace c357::net::core;

server::server(const app_proto_hndl_reg &reg)
    : server(reg, make_shared<thread_pool>())
{ }

server::server(const app_proto_hndl_reg &reg, const executor_sptr &exec)
    : pimpl(new server_impl(reg, exec))
{ }

void server::start()
{
	pimpl->start();
}

void server::stop()
{
	pimpl->stop();
}

void server::wait()
{
	pimpl->wait();
}
