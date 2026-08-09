#pragma once
class i_entity_system;

class i_game_resource_service
{
public:
	char pad_000[88];
	i_entity_system* pGameEntitySystem;
};