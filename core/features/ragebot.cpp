#include "ragebot.hpp"
#define TIME_TO_TICKS(t) ((int)(0.5f + (float)(t) / interfaces::globals->interval_per_tick))

static float getLerp() noexcept
{
	const float updateRate = interfaces::console->get_convar("cl_updaterate")->GetFloat(); //64
	const float maxUpdateRate = interfaces::console->get_convar("sv_maxupdaterate")->GetFloat(); //64
	const float interp = interfaces::console->get_convar("cl_interp")->GetFloat(); //0.031
	const float interpRatio = interfaces::console->get_convar("cl_interp_ratio")->GetFloat(); // 2
	const float minInterpRatio = interfaces::console->get_convar("sv_client_min_interp_ratio")->GetFloat(); // 1
	const float maxInterpRatio = interfaces::console->get_convar("sv_client_max_interp_ratio")->GetFloat(); //5

	float ratio = std::clamp(interpRatio, minInterpRatio, maxInterpRatio);
	float updaterate = (maxUpdateRate ? maxUpdateRate : updateRate);
	return max(interp, (ratio / updaterate));
}

bool valid(float simtime) noexcept
{

	static convar* sv_maxunlag = interfaces::console->get_convar("sv_maxunlag");

	const i_net_channel_info* network = interfaces::engine->get_net_channel_info();
	if (!network)
		return false;
	
	float delta = std::clamp(network->get_latency(0) + network->get_latency(1) + getLerp(), 0.f, sv_maxunlag->GetFloat()) - (interfaces::globals->cur_time - simtime);
	return std::abs(delta) <= 0.2f;
}

int hitbox_to_group(int hitbox) {
	switch (hitbox) {
		case hitbox_head:
		case hitbox_neck:
			return hitgroup_head;
		case hitbox_upper_chest:
		case hitbox_chest:
		case hitbox_lower_chest:
			return hitgroup_chest;
		case hitbox_pelvis:
		case hitbox_stomach:
			return hitgroup_stomach;
		case hitbox_left_calf:
		case hitbox_left_foot:
		case hitbox_left_thigh:
			return hitgroup_leftleg;
		case hitbox_right_calf:
		case hitbox_right_foot:
		case hitbox_right_thigh:
			return hitgroup_rightleg;
		case hitbox_left_forearm:
		case hitbox_left_hand:
		case hitbox_left_upper_arm:
			return hitgroup_leftarm;
		case hitbox_right_forearm:
		case hitbox_right_hand:
		case hitbox_right_upper_arm:
			return hitgroup_rightarm;
		default:
			return hitgroup_generic;
	}
}
bool hitchance(vec3_t& ang,weapon_t*& weap, vec3_t& pos, player_t*& ent, int hitbox) {
	vec3_t forward = { };
	vec3_t right = { };
	vec3_t up = { };
	math::angle_vectors(ang, &forward, &right, &up);

	forward.normalizebetter();
	right.normalizebetter();
	up.normalizebetter();
	//constexpr int hitc = 70;
	int hitc = (int)variables::hitchance;
	int hits = 0;
	const float dist = csgo::local_player->get_eye_pos().distance_to(pos);
	const float spread = weap->get_spread();
	const float inacc = weap->inaccuracy();

	const int hitgroup = hitbox_to_group(hitbox);
	//weapon_info_t* winfo = weap->get_weapon_data();//CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(pWeapon->GetItemDefinitionIndex());
	
	//if (dist > winfo->weapon_range) return false;

	for (int i = 0; i < 100; ++i)
	{
		float b = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2.0f * M_PI));
		float c = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
		float d = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2.0f * M_PI));

		float spread_val = c * spread;
		float inaccuracy_val = c * inacc;

		vec3_t v_spread = vec3_t((cos(b) * spread_val) + (cos(d) * inaccuracy_val), (sin(b) * spread_val) + (sin(d) * inaccuracy_val), 0.0f);
		vec3_t direction;

		direction.x = forward.x + (right.x * v_spread.x) + (up.x * v_spread.y); // 
		direction.y = forward.y + (right.y * v_spread.x) + (up.y * v_spread.y);
		direction.z = forward.z + (right.z * v_spread.x) + (up.z * v_spread.y); //-V778w

		direction.normalizebetter();

		vec3_t spread_view;
		math::vector_angles(direction, spread_view);
		spread_view.normalized();

		vec3_t end;
		vec3_t temp = ang - (spread_view - ang);
		math::angle_vectors(temp, &end, nullptr, nullptr);
		end.normalizebetter();
		vec3_t trace_beg = csgo::local_player->get_eye_pos();
		vec3_t trace_end = trace_beg + end * dist;
		
		trace_t trace{};
		ray_t ray{}; 
		ray.initialize(trace_beg, trace_end);
		trace_filter_skip_one_entity filter(csgo::local_player);
		interfaces::trace_ray->clip_ray_to_entity(ray, (MASK_SHOT), ent, &trace);
		if (trace.entity == ent && trace.hitGroup == hitgroup) {
			hits++;
		}

		if (hits >= hitc)
			return true;

		if (i - hits > (100 - hitc))
			return false;

	}

	return false;
}

ragebot::record ragebot::lastvalidrecord(std::deque<ragebot::record> recorddeque) {
	reverse(recorddeque.begin(), recorddeque.end());

	for (ragebot::record rec : recorddeque) {
		if (valid(rec.simtime))
			return rec;
	}
	
	return recorddeque.at(0);
}

void ragebot::aimbot(c_usercmd* cmd) {
	if (!csgo::local_player->is_alive())
		return;

	weapon_t* weap = csgo::local_player->active_weapon();

	if (weap == nullptr) return;

	points.clear();
	int maxplayers = 16;
	for (std::deque<record> recorddeque : records) {
		if (maxplayers <= 0) break;
		if (recorddeque.size() == 0) continue;
		record recarr[] = { lastvalidrecord(recorddeque), recorddeque.front() };
		for (record rec : recarr) {
		
			if (!rec.player->is_alive() || rec.player->dormant()) {
				recorddeque.clear();
				break;
			}
			if (!valid(rec.simtime)) continue;
			vec3_t old_origin = rec.player->origin();
			vec3_t old_abs_origin = rec.player->abs_origin();
			vec3_t old_angles = rec.player->abs_angles();
			vec3_t old_mins = rec.player->collideable()->mins();
			vec3_t old_maxs = rec.player->collideable()->maxs();
			matrix_t* old_matrix = rec.player->CachedBoneData().base();

			rec.player->origin() = rec.vecOrigin;
			rec.player->set_abs_origin(rec.absOrigin);
			rec.player->set_abs_angles(rec.absAngles);
			rec.player->collideable()->mins() = rec.vecMins;
			rec.player->collideable()->maxs() = rec.vecMaxs;
			std::memcpy(rec.player->CachedBoneData().base(), rec.matrixdata.data(), sizeof(matrix_t) * rec.player->CachedBoneData().count());// * 128

			studio_hdr_t* pStudioHdr = interfaces::model_info->get_studio_model(rec.player->model());
			for (int i : hitboxes) {
				float hbRad = pStudioHdr->get_hitbox(i, 0)->radius;
				vec3_t center = rec.player->get_hitbox_position(i);
				vec3_t current_angles = math::calculate_angle(center, csgo::local_player->get_eye_pos());

				vec3_t forward = math::angle_vector(current_angles);

				vec3_t top = vec3_t(0.0f, 0.0f, 1.0f);
				vec3_t bottom = vec3_t(0.0f, 0.0f, -1.0f);

				vec3_t right;
				forward.crossproduct(forward, vec3_t(0.0f, 0.0f, 1.0f), right);
				vec3_t left = vec3_t(-right.x, -right.y, right.z);

				vec3_t top_right = top + right;
				vec3_t top_left = top + left;

				vec3_t bottom_right = bottom + right;
				vec3_t bottom_left = bottom + left;

				constexpr float scale = 0.85f;
				const float mult = hbRad * scale;

				switch (i) {
				case 0:
					rec.points = { center, center + top * mult };
					break;
				case 2:
					rec.points = { center, center + right * mult, center + left * mult };
					break;
				case 3:
					rec.points = { center, center + right * mult, center + left * mult };
					break;
				case 4:
					rec.points = { center, center + right * mult, center + left * mult };
					break;
				case 5:
					rec.points = { center, center + right * mult, center + left * mult };
					break;
				case 6:
					rec.points = { center, center + right * mult, center + left * mult };
					break;
				default:
					rec.points = { center, center + top * mult, center + left * mult, center + right * mult };
					break;
				}

				for (vec3_t point : rec.points) {
					autowall::fire_bullet_data_t data{};
					if (autowall::GetDamage(csgo::local_player, point, &data) > min(variables::mindamage, rec.player->health()))
						points.push_back(aimbotpoint(rec, point, data.enterTrace.hitbox));
				}
			}

			

			rec.player->origin() = old_origin;
			rec.player->set_abs_origin(old_abs_origin);
			rec.player->set_abs_angles(old_angles);
			rec.player->collideable()->mins() = old_mins;
			rec.player->collideable()->maxs() = old_maxs;
			std::memcpy(rec.player->CachedBoneData().base(), old_matrix, sizeof(old_matrix));
		}
		maxplayers--;
	}
	
	int chances = 5;
	for (aimbotpoint p : points) {
		if (!csgo::local_player->can_shoot()) break;
		if (!GetAsyncKeyState(VK_SPACE))
			misc::movement::stop(cmd);
																//interfaces::console->get_convar("weapon_recoil_scale")->GetFloat()
		vec3_t angle = math::calculate_angle((csgo::local_player->get_eye_pos()), p.point) - csgo::local_player->aim_punch_angle() * 2;

		if (--chances <= 0) break;

		vec3_t old_origin = p.rec.player->origin();
		vec3_t old_abs_origin = p.rec.player->abs_origin();
		vec3_t old_angles = p.rec.player->abs_angles();
		vec3_t old_mins = p.rec.player->collideable()->mins();
		vec3_t old_maxs = p.rec.player->collideable()->maxs();
		matrix_t* old_matrix = p.rec.player->CachedBoneData().base();

		p.rec.player->origin() = p.rec.vecOrigin;
		p.rec.player->set_abs_origin(p.rec.absOrigin);
		p.rec.player->set_abs_angles(p.rec.absAngles);
		p.rec.player->collideable()->mins() = p.rec.vecMins;
		p.rec.player->collideable()->maxs() = p.rec.vecMaxs;
		std::memcpy(p.rec.player->CachedBoneData().base(), p.rec.matrixdata.data(), sizeof(matrix_t) * p.rec.player->CachedBoneData().count()); // *128

		if (!hitchance(angle, weap, p.point, (p.rec.player), p.hitbox)) {
		
			p.rec.player->origin() = old_origin;
			p.rec.player->set_abs_origin(old_abs_origin);
			p.rec.player->set_abs_angles(old_angles);
			p.rec.player->collideable()->mins() = old_mins;
			p.rec.player->collideable()->maxs() = old_maxs;
			std::memcpy(p.rec.player->CachedBoneData().base(), old_matrix, sizeof(old_matrix));

			continue;
		
		}
		
		cmd->buttons |= in_attack;
		cmd->viewangles = angle;

		cmd->tick_count = TIME_TO_TICKS(p.rec.simtime + getLerp()) ; 
		if (!GetAsyncKeyState(0x58))
			csgo::bsendpacket = true;
	
		p.rec.player->origin() = old_origin;
		p.rec.player->set_abs_origin(old_abs_origin);
		p.rec.player->set_abs_angles(old_angles);
		p.rec.player->collideable()->mins() = old_mins;
		p.rec.player->collideable()->maxs() = old_maxs;
		std::memcpy(p.rec.player->CachedBoneData().base(), old_matrix, sizeof(old_matrix));
		if (!p.rec.dontdoos) {
			ragebot::oscham_t oscham;

			oscham.cur_time = 0;
			oscham.info = p.rec.info;
			oscham.matrixdata = p.rec.matrixdata;
			oscham.model_to_world = p.rec.model_to_world;
			oscham.state = p.rec.state;

			ragebot::oschams.push_back(oscham);
		}
		return;

	}
	
}

