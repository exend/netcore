#include <c357/net/core/io/socket_stream.hpp>
#include <c357/net/core/io/socketbuf.hpp>

using namespace c357::net::core;
using namespace std;

socket_stream::socket_stream(int sockfd)
    : iostream(new socketbuf(sockfd))
    , sbuf(static_cast<socketbuf *>(rdbuf()))
{ }

socket_stream::socket_stream(socket_stream &&other)
    : iostream(std::move(other))
    , sbuf(exchange(other.sbuf, nullptr))
{ }

socket_stream::~socket_stream()
{
    close();
}

socket_stream &socket_stream::operator=(socket_stream &&other)
{
	if (&other != this) {
		iostream::operator=(std::move(other));
		sbuf = exchange(other.sbuf, nullptr);
	}
	return *this;
}

bool socket_stream::is_open() const noexcept
{
	if (sbuf)
		return sbuf->is_open();
	return false;
}

void socket_stream::close()
{
	if (sbuf) {
		sbuf->close();
		sbuf = nullptr;
	}
}
