#ifndef C357_NET_CORE_CONNECTION_HPP
#define C357_NET_CORE_CONNECTION_HPP

#include <c357/net/core/ip_address.hpp>
#include <cstdint>
#include <istream>
#include <memory>
#include <vector>

namespace c357::net::core {

class socket_stream;

/// @brief Represents a single TCP connection.
class connection {
public:
	/// @brief Constructs a connection object.
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

	/// @brief Returns the remote IP address.
	const ip_address &ip() const noexcept;

	/// @brief Returns the remote port.
	uint16_t port() const noexcept;

	/// @brief Returns true if the connection is open.
	virtual bool is_open() const noexcept;

	/// @brief Closes the connection.
	virtual void close();

	/// @brief Returns an iostream associated with the connection.
	virtual std::shared_ptr<std::iostream> iostream() const noexcept;

private:
	ip_address ip_;
	uint16_t port_;
	std::shared_ptr<socket_stream> sstream;
};

}

#endif /* C357_NET_CORE_CONNECTION_HPP */
