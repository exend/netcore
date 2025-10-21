#ifndef C357_NET_CORE_SOCKETBUF_HPP
#define C357_NET_CORE_SOCKETBUF_HPP

#include <istream>
#include <vector>

namespace c357::net::core {

/// @brief Stream buffer for reading and writing through a socket.
class socketbuf : public std::streambuf {
public:
	class error;

	/// @brief Constructs a buffer bound to a socket descriptor.
	explicit socketbuf(int sockfd);

	socketbuf(const socketbuf &other) = delete;
	socketbuf(socketbuf &&other);

	~socketbuf() override;

	socketbuf &operator=(const socketbuf &other) = delete;
	socketbuf &operator=(socketbuf &&other);

	/// @brief Returns true if the socket is open.
	bool is_open() const noexcept;

	/// @brief Closes the socket.
	void close();

protected:
	using char_type = traits_type::char_type;
	using int_type = traits_type::int_type;
	std::streamsize xsputn(const char_type *s, std::streamsize count) override;
	int_type underflow() override;
	int_type overflow(int_type ch) override;
	int sync() override;

private:
	static const int_type closed;
	static const std::streamsize buf_size;
	std::vector<char> gbuf;
	std::vector<char> pbuf;
	int sockfd;
	bool send(const char_type *s, std::streamsize count) noexcept;
};

}

#endif /* SOCKETBUF_HPP */
