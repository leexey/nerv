#include "skin_changer.hpp"
#include "../shared/econ_item_attribute_manager.hpp"
#include "../shared/item_schema.hpp"
#include "../../valve/interfaces/interfaces.hpp"
#include "../../valve/schema/schema.hpp"
#include "../../valve/interfaces/vtables/i_econ_item_system.hpp"
#include "../../menu/menu.hpp"
#include <vector>
#include <utility>


c_base_entity* c_skin_changer::get_hud_weapon(c_base_entity* weapon, c_cs_player_pawn* local_pawn) {
	auto arms_handle = local_pawn->m_hud_model_arms();
	if (!arms_handle.is_valid())
		return nullptr;

	auto* hud_arms = reinterpret_cast<c_base_entity*>(
		g_interfaces->m_entity_system->get_base_entity(arms_handle.get_entry_index())
		);
	if (!valid_ptr(hud_arms))
		return nullptr;

	auto* arms_node = hud_arms->m_scene_node();
	if (!valid_ptr(arms_node))
		return nullptr;

	for (auto* vm = arms_node->m_child(); valid_ptr(vm); vm = vm->m_next_sibling()) {
		auto* vm_owner = vm->m_owner();
		if (!valid_ptr(vm_owner))
			continue;

		auto* vm_entity = reinterpret_cast<c_base_entity*>(vm_owner);
		auto owner_handle = vm_entity->m_owner_entity();
		if (!owner_handle.is_valid())
			continue;

		if (g_interfaces->m_entity_system->get_base_entity(owner_handle.get_entry_index()) == weapon)
			return vm_entity;
	}
	return nullptr;
}


void c_skin_changer::apply_skin(c_econ_entity* weapon, c_econ_item_view* item, int paint_kit_id, float wear, int seed, const char* custom_name, c_cs_player_pawn* local_pawn, uint16_t def_index, const char* model)
{
	//if (!def_index) // crashes if we remove a knife lol
	//	econ_item_attribute_manager::remove(item);

	if (def_index)
		item->m_definition_index() = def_index;

	item->m_item_id_high() = 0xFFFFFFFF;
	item->m_initialized() = true;           //m_bInitialized
	item->m_bDisallowSOCm() = false;		//m_bRestoreCustomMaterialAfterPrecache
	item->m_bRestoreCustomMaterialAfterPrecache() = true;

	if (model) {
		weapon->set_model(model);

		if (auto* hud_weapon = get_hud_weapon(weapon, local_pawn)) {
			hud_weapon->set_model(model);
		}
	}

	if ((def_index && def_index != WEAPON_KNIFE_T && def_index != WEAPON_KNIFE) || (!def_index)) {
		weapon->m_paint_kit() = paint_kit_id;
		weapon->m_wear() = wear;
		weapon->m_seed() = seed;
	}

	weapon->update_composite(true);

	//if (paint_kit_id > 0)
	//	econ_item_attribute_manager::create(item, paint_kit_id, wear, seed);

	if (custom_name && custom_name[0] != '\0')
	{
		strcpy_s(item->m_custom_name(), 161, custom_name);
		item->m_name_description_ptr() = 0;
		weapon->add_nametag_entity();
	}

	else
	{
		item->m_custom_name()[0] = '\0';
	}

	bool uses_old_model = false;
	if (paint_kit_id > 0)
		if (auto* pk = g_interfaces->m_source2_client->get_econ_item_system()->get_econ_item_schema()->get_paint_kits().find_by_key(paint_kit_id))
			uses_old_model = pk->uses_old_model();

	uint64_t mesh_mask = uses_old_model ? 2 : 1;

	if (auto* scene_node = weapon->m_scene_node())
		scene_node->set_mesh_group_mask(mesh_mask);
	if (auto* hud_weapon = get_hud_weapon(weapon, local_pawn))
		if (auto* hud_node = hud_weapon->m_scene_node())
			hud_node->set_mesh_group_mask(mesh_mask);

	if (def_index)
		weapon->update_subclass(def_index);
	weapon->update_skin(true);
	weapon->update_weapon_data();
}

void c_skin_changer::initialize() {
	if (m_initialized)
		return;

	if (!g_item_schema->is_initialized())
		g_item_schema->initialize();

	m_initialized = g_item_schema->is_initialized();
}

void c_skin_changer::custom_model()
{
	if(!g_cfg->bShouldUpdate)
		return;

	auto* local_pawn = reinterpret_cast<c_cs_player_pawn*>(g_ctx->m_local_pawn);
	if (!valid_ptr(local_pawn) || local_pawn->m_health() <= 0)
	{
		g_cfg->bShouldUpdate = false;
		return;
	}

	auto observer_service = local_pawn->m_observer_services();
	if (!observer_service || observer_service->m_observer_mode() == 0)
	{
		g_cfg->bShouldUpdate = false;
		return;
	}

	auto observer_target = g_interfaces->m_game_resource->pGameEntitySystem->Get<c_cs_player_pawn>(observer_service->m_observer_target());
	if (!valid_ptr(observer_target) || observer_target->m_health() <= 0)
	{
		g_cfg->bShouldUpdate = false;
		return;
	}

	const char* szPathModel = g_cfg->sPathToModel.c_str();
	g_interfaces->m_resource_system->BlockingLoadResourceByName(szPathModel, 0);

	observer_target->set_model(szPathModel);

	g_cfg->bShouldUpdate = false;
}

void c_skin_changer::process_weapon(c_econ_entity* weapon, c_econ_item_view* item, c_cs_player_pawn* local_pawn, bool force_update, bool& did_update, uint64_t local_steam_id) {

	if (weapon->get_original_owner_xuid() != local_steam_id)
		return;
	auto* weapon_service = local_pawn->m_weapon_services();
	if (!valid_ptr(weapon_service))
		return;
	auto& my_weapons = weapon_service->my_weapons();


	uint16_t def_index = item->m_definition_index();
	int config_index = c_config::skin_changer_t::get_config_index(def_index);
	if (config_index == 0)
		return;

	auto& skin = g_cfg->skin_changer.weapon_skins[config_index];
	if (skin.paint_kit == 0)
		return;

	int paint_kit_id = g_item_schema->get_paint_kit_id_for_item(def_index, skin.paint_kit);
	if (paint_kit_id == 0 || (weapon->m_paint_kit() == paint_kit_id && !force_update))
		return;

	apply_skin(weapon, item, paint_kit_id, skin.wear, skin.seed, skin.custom_name, local_pawn);
	c_hud::clear_hud_weapon_icon_for(weapon);
	did_update = true;
}

void c_skin_changer::process_knife(c_econ_entity* weapon, c_econ_item_view* item, c_cs_player_pawn* local_pawn, bool force_update, bool& did_update, uint64_t local_steam_id) {
	if (g_cfg->knife_changer.m_knife == 0)
		return;
	if (!g_item_schema->is_initialized()
		|| g_cfg->knife_changer.m_knife >= (int)g_item_schema->knives.size())
		return;

	const uint16_t def_index = item->m_definition_index();
	const uint16_t selected_knife = g_item_schema->knives[g_cfg->knife_changer.m_knife].definition_index;
	if (selected_knife == 0)
		return;

	int paint_kit_id = g_item_schema->get_paint_kit_id_for_item(selected_knife, g_cfg->knife_changer.m_paint_kit);
	bool config_changed = (m_last_knife != selected_knife) ||
		(m_last_knife_paint_kit_id != paint_kit_id) ||
		(m_last_knife_wear != g_cfg->knife_changer.m_wear) ||
		(m_last_knife_seed != g_cfg->knife_changer.m_seed);

	if (def_index == selected_knife && !config_changed && !force_update)
		return;

	if (selected_knife != WEAPON_KNIFE_T && selected_knife != WEAPON_KNIFE)
		item->m_entity_quality() = QUALITY_UNUSUAL;

	m_last_knife = selected_knife;
	m_last_knife_paint_kit_id = paint_kit_id;
	m_last_knife_wear = g_cfg->knife_changer.m_wear;
	m_last_knife_seed = g_cfg->knife_changer.m_seed;

	apply_skin(weapon, item, paint_kit_id, g_cfg->knife_changer.m_wear, g_cfg->knife_changer.m_seed, g_cfg->knife_changer.m_custom_name, local_pawn, selected_knife, g_item_schema->knives[g_cfg->knife_changer.m_knife].model_path);
	c_hud::clear_hud_weapon_icon_for(weapon);
	did_update = true;
}

void c_skin_changer::run(int stage) {

	const bool skin_enabled = g_cfg->skin_changer.m_enabled;
	const bool knife_enabled = g_cfg->knife_changer.m_enabled;

	auto game_client = g_interfaces->m_network_client->GetNetworkGameClient();

	if (!game_client)
		return;

	//player model changer
	this->custom_model();

	if ((!skin_enabled && !knife_enabled) || !g_ctx->m_local_pawn || !g_interfaces->m_network_client)
		return;

	auto* local_pawn = reinterpret_cast<c_cs_player_pawn*>(g_ctx->m_local_pawn);
	if (!valid_ptr(local_pawn) || local_pawn->m_health() <= 0)
		return;

	auto observer_service = local_pawn->m_observer_services();
	if (!observer_service || observer_service->m_observer_mode() == 0)
		return;

	auto observer_target = g_interfaces->m_game_resource->pGameEntitySystem->Get<c_cs_player_pawn>(observer_service->m_observer_target());
	if (!valid_ptr(observer_target) || observer_target->m_health() <= 0)
		return;

	auto* controller = observer_target->get_controller();
	if (!controller) return;

	g_ctx->m_observer_target = controller;

	const float current_spawn_time = observer_target->m_last_spawn_time_index();
	const int   current_team = observer_target->m_team_num();
	uint64_t    local_steam_id = controller->m_steam_id();


	const bool team_changed = (current_team != m_last_team) && m_last_team != 0;
	const bool spawn_changed = (current_spawn_time != m_last_spawn_time) && m_last_spawn_time != 0.0f;


	static auto last_cfg_skin = g_cfg->skin_changer;
	static auto last_cfg_knife = g_cfg->knife_changer;


	bool config_changed = memcmp(&last_cfg_skin, &g_cfg->skin_changer, sizeof(last_cfg_skin)) != 0 ||
		memcmp(&last_cfg_knife, &g_cfg->knife_changer, sizeof(last_cfg_knife)) != 0;

	std::vector<uint16_t> current_weapon_indices;
	auto* weapon_service = observer_target->m_weapon_services();
	if (valid_ptr(weapon_service)) {
		auto& my_weapons = weapon_service->my_weapons();
		for (unsigned int i = 0; i < my_weapons.m_size; i++) {
			auto* weapon = reinterpret_cast<c_econ_entity*>(g_interfaces->m_entity_system->get_base_entity(my_weapons.m_elements[i].get_entry_index()));
			if (!weapon) continue;

			auto* item = weapon->m_attribute_manager()->m_item();
			if (valid_ptr(item)) {
				current_weapon_indices.push_back(item->m_definition_index());
			}
		}
	}

	bool weapon_changed = (current_weapon_indices != m_last_weapon_indices);

	m_last_weapon_indices = current_weapon_indices;

	if (team_changed || spawn_changed || should_update || weapon_changed || config_changed) {
		m_update_frames = 6;
		should_update = false;

		last_cfg_skin = g_cfg->skin_changer;
		last_cfg_knife = g_cfg->knife_changer;
	}
	auto sync_pawn_state = [&]() {
		m_last_spawn_time = current_spawn_time;
		m_last_team = current_team;
		};

	if (m_update_frames <= 0) {
		sync_pawn_state();
		return;
	}

	if (!valid_ptr(weapon_service)) {
		sync_pawn_state();
		return;
	}

	auto& my_weapons = weapon_service->my_weapons();
	auto* entity_system = g_interfaces->m_entity_system;
	std::vector<std::pair<c_econ_item_view*, uint32_t>> restore_list;
	bool did_update = false;

	for (unsigned int i = 0; i < my_weapons.m_size; i++) {
		auto* weapon = reinterpret_cast<c_econ_entity*>(
			entity_system->get_base_entity(my_weapons.m_elements[i].get_entry_index())
			);
		if (!weapon) continue;

		auto* item = weapon->m_attribute_manager()->m_item();
		if (!valid_ptr(item)) continue;

		const uint16_t def_index = item->m_definition_index();
		const bool is_knife = (def_index == WEAPON_KNIFE || def_index == WEAPON_KNIFE_T || (def_index >= 500 && def_index <= 526));

		if (is_knife && knife_enabled)
			process_knife(weapon, item, observer_target, true, did_update, local_steam_id); // idk y force update is set to true here...
		else if (!is_knife && skin_enabled)
			process_weapon(weapon, item, observer_target, true, did_update, local_steam_id);
	}

	if (did_update)
		c_hud::regenerate_skins();

	sync_pawn_state();
	m_update_frames--;
}