#include "glove_changer.hpp"
#include "../shared/econ_item_attribute_manager.hpp"
#include "../../valve/interfaces/interfaces.hpp"

#include "glove_changer.hpp"
#include "../shared/econ_item_attribute_manager.hpp"
#include "../../valve/interfaces/interfaces.hpp"

void c_glove_changer::run(int stage) {
	if (!g_cfg->glove_changer.m_enabled || !g_ctx->m_local_pawn)
		return;

	auto* local_pawn = reinterpret_cast<c_cs_player_pawn*>(g_ctx->m_local_pawn);
	if (!valid_ptr(local_pawn) || local_pawn->m_health() <= 0)
		return;

	auto observer_service = local_pawn->m_observer_services();
	if (!observer_service || observer_service->m_observer_mode() == 0) // OBSERVER_MODE_NONE = 0x0
		return;

	auto observer_target = g_interfaces->m_game_resource->pGameEntitySystem->Get<c_cs_player_pawn>(observer_service->m_observer_target());
	if (!valid_ptr(observer_target) || observer_target->m_health() <= 0)
		return;

	auto* identity = observer_target->m_entity();
	if (!identity || !identity->is_valid())
		return;

	if (!identity->is_safe_to_modify())
		return;

	auto* glove_item = observer_target->m_econ_gloves();
	if (!glove_item)
		return;

	if (!g_item_schema->is_initialized()
		|| g_cfg->glove_changer.m_glove >= (int)g_item_schema->gloves.size())
		return;

	const uint16_t selected_glove =
		g_item_schema->gloves[g_cfg->glove_changer.m_glove].definition_index;
	if (selected_glove == 0)
		return;

	int paint_kit_id = g_item_schema->get_paint_kit_id_for_item(selected_glove, g_cfg->glove_changer.m_paint_kit);

	const float current_spawn_time = observer_target->m_last_spawn_time_index();
	const int   current_team = observer_target->m_team_num();

	const bool config_changed = (m_last_glove != selected_glove)
		|| (m_last_paint_kit_id != paint_kit_id)
		|| (m_last_wear != g_cfg->glove_changer.m_wear)
		|| (m_last_seed != g_cfg->glove_changer.m_seed);
	const bool team_changed = (current_team != m_last_team) && m_last_team != 0;
	const bool spawn_changed = (current_spawn_time != m_last_spawn_time);
	const bool pawn_state_changed = team_changed || spawn_changed;
	const bool engine_reset = (glove_item->m_definition_index() != selected_glove)
		|| !glove_item->m_initialized()
		|| observer_target->m_need_to_reapply_gloves();

	if (config_changed || should_update)
		m_update_frames = 4;

	if (m_update_frames <= 0) {
		should_update = false;
		return;
	}

	auto* paint_kit = glove_item->construct_paint_kit(); // bugs if u apply a wrong skin to a wrong type of glove

	glove_item->m_definition_index() = selected_glove;
	glove_item->m_entity_quality() = QUALITY_UNUSUAL;

	if (paint_kit && paint_kit_id > 0) {
		if (auto* desired_pk = g_interfaces->m_source2_client->get_econ_item_system()
			->get_econ_item_schema()->get_paint_kits().find_by_key(paint_kit_id))
			paint_kit->m_name = desired_pk->m_name;
	}

	econ_item_attribute_manager::remove(glove_item);
	if (paint_kit_id > 0)
		econ_item_attribute_manager::create(glove_item, paint_kit_id, g_cfg->glove_changer.m_wear, g_cfg->glove_changer.m_seed);

	glove_item->m_item_id_high() = 0xFFFFFFFF;
	glove_item->m_initialized() = true; //m_bInitialized
	glove_item->m_bDisallowSOCm() = false;
	glove_item->m_bRestoreCustomMaterialAfterPrecache() = true;

	observer_target->set_body_group();
	observer_target->m_need_to_reapply_gloves() = true;

	m_last_glove = selected_glove;
	m_last_paint_kit_id = paint_kit_id;
	m_last_wear = g_cfg->glove_changer.m_wear;
	m_last_seed = g_cfg->glove_changer.m_seed;
	m_last_spawn_time = current_spawn_time;
	m_last_team = current_team;
	m_update_frames--;
	should_update = false;
}
