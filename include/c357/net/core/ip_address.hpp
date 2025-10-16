#ifndef C357_NET_CORE_IP_ADDRESS_HPP
#define C357_NET_CORE_IP_ADDRESS_HPP

#include <cstdint>
#include <string>

struct in_addr;
struct in6_addr;

namespace c357::net::core {

struct ip_address final {
	class inet_ntop_error;

	enum class version_t : uint8_t {
		v4 = 4,
		v6 = 6
	};

	union addr_t {
		uint32_t v4;
		uint8_t v6[16];
	};

	ip_address(uint32_t v4) noexcept;
	ip_address(const in_addr &v4) noexcept;
	ip_address(const uint8_t (&v6)[16]) noexcept;
	ip_address(const in6_addr &v6) noexcept;
	version_t version() const noexcept;
	const addr_t &raw_value() const noexcept;
	std::string str() const;

private:
	version_t version_;
	addr_t value;
};

}

#endif /* C357_NET_CORE_IP_ADDRESS_HPP */
