#include "agent_changer.hpp"
#include "../shared/econ_item_attribute_manager.hpp"
#include "../../valve/interfaces/interfaces.hpp"

void c_agent_changer::custom_model()
{
	if (!g_cfg->bShouldUpdate)
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

void c_agent_changer::run()
{
	this->custom_model();

	if (!g_cfg->agent_changer.m_enabled || !g_ctx->m_local_pawn)
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

	auto* identity = observer_target->m_entity();
	if (!identity || !identity->is_valid())
		return;

	if (!identity->is_safe_to_modify())
		return;

	if(!g_item_schema->is_initialized()
		|| g_cfg->agent_changer.m_agent >= (int)g_item_schema->agents.size())
		return;

	const char* agent_model_path = g_item_schema->agents[g_cfg->agent_changer.m_agent].model_path;
	if (agent_model_path[0] == '\0')
		return;

	const float current_spawn_time = observer_target->m_last_spawn_time_index();
	const int   current_team = observer_target->m_team_num();
	const bool team_changed = (current_team != m_last_team) && m_last_team != 0;
	const bool spawn_changed = (current_spawn_time != m_last_spawn_time);
	const bool pawn_state_changed = team_changed || spawn_changed;

	if ((pawn_state_changed) && !should_update)
		should_update = true;

	if(!should_update)
		return;

	observer_target->set_model(agent_model_path);

	m_last_spawn_time = current_spawn_time;
	m_last_team = current_team;
	should_update = false;
}
