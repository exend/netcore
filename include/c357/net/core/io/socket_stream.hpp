#ifndef C357_NET_CORE_SOCKET_STREAM_HPP
#define C357_NET_CORE_SOCKET_STREAM_HPP

#include <istream>
#include <memory>

namespace c357::net::core {

class socketbuf;

class socket_stream : public std::iostream {
public:
	explicit socket_stream(int sockfd);
	socket_stream(const socket_stream &other) = delete;
	socket_stream(socket_stream &&other);
	~socket_stream() override;
	socket_stream &operator=(const socket_stream &other) = delete;
	socket_stream &operator=(socket_stream &&other);
	virtual bool is_open() const noexcept;
	virtual void close();
private:
	std::unique_ptr<socketbuf> sbuf;
};

}

#endif /* C357_NET_CORE_SOCKET_STREAM_HPP */
