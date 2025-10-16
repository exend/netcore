#include <c357/net/core/app_protocol_handler_registry.hpp>

using namespace std;
using namespace c357::net::core;

void app_proto_hndl_reg::register_handler(
    uint16_t port,
    const app_proto_hndl_sptr &hndl)
{
	hndls.emplace(port, hndl);
}

app_proto_hndl_sptr app_proto_hndl_reg::get_handler(uint16_t port) const
{
	auto it = hndls.find(port);
	if (it != hndls.end())
		return it->second;
	return nullptr;
}

void app_proto_hndl_reg::remove_handler(uint16_t port)
{
	hndls.erase(port);
}

vector<uint16_t> app_proto_hndl_reg::ports() const
{
	vector<uint16_t> ports;
	ports.reserve(hndls.size());
	for (auto &[port, _] : hndls)
		ports.push_back(port);
	return ports;
}

void app_proto_hndl_reg::clear() noexcept
{
	hndls.clear();
}
