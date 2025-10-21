#ifndef C357_NET_CORE_IP_ADDRESS_HPP
#define C357_NET_CORE_IP_ADDRESS_HPP

#include <cstdint>
#include <string>

struct in_addr;
struct in6_addr;

namespace c357::net::core {

/// @brief Represents an IPv4 or IPv6 address.
struct ip_address final {
	class inet_ntop_error;

	/// @brief IP version.
	enum class version_t : uint8_t {
		v4 = 4,
		v6 = 6
	};

	/// @brief Union holding raw address data.
	union addr_t {
		uint32_t v4;
		uint8_t v6[16];
	};

	// @brief Constructs an IPv4 address from a 32-bit integer.
	ip_address(uint32_t v4) noexcept;

	/// @brief Constructs an IPv4 addres from struct in_addr.
	ip_address(const in_addr &v4) noexcept;

	/// @brief Constructs an IPv6 addres from raw IPv6 bytes.
	ip_address(const uint8_t (&v6)[16]) noexcept;

	/// @brief Constructs an IPv6 addres from struct in6_addr.
	ip_address(const in6_addr &v6) noexcept;

	/// @brief Returns IP version (v4 or v6).
	version_t version() const noexcept;

	/// @brief Returns raw address value.
	const addr_t &raw_value() const noexcept;

	/// @brief Returns textual representation.
	std::string str() const;

private:
	version_t version_;
	addr_t value;
};

}

#endif /* C357_NET_CORE_IP_ADDRESS_HPP */
