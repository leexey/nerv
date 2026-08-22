#pragma once

#include "../shared/data.hpp"
#include "../../main.hpp"
#include "../../valve/classes/c_cs_player_pawn.hpp"
#include "../shared/item_schema.hpp"

class c_agent_changer {
public:
	void run(c_cs_player_pawn* observer_target, engine_data current);
	bool should_update = false;

private:
	void custom_model(c_cs_player_pawn* observer_target, engine_data current);
	void agent_model(c_cs_player_pawn* observer_target, engine_data current);

	c_config::agent_changer_t agent_backup_cfg;
	engine_data backup_engine_data;

	int m_update_frames = 0;
};

inline const auto g_agent_changer = std::make_unique<c_agent_changer>();
