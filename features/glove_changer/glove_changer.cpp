#include "glove_changer.hpp"
#include "../shared/econ_item_attribute_manager.hpp"
#include "../../valve/interfaces/interfaces.hpp"

void c_glove_changer::run(c_cs_player_pawn* observer_target, engine_data current) {
	if (!g_cfg->glove_changer.m_enabled)
		return;

	auto* identity = observer_target->m_entity();
	if (!identity || !identity->is_valid())
		return;

	if (!identity->is_safe_to_modify())
		return;

	auto* glove_item = observer_target->m_econ_gloves();
	if (!glove_item)
		return;

	if (g_cfg->glove_changer.m_glove >= (int)g_item_schema->gloves.size())
		return;

	const bool config_changed = memcmp(&glove_backup_cfg, &g_cfg->glove_changer, sizeof(glove_backup_cfg)) != 0;

	const uint16_t def_index = g_item_schema->gloves[g_cfg->glove_changer.m_glove].definition_index;
	if (def_index == 0)
		return;

	const int paint_kit_id = g_item_schema->get_paint_kit_id_for_item(def_index, g_cfg->glove_changer.m_paint_kit);

	if (config_changed || current.data_changed(backup_engine_data, glove_item) || should_update || observer_target->m_need_to_reapply_gloves())
		m_update_frames = 4;

	if (m_clear_frames > 0)
	{
		econ_item_attribute_manager::remove(glove_item);

		glove_item->m_definition_index() = 0;
		glove_item->m_initialized() = false;
		reinterpret_cast<c_cs_player_pawn*>(g_ctx->m_local_pawn)->m_need_to_reapply_gloves() = true;
		m_clear_frames--;

		backup_engine_data = current;
		should_update = false;
		return;
	}

	if (m_update_frames <= 0) {
		should_update = false;
		return;
	}

	auto* paint_kit = glove_item->construct_paint_kit(); // bugs if u apply a wrong skin to a wrong type of glove

	glove_item->m_definition_index() = def_index;
	glove_item->m_entity_quality() = QUALITY_UNUSUAL;

	if (paint_kit && paint_kit_id > 0) {
		if (auto* desired_pk = g_interfaces->m_source2_client->get_econ_item_system()
			->get_econ_item_schema()->get_paint_kits().find_by_key(paint_kit_id))
			paint_kit->m_name = desired_pk->m_name;
	}

	econ_item_attribute_manager::remove(glove_item);
	if (paint_kit_id > 0)
		econ_item_attribute_manager::create(glove_item, paint_kit_id, g_cfg->glove_changer.m_wear, g_cfg->glove_changer.m_seed);

	//glove_item->m_item_id_high() = 0xFFFFFFFF;
	glove_item->m_initialized() = true; //m_bInitialized
	//glove_item->m_bDisallowSOCm() = false;
	//glove_item->m_bRestoreCustomMaterialAfterPrecache() = true;

	observer_target->set_body_group();
	observer_target->m_need_to_reapply_gloves() = true;

	glove_backup_cfg = g_cfg->glove_changer;
	backup_engine_data = current;

	m_update_frames--;
	should_update = false;
}
