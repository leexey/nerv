#pragma once
#include "item_schema.hpp"
#include "../../main.hpp"

struct engine_data {
	float m_spawn_time = 0.f;
	int m_team = 0;
	int m_demo_tick = 0;
	uint64_t m_steam_id = 0;

	bool need_engine_reset(c_econ_item_view* item) {
		return item->m_definition_index() != g_item_schema->gloves[g_cfg->glove_changer.m_glove].definition_index || !item->m_initialized();
	}

	bool state_changed(const engine_data& backup) {
		return (this->m_spawn_time != backup.m_spawn_time) || (this->m_team != backup.m_team && backup.m_team != 0);
	}

	bool data_changed(const engine_data& backup, c_econ_item_view* item) {
		return (this->m_spawn_time != backup.m_spawn_time || this->m_team != backup.m_team) || need_engine_reset(item);
	}
}; inline engine_data general_engine_backup;