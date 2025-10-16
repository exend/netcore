#ifndef C357_NET_CORE_APP_PROTOCOL_HANDLER_REGISTRY
#define C357_NET_CORE_APP_PROTOCOL_HANDLER_REGISTRY

#include <c357/net/core/app_protocol_handler.hpp>
#include <vector>
#include <unordered_map>

namespace c357::net::core {

class app_protocol_handler_registry final {
public:
	void register_handler(uint16_t port, const app_proto_hndl_sptr &);
	app_proto_hndl_sptr get_handler(uint16_t port) const;
	void remove_handler(uint16_t port);
	std::vector<uint16_t> ports() const;
	void clear() noexcept;

private:
	std::unordered_map<uint16_t, app_proto_hndl_sptr> hndls;
};

typedef app_protocol_handler_registry app_proto_hndl_reg;

}

#endif /* C357_NET_CORE_APP_PROTOCOL_HANDLER_REGISTRY */
