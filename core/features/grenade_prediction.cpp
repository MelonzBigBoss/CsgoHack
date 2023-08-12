// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "grenade_prediction.hpp"

void GrenadePrediction::Tick(int buttons)
{
	this->data.throw_strength = std::clamp(csgo::local_player->active_weapon()->throwstrength(), 0.f, 1.f);
	this->data.act = ACT_NONE;

	auto bin_attack = buttons & in_attack;
	auto bin_attack2 = buttons & in_attack2;

	if (bin_attack || bin_attack2)
	{
		if (bin_attack && bin_attack2)
			this->data.act = ACT_LOB;
		else if (!bin_attack)
			this->data.act = ACT_DROP;
		else
			this->data.act = ACT_THROW;
	}
	else 
		this->data.act = ACT_THROW;
}

void GrenadePrediction::View(view_setup_t* setup, weapon_t* weapon)
{
	bool attack, attack2;
											
	if (csgo::local_player->is_alive() && csgo::pcmd)// && g_ctx.get_command()
	{
		attack = (csgo::pcmd->buttons & in_attack);
		attack2 = (csgo::pcmd->buttons & in_attack2);

		if (this->data.act != ACT_NONE && this->data.throw_strength >= -1.f)
		{
			this->data.type = weapon->item_definition_index();
			this->Simulate(setup);
		}
		else
			this->data.type = 0;
	}
}

inline float CSGO_Armor(float flDamage, int ArmorValue) {
	float flArmorRatio = 0.5f;
	float flArmorBonus = 0.5f;
	if (ArmorValue > 0) {
		float flNew = flDamage * flArmorRatio;
		float flArmor = (flDamage - flNew) * flArmorBonus;

		if (flArmor > static_cast<float>(ArmorValue)) {
			flArmor = static_cast<float>(ArmorValue) * (1.f / flArmorBonus);
			flNew = flDamage - flArmor;
		}

		flDamage = flNew;
	}
	return flDamage;
}

void GrenadePrediction::Paint()
{
	

	if (!csgo::local_player->is_alive())
		return;

	weapon_t* weapon = csgo::local_player->active_weapon();

	if (!weapon)
		return;

	if (path.size() < 2)
		return;

	trace_filter_skip_one_entity filter(csgo::local_player);

	std::pair <float, player_t*> target{ 0.f, nullptr };

	vec3_t prev = path[0];
	
	for (int i = 1; i < interfaces::globals->max_clients; ++i) {
		
		player_t* player = (player_t*)interfaces::entity_list->get_client_entity(i);
		if (!player || !player->is_player() || !player->is_alive() || player->team() == csgo::local_player->team())
			continue;
		// get center of mass for player.
		auto origin = player->origin();

		auto min = player->vecMins() + origin;
		auto max = player->vecMaxs() + origin;

		auto center = min + (max - min) * 0.5f;

		// get delta between center of mass and final nade pos.
		auto delta = center - path[path.size() - 1];

		if (weapon->item_definition_index() == WEAPON_HEGRENADE) {

			// is within damage radius?
			if (delta.length() > 350.f)
				continue;

			vec2_t NadeScreen;
			math::world_to_screen(path[path.size() - 1], NadeScreen);

			// main hitbox, that takes damage
			vec3_t vPelvis = player->get_hitbox_position(hitbox_pelvis);
			ray_t ray;
			ray.initialize(path[path.size() - 1], vPelvis);
			trace_t ptr;
			interfaces::trace_ray->trace_ray(ray, MASK_SHOT, &filter, &ptr);
			//trace to it

			if (ptr.entity == player) {
				vec2_t PelvisScreen;

				math::world_to_screen(vPelvis, PelvisScreen);

				// some magic values by VaLvO
				static float a = 105.0f;
				static float b = 25.0f;
				static float c = 140.0f;

				float d = ((delta.length() - b) / c);
				float flDamage = a * exp(-d * d);

				// do main damage calculation here
				auto dmg = max(static_cast<int>(ceilf(CSGO_Armor(flDamage, player->armor()))), 0);

				// clip max damage.
				dmg = min(dmg, (player->armor() > 0) ? 57 : 98);
				// max damage proof - https://counterstrike.fandom.com/wiki/HE_Grenade 

				// if we have target with more damage
				if (dmg > target.first) {
					target.first = dmg;
					target.second = player;
				}
			}
		}
	}

	// we have a target for damage.
	if (target.second && weapon->item_definition_index() == WEAPON_HEGRENADE) {
		vec2_t screen;

		if (math::world_to_screen(target.second->get_hitbox_position(hitbox_head), screen))
			render::text(screen.x - 4, screen.y - 28, render::fonts::watermark_font, std::to_string((int)target.first), true, target.first >= target.second->health() ? color::red() : color::white());
	}

	if (this->data.type && path.size() > 1)
	{
		vec2_t nadeStart, nadeEnd;

		vec3_t endpos = path[path.size() - 1];

	/*	if (weapon->item_definition_index() == WEAPON_MOLOTOV || weapon->item_definition_index() == WEAPON_INCGRENADE)
			render::get().Draw3DCircle(endpos, 120, g_cfg.esp.grenade_prediction_tracer_color, 1);
		else if (weapon->item_definition_index() == WEAPON_SMOKEGRENADE)
			render::get().Draw3DCircle(endpos, 144, g_cfg.esp.grenade_prediction_tracer_color, 1);
		else if (weapon->item_definition_index() == WEAPON_HEGRENADE)
			render::get().Draw3DCircle(endpos, 384, g_cfg.esp.grenade_prediction_tracer_color, 1);
		else if (weapon->item_definition_index() == WEAPON_FLASHBANG)
			render::get().Draw3DCircle(endpos, 180, g_cfg.esp.grenade_prediction_tracer_color, 1);
		else if (weapon->item_definition_index() == WEAPON_DECOY)
			render::get().Draw3DCircle(endpos, 60, g_cfg.esp.grenade_prediction_tracer_color, 1);
			*/
		for (auto it = path.begin(); it != path.end(); ++it)
		{
			if (math::world_to_screen(prev, nadeStart) && math::world_to_screen(*it, nadeEnd))
			{
				render::draw_line(nadeStart.x, nadeStart.y, nadeEnd.x, nadeEnd.y, color(255, 255, 255, 255));
			}
			prev = *it;
		}
	}
}

void GrenadePrediction::Setup(vec3_t& vecSrc, vec3_t& vecThrow, const vec3_t& viewangles)
{
	vec3_t angThrow = viewangles;
	float pitch = math::normalize_pitch(angThrow.x);

	float a = pitch - (90.0f - fabs(pitch)) * 10.0f / 90.0f;
	angThrow.x = a;

	float b = 750.0f * 0.9f;

	std::clamp(b, 15.f, 750.f);

	b *= ((this->data.throw_strength * 0.7f) + 0.3f);

	vec3_t vForward, vRight, vUp;
	math::angle_vectors(angThrow, &vForward, &vRight, &vUp);
	
	vecSrc = csgo::local_player->get_eye_pos();
	vecSrc.z += (this->data.throw_strength * 12.f) - 12.f;

	trace_t tr;
	vec3_t vecDest = vecSrc;
	vecDest += vForward * 22.0f;

	TraceHull(vecSrc, vecDest, tr);

	vec3_t vecBack = vForward; vecBack *= 6.0f;
	vecSrc = tr.end;
	vecSrc -= vecBack;
	
	vecThrow = csgo::local_player->velocity(); vecThrow *= 1.25f;
	vecThrow += vForward * b;
}

void GrenadePrediction::Simulate(view_setup_t* setup)
{
	vec3_t vecSrc, vecThrow;
	vec3_t angles; interfaces::engine->get_view_angles(angles);
	Setup(vecSrc, vecThrow, angles);
	
	float interval = interfaces::globals->interval_per_tick;
	int logstep = (int)(0.05f / interval);
	int logtimer = 0;

	if (!path.empty())
		path.clear();

	for (auto i = 0; i < 4096; ++i)
	{
		if (!logtimer)
			path.push_back(vecSrc);

		int s = Step(vecSrc, vecThrow, i, interval);

		if (s & 1)
			break;

		if (s & 2 || logtimer >= logstep)
			logtimer = 0;
		else
			++logtimer;

		if (vecThrow.x == 0 && vecThrow.y == 0 && vecThrow.z == 0)
			break;
	}

	path.push_back(vecSrc);
}

int GrenadePrediction::Step(vec3_t& vecSrc, vec3_t& vecThrow, int tick, float interval)
{
	vec3_t move; AddGravityMove(move, vecThrow, interval, false);
	trace_t tr; PushEntity(vecSrc, move, tr);

	int result = 0;

	if (CheckDetonate(vecThrow, tr, tick, interval))
		result |= 1;

	if (tr.flFraction != 1.0f) //-V550
	{
		result |= 2;
		ResolveFlyCollisionCustom(tr, vecThrow, move, interval);
	}

	vecSrc = tr.end;
	return result;
}

bool GrenadePrediction::CheckDetonate(vec3_t& vecThrow, trace_t& tr, int tick, float interval)
{
	auto time = TICKS_TO_TIME(tick);

	switch (this->data.type)
	{
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
		return time >= 1.5f && !(tick % TIME_TO_TICKS(0.2f));

	case WEAPON_SMOKEGRENADE:
		return vecThrow.length() <= 0.1f && !(tick % TIME_TO_TICKS(0.2f));

	case WEAPON_DECOY:
		return vecThrow.length() <= 0.2f && !(tick % TIME_TO_TICKS(0.2f));

	case WEAPON_MOLOTOV:
	case WEAPON_FIREBOMB:
		// detonate when hitting the floor.					/weapon_molotov_maxdetonateslope 30.0 
		if (tr.flFraction != 1.f && (std::cos(DEG2RAD(30.0f)) <= tr.plane.normal.z)) //-V550
			return true;

		// detonate if we have traveled for too long.
		// checked every 0.1s
		return time >= 30.0f && !(tick % TIME_TO_TICKS(0.1f));

	default:
		return false;
	}

	return false;
}

void GrenadePrediction::TraceHull(vec3_t& src, vec3_t& end, trace_t& tr)
{
	trace_filter_world_and_props_only filter;
	ray_t ray;
	ray.initialize(src, end, vec3_t(-2.0f, -2.0f, -2.0f), vec3_t(2.0f, 2.0f, 2.0f));
	interfaces::trace_ray->trace_ray(ray, MASK_SHOT, &filter, &tr);
	
}

void GrenadePrediction::AddGravityMove(vec3_t& move, vec3_t& vel, float frametime, bool onground)
{
	// gravity for grenades. sv_gravity
	
	float gravity = interfaces::console->get_convar("sv_gravity")->GetFloat() * 0.4f;

	// move one tick using current velocity.
	
	move.x = vel.x * interfaces::globals->interval_per_tick; //-V807
	move.y = vel.y * interfaces::globals->interval_per_tick;

	// apply linear acceleration due to gravity.
	// calculate new z velocity.
	float z = vel.z - (gravity * interfaces::globals->interval_per_tick);

	// apply velocity to move, the average of the new and the old.
	move.z = ((vel.z + z) / 2.f) * interfaces::globals->interval_per_tick;

	// write back new gravity corrected z-velocity.
	vel.z = z;
}

void GrenadePrediction::PushEntity(vec3_t& src, const vec3_t& move, trace_t& tr)
{
	vec3_t vecAbsEnd = src;
	vecAbsEnd += move;
	TraceHull(src, vecAbsEnd, tr);
}

void GrenadePrediction::ResolveFlyCollisionCustom(trace_t& tr, vec3_t& vecVelocity, const vec3_t& move, float interval)
{
	if (tr.entity)
	{
		
		if (tr.entity->breakable())
		{
			auto client_class = tr.entity->client_class();

			if (!client_class)
				return;

			auto network_name = client_class->network_name;

			if (strcmp(network_name, "CFuncBrush") && strcmp(network_name, "CBaseDoor") && strcmp(network_name, "CCSPlayer") && strcmp(network_name, "CBaseEntity")) //-V526
			{
				// move object.
				PushEntity(tr.end, move, tr);

				// deduct velocity penalty.
				vecVelocity *= 0.4f;
				return;
			}
		}
	}
	
	float flSurfaceElasticity = 1.0, flGrenadeElasticity = 0.45f;
	float flTotalElasticity = flGrenadeElasticity * flSurfaceElasticity;
	if (flTotalElasticity > 0.9f) flTotalElasticity = 0.9f;
	if (flTotalElasticity < 0.0f) flTotalElasticity = 0.0f;

	vec3_t vecAbsVelocity;
	PhysicsClipVelocity(vecVelocity, tr.plane.normal, vecAbsVelocity, 2.0f);
	vecAbsVelocity *= flTotalElasticity;

	float flSpeedSqr = vecAbsVelocity.length_sqr();
	static const float flMinSpeedSqr = 20.0f * 20.0f;

	if (flSpeedSqr < flMinSpeedSqr)
	{
		vecAbsVelocity.x = 0.0f;
		vecAbsVelocity.y = 0.0f;
		vecAbsVelocity.z = 0.0f;
	}

	if (tr.plane.normal.z > 0.7f)
	{
		vecVelocity = vecAbsVelocity;
		vecAbsVelocity *= ((1.0f - tr.flFraction) * interval);
		PushEntity(tr.end, vecAbsVelocity, tr);
	}
	else
		vecVelocity = vecAbsVelocity;
}

int GrenadePrediction::PhysicsClipVelocity(const vec3_t& in, const vec3_t& normal, vec3_t& out, float overbounce)
{
	static const float STOP_EPSILON = 0.1f;

	float backoff, change, angle;
	int   i, blocked;

	blocked = 0;
	angle = normal[2];

	if (angle > 0) blocked |= 1;
	if (!angle) blocked |= 2; //-V550
	backoff = in.dot(normal) * overbounce;
	for (i = 0; i < 3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0;
	}
	return blocked;
}