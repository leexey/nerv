#include "agent_changer.hpp"
#include "../shared/econ_item_attribute_manager.hpp"
#include "../../valve/interfaces/interfaces.hpp"

void c_agent_changer::custom_model(c_cs_player_pawn* observer_target, engine_data current)
{
	if (g_cfg->custom_model.m_model_path.empty() || g_cfg->agent_changer.m_enabled || !g_cfg->custom_model.m_enabled)
		return;

	if (!g_cfg->custom_model.m_should_update && !current.state_changed(backup_engine_data) && current.m_demo_tick >= general_engine_backup.m_demo_tick /*simple workaraound, idk how to actually fix it rn. prob its possible to check the model name and compare it -> https://www.unknowncheats.me/forum/counter-strike-2-a/606375-reading-player-model-name.html*/)
		return;

	const char* szPathModel = g_cfg->custom_model.m_model_path.c_str();
	g_interfaces->m_resource_system->BlockingLoadResourceByName(szPathModel, 0);

	observer_target->set_model(szPathModel);

	backup_engine_data = current;
	g_cfg->custom_model.m_should_update = false;
}

void c_agent_changer::agent_model(c_cs_player_pawn* observer_target, engine_data current)
{
	if (!g_cfg->agent_changer.m_enabled)
		return;

	auto* identity = observer_target->m_entity();
	if (!identity || !identity->is_valid())
		return;

	if (!identity->is_safe_to_modify())
		return;

	if (g_cfg->agent_changer.m_agent >= (int)g_item_schema->agents.size())
		return;

	const char* agent_model_path = g_item_schema->agents[g_cfg->agent_changer.m_agent].model_path;
	if (agent_model_path[0] == '\0')
		return;

	const bool config_changed = memcmp(&agent_backup_cfg, &g_cfg->agent_changer, sizeof(agent_backup_cfg)) != 0;

	if (current.state_changed(backup_engine_data) || config_changed || current.m_demo_tick < general_engine_backup.m_demo_tick /*simple workaraound, idk how to actually fix it rn. prob its possible to check the model name and compare it -> https://www.unknowncheats.me/forum/counter-strike-2-a/606375-reading-player-model-name.html*/)
		should_update = true;

	if (!should_update)
		return;

	observer_target->set_model(agent_model_path);

	agent_backup_cfg = g_cfg->agent_changer;
	backup_engine_data = current;

	should_update = false;
}

void c_agent_changer::run(c_cs_player_pawn* observer_target, engine_data current) {
	this->custom_model(observer_target, current);
	this->agent_model(observer_target, current);
}
