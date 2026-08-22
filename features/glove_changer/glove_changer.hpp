#pragma once
#include "../../main.hpp"

#include "../shared/item_schema.hpp"
#include "../shared/data.hpp"

class c_cs_player_pawn;
class c_glove_changer {
public:
	void run(c_cs_player_pawn* observer_target, engine_data current);
	bool should_update = false;

private:
	c_config::glove_changer_t glove_backup_cfg;
	engine_data backup_engine_data;

	int m_update_frames = 0;
	int m_clear_frames = 0;
};
inline const auto g_glove_changer = std::make_unique<c_glove_changer>();
