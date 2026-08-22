#include <vector>
#include <utility>

#include "shared.hpp"

void c_changer::run() {
	auto game_client = g_interfaces->m_network_client->GetNetworkGameClient();
	if (!game_client)
		return;

	if (!g_ctx->m_local_pawn || !g_interfaces->m_network_client || !g_item_schema->is_initialized())
		return;

	auto* local_pawn = reinterpret_cast<c_cs_player_pawn*>(g_ctx->m_local_pawn);
	if (!valid_ptr(local_pawn) || local_pawn->m_health() <= 0)
		return;

	auto observer_service = local_pawn->m_observer_services();
	if (!observer_service || !valid_ptr(observer_service) || observer_service->m_observer_mode() == 0)
		return;

	auto observer_target = g_interfaces->m_game_resource->pGameEntitySystem->Get<c_cs_player_pawn>(observer_service->m_observer_target());
	if (!observer_target || !valid_ptr(observer_target) || observer_target->m_health() <= 0)
		return;

	g_ctx->m_observer_target = observer_target;

	engine_data cur_engine_data;
	cur_engine_data.m_spawn_time = observer_target->m_last_spawn_time_index();
	cur_engine_data.m_team = observer_target->m_team_num();
	cur_engine_data.m_demo_tick = g_interfaces->m_engine2_to_client->GetCurrentDemoTick();

	auto* controller = observer_target->get_controller();

	g_agent_changer->run(observer_target, cur_engine_data);

	if (controller) {
		cur_engine_data.m_steam_id = controller->m_steam_id();

		g_glove_changer->run(observer_target, cur_engine_data);
		g_skin_changer->run(observer_target, cur_engine_data);
	}

	general_engine_backup = cur_engine_data;
}