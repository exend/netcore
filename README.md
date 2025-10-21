# net::core

Part of the **c357** C++ libraries.  
Provides basic TCP networking components.

---

## Overview

`net::core` defines low-level and high-level abstractions for working with TCP connections and application protocols.  
It is written in modern C++ and designed to integrate with the `c357::base` concurrency utilities.

Main headers:

| Header | Description |
|--------|--------------|
| `ip_address.hpp` | IPv4 / IPv6 address representation |
| `connection.hpp` | Represents a TCP connection |
| `server_socket.hpp` | Accepts incoming connections |
| `socket_stream.hpp` / `socketbuf.hpp` | Stream interface over sockets |
| `accept_handler.hpp` | Interface for handling accepted connections |
| `app_protocol_handler.hpp` | Interface for protocol logic |
| `app_protocol_handler_registry.hpp` | Maps ports to protocol handlers |
| `server.hpp` | Manages multiple protocol handlers and connections |

---

## Example
```cpp
#include <c357/net/core/app_protocol_handler.hpp>
#include <c357/net/core/connection.hpp>
#include <c357/net/core/server.hpp>
#include <csignal>

using namespace std;
using namespace c357::base::concurrent;
using namespace c357::net::core;

class echo_handler : public app_protocol_handler {
public:
	void handle(shared_ptr<connection> conn, cancellation_token token) override
	{
		shared_ptr<iostream> stream = conn->iostream();
		string msg;
		while (!token.is_cancellation_requested() && *stream >> msg) {
			if (msg == "q")
				return conn->close();
			*stream << msg << endl;
			stream->flush();
		}
	}
	void handle(exception_ptr) override { }
};

server *psrv = nullptr;

static void sighndl(int sign)
{
	if (psrv)
		psrv->stop();
}

int main(void)
{
	signal(SIGINT, sighndl);
	signal(SIGTERM, sighndl);
	app_proto_hndl_reg reg;
	reg.register_handler(4242, make_shared<echo_handler>());
	server srv(reg);
	psrv = &srv;
	srv.start();
	srv.wait();
	return 0;
}
```
## Build
```bash
git clone https://github.com/exend/netcore.git
cd netcore
cmake -B build
cmake --build build
```
Include in your project:
```cmake
add_subdirectory(netcore)
target_link_libraries(${PROJECT_NAME} PRIVATE netcore)
```
