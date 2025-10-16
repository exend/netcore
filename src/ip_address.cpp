#include <c357/net/core/ip_address.hpp>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <system_error>

using namespace c357::net::core;
using namespace std;

class ip_address::inet_ntop_error : public system_error {
public:
	inet_ntop_error(int errcode)
	    : system_error(errcode, generic_category(), "inet_ntop")
	{ }
};

ip_address::ip_address(uint32_t v4) noexcept
    : version_(version_t::v4)
    , value { .v4 = v4 }
{ }

ip_address::ip_address(const in_addr &v4) noexcept
    : ip_address(v4.s_addr)
{ }

ip_address::ip_address(const uint8_t (&v6)[16]) noexcept
    : version_(version_t::v6)
{
	std::memcpy(value.v6, v6, sizeof(v6));
}

ip_address::ip_address(const in6_addr &v6) noexcept
    : ip_address(v6.__u6_addr.__u6_addr8)
{ }

ip_address::version_t ip_address::version() const noexcept { return version_; }

const ip_address::addr_t &ip_address::raw_value() const noexcept
{
	return value;
}

string ip_address::str() const
{
	switch (version_) {
	case version_t::v4:
		return inet_ntoa({ value.v4 });
	case version_t::v6:
		char ipv6_str[INET6_ADDRSTRLEN];
		if (!inet_ntop(AF_INET6, &value.v6, ipv6_str, INET6_ADDRSTRLEN))
			throw inet_ntop_error(errno);
		return ipv6_str;
	}
}
