#ifndef C357_NET_CORE_CONNECTION_HPP
#define C357_NET_CORE_CONNECTION_HPP

#include <c357/net/core/ip_address.hpp>
#include <cstdint>
#include <istream>
#include <memory>
#include <vector>

namespace c357::net::core {

class socket_stream;

class connection {
public:
	connection(
		const ip_address &ip,
		uint16_t port,
		const std::shared_ptr<socket_stream> &sstream
	) noexcept;
	connection(const connection &other) = delete;
	connection(connection &&other) noexcept;
	virtual ~connection();
	connection &operator=(const connection &other) = delete;
	connection &operator=(connection &&other);
	const ip_address &ip() const noexcept;
	uint16_t port() const noexcept;
	virtual bool is_open() const noexcept;
	virtual void close();
	virtual std::shared_ptr<std::iostream> iostream() const noexcept;

private:
	ip_address ip_;
	uint16_t port_;
	std::shared_ptr<socket_stream> sstream;
};

}

#endif /* C357_NET_CORE_CONNECTION_HPP */
