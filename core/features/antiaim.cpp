#include "antiaim.hpp"
#define TIME_TO_TICKS(t) ((int)(0.5f + (float)(t) / interfaces::globals->interval_per_tick))
#define TICKS_TO_TIME(t) (interfaces::globals->interval_per_tick * (t))

float get_fov(const vec3_t& view_angle, const vec3_t& aim_angle)
{
	vec3_t delta = aim_angle - view_angle;
	delta.normalizebutcool();

	return min(sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f)), 180.0f);
}

float antiaim::at_target(c_usercmd* cmd) {
	float best_fov = FLT_MAX;
	player_t* best_player = nullptr;
	int x, y;
	interfaces::engine->get_screen_size(x, y);
	vec3_t viewangles;
	interfaces::engine->get_view_angles(viewangles);
	for (int i = 1; i < interfaces::globals->max_clients; i++) {
		player_t* player = (player_t*)interfaces::entity_list->get_client_entity(i);

		if (!player || !player->is_alive() || player->dormant() || player->team() == csgo::local_player->team() || player == csgo::local_player || player->has_gun_game_immunity())
			continue;

		float fov = get_fov(viewangles, math::calculate_angle(csgo::local_player->get_eye_pos(), player->origin()));

		if (fov < best_fov) {
			best_fov = fov;
			best_player = player;
		}
	}
	if (!best_player)
		return cmd->viewangles.y - 180;

	return math::calculate_angle(csgo::local_player->get_eye_pos(), best_player->origin()).y + 180;
}


void antiaim::run(c_usercmd* cmd) {

	if (!csgo::local_player->is_alive() || csgo::local_player->flags() & fl_frozen || csgo::local_player->has_gun_game_immunity())
		return;

	if (cmd->buttons & in_use) 
		return;

	if (csgo::local_player->move_type() == movetype_ladder || csgo::local_player->move_type() == movetype_noclip)
		return;

	float servertime = TICKS_TO_TIME(csgo::local_player->get_tick_base());
	
	weapon_t* weapon = csgo::local_player->active_weapon();

	if (!weapon)
		return;

	weapon_info_t* weapon_info = weapon->get_weapon_data();

	csgo::bsendpacket = true;

	if (csgo::local_player->can_shoot() && cmd->buttons & in_attack)
		return;
	
	if (weapon_info && weapon_info->nWeaponType == WEAPONTYPE_KNIFE) 
		if ((cmd->buttons & in_attack && weapon->next_primary_attack() <= servertime) || (cmd->buttons & in_attack2 && weapon->next_secondary_attack() <= servertime))
			return;
	
	if (weapon_info && weapon_info->nWeaponType == WEAPONTYPE_GRENADE)
		if (!weapon->pin_pulled() || cmd->buttons & (in_attack | in_attack2))
			if (weapon->throwtime() > 0.f)
				return;

	csgo::bsendpacket = false;

	sentview = cmd->viewangles;

	if (csgo::local_player->get_animation_stateb()->m_flVelocityLengthXY > 0.1f)
		nextlbyupdate = servertime + 0.22f;
	else if (std::fabsf(std::remainderf(csgo::local_player->get_animation_stateb()->m_flFootYaw - csgo::local_player->get_animation_stateb()->m_flEyeYaw, 360.f)) > 35.f && servertime > nextlbyupdate)
		nextlbyupdate = servertime + 1.1f;

	bool lbyupdate = servertime > nextlbyupdate;
	float desync = 0;
	if (lbyupdate)
	{
		if (interfaces::clientstate->choked_commands == 0)
			csgo::bsendpacket = false;

		desync += 120.f;
	}
	else {
		if (interfaces::clientstate->choked_commands >= 14) {
			desync += (csgo::local_player->max_desync_delta()) / 2;
		}
		else {
			desync -= (csgo::local_player->max_desync_delta()) / 2;
		}
	}
	sentview = vec3_t( 89, at_target(cmd) + desync, 0 );


	cmd->viewangles = sentview;
}