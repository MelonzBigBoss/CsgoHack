#include "engine_prediction.hpp"

void prediction::start(c_usercmd* cmd) {
	if (!csgo::local_player)
		return;

	if (!prediction_random_seed) 
		prediction_random_seed = *reinterpret_cast<int**>(utilities::pattern_scan("client.dll", sig_prediction_random_seed) + 2);

	*prediction_random_seed = cmd->randomseed & 0x7FFFFFFF;

	old_cur_time = interfaces::globals->cur_time;
	old_frame_time = interfaces::globals->frame_time;

	interfaces::globals->cur_time = csgo::local_player->get_tick_base() * interfaces::globals->interval_per_tick;
	interfaces::globals->frame_time = interfaces::globals->interval_per_tick;

	interfaces::game_movement->start_track_prediction_errors(csgo::local_player);

	memset(&data, 0, sizeof(data));
	interfaces::move_helper->set_host(csgo::local_player);
	interfaces::prediction->setup_move(csgo::local_player, cmd, interfaces::move_helper, &data);
	interfaces::game_movement->process_movement(csgo::local_player, &data);
	interfaces::prediction->finish_move(csgo::local_player, cmd, &data);
}

void prediction::end() {
	if (!csgo::local_player)
		return;

	interfaces::game_movement->finish_track_prediction_errors(csgo::local_player);
	interfaces::move_helper->set_host(nullptr);

	*prediction_random_seed = -1;

	interfaces::globals->cur_time = old_cur_time;
	interfaces::globals->frame_time = old_frame_time;
}

void prediction::store_viewmodel_data() {
	
	auto viewmodel = (base_view_model_t*)interfaces::entity_list->get_client_entity_handle(csgo::local_player->view_model());
	if (!viewmodel)
		return;

	animation_parity = viewmodel->m_nAnimationParity();
	sequence = viewmodel->sequence();
	cycle = viewmodel->cycle();
	animtime = viewmodel->animtime();
}

void prediction::adjust_viewmodel_data() {
	auto viewmodel = (base_view_model_t*)interfaces::entity_list->get_client_entity_handle(csgo::local_player->view_model());
	if (!viewmodel)
		return;

	if (sequence != viewmodel->sequence() || animation_parity != viewmodel->m_nAnimationParity())
		return;

	viewmodel->cycle() = cycle;
	viewmodel->animtime() = animtime;
}

bool is_vector_valid(vec3_t vecOriginal, vec3_t vecCurrent)
{
	vec3_t vecDelta = vecOriginal - vecCurrent;
	for (int i = 0; i < 3; i++)
	{
		if (fabsf(vecDelta[i]) > 0.03125f)
			return false;
	}

	return true;
}

bool is_float_valid(float_t flOriginal, float_t flCurrent)
{
	if (fabsf(flOriginal - flCurrent) > 0.03125f)
		return false;

	return true;
}

void prediction::store_netvar_data(int command_number)
{
	compress_data[command_number % 150].m_nTickbase = csgo::local_player->get_tick_base();
	compress_data[command_number % 150].m_flDuckAmount = csgo::local_player->duck_amount();
	compress_data[command_number % 150].m_flDuckSpeed = csgo::local_player->duck_speed();
	compress_data[command_number % 150].m_vecOrigin = csgo::local_player->origin();
	compress_data[command_number % 150].m_vecVelocity = csgo::local_player->velocity();
	compress_data[command_number % 150].m_vecBaseVelocity = csgo::local_player->base_velocity();
	compress_data[command_number % 150].m_flFallVelocity = csgo::local_player->fall_velocity();
	compress_data[command_number % 150].m_vecViewOffset = csgo::local_player->view_offset();
	compress_data[command_number % 150].m_angAimPunchAngle = csgo::local_player->aim_punch_angle();
	compress_data[command_number % 150].m_vecAimPunchAngleVel = csgo::local_player->aim_punch_angle_vel();
	compress_data[command_number % 150].m_angViewPunchAngle = csgo::local_player->punch_angle();

	auto weapon = csgo::local_player->active_weapon();
	if (!weapon)
	{
		compress_data[command_number % 150].m_flRecoilIndex = 0.0f;
		compress_data[command_number % 150].m_flAccuracyPenalty = 0.0f;
		compress_data[command_number % 150].m_flPostponeFireReadyTime = 0.0f;

		return;
	}

	compress_data[command_number % 150].m_flRecoilIndex = weapon->recoil_index();
	compress_data[command_number % 150].m_flAccuracyPenalty = weapon->accuracy_penalty();
	compress_data[command_number % 150].m_flPostponeFireReadyTime = weapon->ready_fire_time();
}

void prediction::restore_netvar_data(int command_number)
{
	volatile auto netvars = &compress_data[command_number % 150];
	
	if (netvars->m_nTickbase != csgo::local_player->get_tick_base())
		return;

	if (is_vector_valid(netvars->m_vecVelocity, csgo::local_player->velocity()))
		csgo::local_player->velocity() = netvars->m_vecVelocity;

	if (is_vector_valid(netvars->m_vecBaseVelocity, csgo::local_player->base_velocity()))
		csgo::local_player->base_velocity() = netvars->m_vecBaseVelocity;

	if (is_vector_valid(netvars->m_angAimPunchAngle, csgo::local_player->aim_punch_angle()))
		csgo::local_player->aim_punch_angle() = netvars->m_angAimPunchAngle;

	if (is_vector_valid(netvars->m_vecAimPunchAngleVel, csgo::local_player->aim_punch_angle_vel()))
		csgo::local_player->aim_punch_angle_vel() = netvars->m_vecAimPunchAngleVel;

	if (is_vector_valid(netvars->m_angViewPunchAngle, csgo::local_player->punch_angle()))
		csgo::local_player->punch_angle() = netvars->m_angViewPunchAngle;

	if (is_float_valid(netvars->m_flFallVelocity, csgo::local_player->fall_velocity()))
		csgo::local_player->fall_velocity() = netvars->m_flFallVelocity;

	if (is_float_valid(netvars->m_flDuckAmount, csgo::local_player->duck_amount()))
		csgo::local_player->duck_amount() = netvars->m_flDuckAmount;

	if (is_float_valid(netvars->m_flDuckSpeed, csgo::local_player->duck_speed()))
		csgo::local_player->duck_speed() = netvars->m_flDuckSpeed;

	if (csgo::local_player->active_weapon())
	{
		if (is_float_valid(netvars->m_flAccuracyPenalty, csgo::local_player->active_weapon()->accuracy_penalty()))
			csgo::local_player->active_weapon()->accuracy_penalty() = netvars->m_flAccuracyPenalty;

		if (is_float_valid(netvars->m_flRecoilIndex, csgo::local_player->active_weapon()->recoil_index()))
			csgo::local_player->active_weapon()->recoil_index() = netvars->m_flRecoilIndex;
	}
	
	if (csgo::local_player->flags() & fl_onground)
		csgo::local_player->fall_velocity() = 0.0f;
}