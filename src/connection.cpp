#include <c357/net/core/connection.hpp>
#include <c357/net/core/io/socket_stream.hpp>

using namespace c357::net::core;
using namespace std;

connection::connection(
    const ip_address &ip,
    uint16_t port,
    const shared_ptr<socket_stream> &ss) noexcept
    : ip_(ip)
    , port_(port)
    , sstream(ss)
{ }

connection::connection(connection &&other) noexcept
    : ip_(other.ip_)
    , port_(other.port_)
    , sstream(exchange(other.sstream, nullptr))
{ }

connection::~connection()
{
	close();
}

connection &connection::operator=(connection &&other)
{
	if (&other != this) {
		ip_ = other.ip_;
		port_ = other.port_;
		sstream = exchange(other.sstream, nullptr);
	}
	return *this;
}

const ip_address &connection::ip() const noexcept
{
	return ip_;
}

uint16_t connection::port() const noexcept
{
	return port_;
}

bool connection::is_open() const noexcept
{
	return sstream->is_open();
}

void connection::close()
{
	sstream->close();
}

shared_ptr<std::iostream> connection::iostream() const noexcept
{
	return sstream;
}
