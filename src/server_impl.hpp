#ifndef C357_NET_CORE_SERVER_IMPL_HPP
#define C357_NET_CORE_SERVER_IMPL_HPP

#include <c357/base/concurrent/executor.hpp>
#include <c357/base/concurrent/cancellation_source.hpp>
#include <c357/net/core/app_protocol_handler_registry.hpp>
#include <c357/net/core/io/server_socket.hpp>
#include <atomic>
#include <mutex>
#include <deque>

namespace c357::net::core {

class accept_handler;

class server_impl final {
	using executor_sptr = base::concurrent::executor_sptr;

public:
	server_impl(const app_proto_hndl_reg &, const executor_sptr &);
	server_impl(const server_impl &) = delete;
	~server_impl();
	server_impl &operator=(const server_impl &) = delete;
	void start();
	void stop();
	void wait();

private:
	base::concurrent::cancellation_source cancel_src;
	std::shared_ptr<accept_handler> hndl;
	std::deque<server_socket> sockets;
	std::mutex mtx;
	std::atomic_bool all_sockets_closed;
};

}

#endif /* C357_NET_CORE_SERVER_IMPL_HPP */
