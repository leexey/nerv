#pragma once
#include "../../main.hpp"

#include "../shared/item_schema.hpp"
#include "../shared/data.hpp"

class c_cs_player_pawn;
class c_skin_changer {
public:
	void run(c_cs_player_pawn* observer_target, engine_data current);
	bool should_update = false;

private:
	c_config::knife_changer_t knife_backup_cfg;
	c_config::skin_changer_t skin_backup_cfg;
	engine_data backup_engine_data;
	std::vector<uint16_t> m_last_weapon_indices;

	int m_update_frames = 0;
	int m_clear_frames = 0;

	c_base_entity* get_hud_weapon(c_base_entity* weapon, c_cs_player_pawn* local_pawn);
	void apply_skin(c_econ_entity* weapon, c_econ_item_view* item, int paint_kit_id, float wear, int seed, const char* custom_name, c_cs_player_pawn* local_pawn, uint16_t def_index = 0, const char* model = {});
	void process_weapon(c_econ_entity* weapon, c_econ_item_view* item, c_cs_player_pawn* local_pawn, bool force_update, bool& did_update, uint64_t local_steam_id);
	void process_knife(c_econ_entity* weapon, c_econ_item_view* item, c_cs_player_pawn* local_pawn, bool force_update, bool& did_update, uint64_t local_steam_id);
};

inline const auto g_skin_changer = std::make_unique<c_skin_changer>();