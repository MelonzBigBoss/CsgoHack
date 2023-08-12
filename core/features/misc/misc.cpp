#include "../features.hpp"

void misc::movement::bunny_hop(c_usercmd* cmd) {
	if (!variables::bhop)
		return;

	const int move_type = csgo::local_player->move_type();

	if (move_type == movetype_ladder || move_type == movetype_noclip || move_type == movetype_observer)
		return;

	if (!(csgo::local_player->flags() & fl_onground))
		cmd->buttons &= ~in_jump;
};
void misc::movement::stop(c_usercmd* cmd) {
	vec3_t viewangles;
	interfaces::engine->get_view_angles(viewangles);
	vec3_t vel = csgo::local_player->velocity();
	vec3_t dir;
	math::vector_angles(vel, dir);
	dir.y = viewangles.y - dir.y;
	vec3_t ndir = math::angle_vector(dir);
	
	ndir = ndir * (-1 * vel.length());
	if ((csgo::local_player->flags() & fl_onground && !(cmd->buttons & in_jump))&& vel.length() > 30) {
		cmd->forwardmove = ndir.x * 4;
		cmd->sidemove =  ndir.y * 4;
	}
}
void misc::movement::autostrafe(c_usercmd* cmd) { //misc::movement::a

	if (csgo::local_player->move_type() == movetype_ladder || csgo::local_player->move_type() == movetype_noclip)
		return;

	if (csgo::local_player->flags() & fl_onground)
		return;

	float wish_yaw = cmd->viewangles.y;

	static auto cl_sidespeed = interfaces::console->get_convar("cl_sidespeed");
	auto side_speed = 450;//cl_sidespeed->GetFloat();

		static auto old_yaw = 0.0f;

		auto get_velocity_degree = [](float velocity)
		{
			auto tmp = RAD2DEG(atan2(0.0f, velocity));
			return std::clamp(tmp, 0.f, 90.f);
		};
		
		if (csgo::local_player->move_type() != movetype_walk)
			return;
		
		auto velocity = csgo::local_player->velocity();
		velocity.z = 0.0f;
		auto forwardmove = cmd->forwardmove;
		auto sidemove = cmd->sidemove;

		static auto flip = false;
		flip = !flip;

		auto turn_direction_modifier = flip ? 1.0f : -1.0f;

		if (forwardmove || sidemove)
		{
			
			cmd->forwardmove = 0.0f;
			cmd->sidemove = 0.0f;

			auto turn_angle = atan2(-sidemove, forwardmove);
			wish_yaw += turn_angle * M_RADPI;
		}
		else if (forwardmove) //-V550
			cmd->forwardmove = 0.0f;

		auto strafe_angle = RAD2DEG(atan(15.0f / velocity.length_2d()));
		std::clamp(strafe_angle, 0.f, 90.f);

		auto temp = vec3_t(0.0f, wish_yaw - old_yaw, 0.0f);
		temp.normalizebutcool();

		auto yaw_delta = temp.y;
		old_yaw = wish_yaw;

		auto abs_yaw_delta = fabs(yaw_delta);

		if (abs_yaw_delta <= strafe_angle || abs_yaw_delta >= 15.0f)
		{
			vec3_t velocity_angles;
			math::vector_angles(velocity, velocity_angles);

			temp = vec3_t(0.0f, wish_yaw - velocity_angles.y, 0.0f);
			temp.normalizebutcool();

			auto velocityangle_yawdelta = temp.y;
			auto velocity_degree = get_velocity_degree(velocity.length_2d());

			if (velocityangle_yawdelta <= velocity_degree || velocity.length_2d() <= 15.0f)
			{
				if (-velocity_degree <= velocityangle_yawdelta || velocity.length_2d() <= 15.0f)
				{
					wish_yaw += strafe_angle * turn_direction_modifier;
					cmd->sidemove = side_speed * turn_direction_modifier;
				}
				else
				{
					wish_yaw = velocity_angles.y - velocity_degree;
					cmd->sidemove = side_speed;
				}
			}
			else
			{
				wish_yaw = velocity_angles.y + velocity_degree;
				cmd->sidemove = -side_speed;
			}
		}
		else if (yaw_delta > 0.0f)
			cmd->sidemove = -side_speed;
		else if (yaw_delta < 0.0f)
			cmd->sidemove = side_speed;
}
void misc::movement::FixMove(c_usercmd* cmd, const vec3_t& wish_angles) {
	if (!csgo::local_player || csgo::local_player->move_type() == movetype_ladder || csgo::local_player->move_type() == movetype_noclip)
		return;
	
	vec3_t  move, dir;
	float   delta, len;
	vec3_t   move_angle;

	// roll nospread fix.
	if (!(csgo::local_player->flags() & fl_onground) && cmd->viewangles.z != 0.f)
		cmd->sidemove = 0.f;

	// convert movement to vector.
	move = { cmd->forwardmove, cmd->sidemove, 0.f };

	// get move length and ensure we're using a unit vector ( vector with length of 1 ).
	len = move.normalizebetterv2();

	if (!len)
		return;

	// convert move to an angle.
	math::vector_angles(move, move_angle);
	// calculate yaw delta.
	delta = (cmd->viewangles.y - wish_angles.y);

	// accumulate yaw delta.
	move_angle.y += delta;

	// calculate our new move direction.
	// dir = move_angle_forward * move_length
	math::angle_vectors(move_angle, &dir);
	// scale to og movement.
	dir *= len;

	// strip old flags.
	
	cmd->buttons &= ~(in_forward | in_back | in_moveleft | in_moveright);

	// fix ladder and noclip.
	if (csgo::local_player->move_type() == movetype_ladder) {
		// invert directon for up and down.
		if (cmd->viewangles.x >= 45.f && wish_angles.x < 45.f && std::abs(delta) <= 65.f)
			dir.x = -dir.x;

		// write to movement.
		cmd->forwardmove = dir.x;
		cmd->sidemove = dir.y;

		// set new button flags.
		if (cmd->forwardmove > 200.f)
			cmd->buttons |= in_forward;

		else if (cmd->forwardmove < -200.f)
			cmd->buttons |= in_back;

		if (cmd->sidemove > 200.f)
			cmd->buttons |= in_moveright;

		else if (cmd->sidemove < -200.f)
			cmd->buttons |= in_moveleft;
	}

	// we are moving normally.
	else {
		// we must do this for pitch angles that are out of bounds.
		if (cmd->viewangles.x < -90.f || cmd->viewangles.x > 90.f)
			dir.x = -dir.x;

		// set move.
		cmd->forwardmove = dir.x;
		cmd->sidemove = dir.y;

		// set new button flags.
		if (cmd->forwardmove > 0.f)
			cmd->buttons |= in_forward;

		else if (cmd->forwardmove < 0.f)
			cmd->buttons |= in_back;

		if (cmd->sidemove > 0.f)
			cmd->buttons |= in_moveright;

		else if (cmd->sidemove < 0.f)
			cmd->buttons |= in_moveleft;
	}
}
void misc::movement::autostrafegs(c_usercmd* cmd, vec3_t old_angs) {
	static int strafer_flags = 0;
	
	if (!!(csgo::local_player->flags() & fl_onground)) {
		strafer_flags = 0;
		return;
	}
	
	auto velocity = csgo::local_player->velocity();
	auto velocity_len = velocity.length_2d();

	if (velocity_len <= 0.0f) {
		strafer_flags = 0;
		return;
	}
	
	auto ideal_step = std::min < float >(90.0f, 845.5f / velocity_len);
	auto velocity_yaw = (velocity.y || velocity.x) ? RAD2DEG(atan2f(velocity.y, velocity.x)) : 0.0f;

	auto unmod_angles = old_angs;
	auto angles = old_angs;

	if (velocity_len < 2.0f && !!(cmd->buttons & in_jump))
		cmd->forwardmove = 450.0f;

	auto forward_move = cmd->forwardmove;
	auto onground = !!(csgo::local_player->flags() & fl_onground);

	if (forward_move || cmd->sidemove) {
		cmd->forwardmove = 0.0f;

		if (velocity_len != 0.0f && abs(velocity.z) != 0.0f) {
			if (!onground) {
			DO_IT_AGAIN:
				vec3_t fwd;
				math::angle_vectors(angles, &fwd);
				auto right = fwd.cross(vec3_t(0.0f, 0.0f, 1.0f));

				auto v262 = (fwd.x * forward_move) + (cmd->sidemove * right.x);
				auto v263 = (right.y * cmd->sidemove) + (fwd.y * forward_move);
				
				angles.y = (v262 || v263) ? RAD2DEG(atan2f(v263, v262)) : 0.0f;
			}
		}
	}

	auto yaw_to_use = 0.0f;

	strafer_flags &= ~4;

	if (!onground) {
		auto clamped_angles = angles.y;

		if (clamped_angles < -180.0f) clamped_angles += 360.0f;
		if (clamped_angles > 180.0f) clamped_angles -= 360.0f;

		yaw_to_use = old_angs.y;

		strafer_flags |= 4;
	}

	if (strafer_flags & 4) {
		auto diff = angles.y - yaw_to_use;

		if (diff < -180.0f) diff += 360.0f;
		if (diff > 180.0f) diff -= 360.0f;

		if (abs(diff) > ideal_step && abs(diff) <= 30.0f) {
			auto move = 450.0f;

			if (diff < 0.0f)
				move *= -1.0f;

			cmd->sidemove = move;
			return;
		}
	}

	auto diff = angles.y - velocity_yaw;

	if (diff < -180.0f) diff += 360.0f;
	if (diff > 180.0f) diff -= 360.0f;

	auto step = 0.6f * (ideal_step + ideal_step);
	auto side_move = 0.0f;

	if (abs(diff) > 170.0f && velocity_len > 80.0f || diff > step && velocity_len > 80.0f) {
		angles.y = step + velocity_yaw;
		cmd->sidemove = -450.0f;
	}
	else if (-step <= diff || velocity_len <= 80.0f) {
		if (strafer_flags & 1) {
			angles.y -= ideal_step;
			cmd->sidemove = -450.0f;
		}
		else {
			angles.y += ideal_step;
			cmd->sidemove = 450.0f;
		}
	}
	else {
		angles.y = velocity_yaw - step;
		cmd->sidemove = 450.0f;
	}

	if (!(cmd->buttons & in_back) && !cmd->sidemove)
		goto DO_IT_AGAIN;

	strafer_flags ^= (strafer_flags ^ ~strafer_flags) & 1;

	if (angles.y < -180.0f) angles.y += 360.0f;
	if (angles.y > 180.0f) angles.y -= 360.0f;

	FixMove(cmd, angles);
}