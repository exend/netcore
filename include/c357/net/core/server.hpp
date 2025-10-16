#ifndef C357_NET_CORE_SERVER_HPP
#define C357_NET_CORE_SERVER_HPP

#include <c357/base/concurrent/executor.hpp>
#include <c357/net/core/app_protocol_handler_registry.hpp>
#include <memory>

namespace c357::net::core {

class server_impl;

class server {
	using executor_sptr = base::concurrent::executor_sptr;

public:
	server(const app_proto_hndl_reg &);
	server(const app_proto_hndl_reg &, const executor_sptr &);
	server(const server &) = delete;
	virtual ~server() = default;
	virtual server &operator=(const server &) = delete;
	virtual void start();
	virtual void stop();
	virtual void wait();

private:
	std::shared_ptr<server_impl> pimpl;
};
}

#endif /* C357_NET_CORE_SERVER_HPP */
