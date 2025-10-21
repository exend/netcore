#include <c357/net/core/io/socketbuf.hpp>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <utility>
#include <cstring>
#include <system_error>

using namespace c357::net::core;
using namespace std;

class socketbuf::error : public system_error {
public:
	error(int errcode, const char *what)
	    : system_error(errcode, generic_category(), what)
	{ }
	error(const char *what)
	    : error(errno, what)
	{ }
};

const socketbuf::int_type socketbuf::closed = -1;
const streamsize socketbuf::buf_size = 0x2000;

socketbuf::socketbuf(int sockfd)
    : gbuf(buf_size)
    , pbuf(buf_size)
    , sockfd(sockfd)
{
	setg(gbuf.data(), gbuf.data(), gbuf.data());
	setp(pbuf.data(), pbuf.data() + pbuf.size());
}

socketbuf::socketbuf(socketbuf &&other)
    : streambuf(other)
    , gbuf(std::move(other.gbuf))
    , pbuf(std::move(other.pbuf))
    , sockfd(exchange(other.sockfd, -1))
{
	other.setg(nullptr, nullptr, nullptr);
	other.setp(nullptr, nullptr);
}

socketbuf::~socketbuf()
{
	close();
}

socketbuf &socketbuf::operator=(socketbuf &&other)
{
	if (&other != this) {
		sync();
		streambuf::operator=(other);
		gbuf = std::move(other.gbuf);
		pbuf = std::move(other.pbuf);
		sockfd = exchange(other.sockfd, -1);
		other.setg(nullptr, nullptr, nullptr);
		other.setp(nullptr, nullptr);
	}
	return *this;
}

bool socketbuf::is_open() const noexcept
{
	if (sockfd == closed)
		return false;
	int flags = fcntl(sockfd, F_GETFL);
	return flags != -1 || errno != EBADF;
}

void socketbuf::close()
{
	if (!is_open())
		return;
	sync();
	while (::close(sockfd) == -1 && errno == EINTR)
		continue;
	sockfd = closed;
}

streamsize socketbuf::xsputn(const char_type *s, streamsize count)
{
	streamsize avail = epptr() - pptr();
	if (count <= avail) {
		memcpy(pptr(), s, count);
		pbump(count);
		return count;
	}
	if (pptr() != pbase()) {
		if (sync() == -1)
			return 0;
	}
	streamsize pbufsize = pbuf.size();
	streamsize total_written = 0;
	streamsize remain = count;
	const char_type *sptr = s;
	while (remain > 0) {
		if (remain >= pbufsize) {
			if (!send(sptr, pbufsize))
				return total_written;
			total_written += pbufsize;
			remain -= pbufsize;
			sptr += pbufsize;
		} else {
			setp(pbuf.data(), pbuf.data() + pbuf.size());
			memcpy(pbase(), sptr, remain);
			pbump(remain);
			total_written += remain;
			break;
		}
	}
	return total_written;
}

socketbuf::int_type socketbuf::underflow()
{
	if (gptr() == egptr()) {
		ssize_t bytes_read = ::recv(sockfd, gbuf.data(), gbuf.size(), 0);
		if (bytes_read <= 0)
			return traits_type::eof();
		char *data = gbuf.data();
		setg(data, data, data + bytes_read);
	}
	return traits_type::to_int_type(*gptr());
}

socketbuf::int_type socketbuf::overflow(int_type ch)
{
	if (traits_type::eq_int_type(ch, traits_type::eof()))
		return traits_type::not_eof(ch);
	if (pptr() == epptr() && sync() == -1)
		return traits_type::eof();
	*pptr() = traits_type::to_char_type(ch);
	pbump(1);
	return ch;
}

int socketbuf::sync()
{
	streamsize to_send = pptr() - pbase();
	if (to_send > 0 && !send(pbase(), to_send)) {
		return -1;
	}
	setp(pbuf.data(), pbuf.data() + pbuf.size());
	return 0;
}

bool socketbuf::send(const char_type *s, streamsize count) noexcept
{
	streamsize total_sent = 0;
	while (total_sent < count) {
		const void *sptr = s + total_sent;
		size_t remain = count - total_sent;
		ssize_t sent = ::send(sockfd, sptr, remain, MSG_NOSIGNAL);
		if (sent == -1 && errno == EINTR)
			continue;
		if (sent <= 0)
			return false;
		total_sent += sent;
	}
	return true;
}
