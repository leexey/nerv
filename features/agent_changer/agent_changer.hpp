#pragma once

#include "../../main.hpp"
#include "../../valve/classes/c_cs_player_pawn.hpp"
#include "../shared/item_schema.hpp"

class c_agent_changer {
public:
	void run();

	bool should_update = false;
private:
	void custom_model();

	uint16_t m_last_agent = 0;
	float m_last_spawn_time = -1.f;
	int m_last_team = 0;
};

inline const auto g_agent_changer = std::make_unique<c_agent_changer>();
