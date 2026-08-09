#pragma once
#include <cstdint>
#include "../../../sdk/vfunc/vfunc.hpp"


class CNetworkGameClient
{
public:
	void SetUseGameState()
	{
		bool value = *reinterpret_cast<bool*>((uintptr_t)this + 0x1230);
		if(!value)
			*reinterpret_cast<bool*>((uintptr_t)this + 0x1230) = true;
	}
};

class i_network_client_service
{
public:


	[[nodiscard]] CNetworkGameClient* GetNetworkGameClient()
	{
		return vmt::call_virtual<CNetworkGameClient*>(this, 23);
	}
};
