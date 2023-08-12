#pragma once

#include "../../dependencies/utilities/csgo.hpp"
#include "../menu/variables.hpp"
#include "../features/features.hpp"

#define TICKS_TO_TIME(t) (interfaces::globals->interval_per_tick * (t))
#define TIME_TO_TICKS(t) ((int)(0.5f + (float)(t) /interfaces::globals->interval_per_tick))

enum collision_group_t {
	COLLISION_GROUP_NONE,
	COLLISION_GROUP_DEBRIS,				// Collides with nothing but world and static stuff
	COLLISION_GROUP_DEBRIS_TRIGGER,		// Same as debris, but hits triggers
	COLLISION_GROUP_INTERACTIVE_DEB,	// RIS, // Collides with everything except other interactive debris or debris
	COLLISION_GROUP_INTERACTIVE,		// Collides with everything except interactive debris or debris
	COLLISION_GROUP_PLAYER,
	COLLISION_GROUP_BREAKABLE_GLASS,
	COLLISION_GROUP_VEHICLE,
	COLLISION_GROUP_PLAYER_MOVEMENT,	// For HL2, same as Collision_Group_Player
	COLLISION_GROUP_NPC,				// Generic NPC group
	COLLISION_GROUP_IN_VEHICLE,			// for any entity inside a vehicle
	COLLISION_GROUP_WEAPON,				// for any weapons that need collision detection
	COLLISION_GROUP_VEHICLE_CLIP,		// vehicle clip brush to restrict vehicle movement
	COLLISION_GROUP_PROJECTILE,			// Projectiles!
	COLLISION_GROUP_DOOR_BLOCKER,		// Blocks entities not permitted to get near moving doors
	COLLISION_GROUP_PASSABLE_DOOR,		// Doors that the player shouldn't collide with
	COLLISION_GROUP_DISSOLVING,			// Things that are dissolving are in this group
	COLLISION_GROUP_PUSHAWAY,			// Nonsolid on client and server, pushaway in player code
	COLLISION_GROUP_NPC_ACTOR,			// Used so NPCs in scripts ignore the player.
	LAST_SHARED_COLLISION_GROUP
};

inline void tracehull(vec3_t src, vec3_t dst, vec3_t mins, vec3_t maxs, uint32_t mask, entity_t* ignore, int collisiongroup, trace_t* trace) {
	trace_filter_world_and_props_only filter;
	ray_t ray;
	ray.initialize(src, dst, mins, maxs);
	interfaces::trace_ray->trace_ray(ray, MASK_SHOT, &filter, trace);
}
inline void traceline(vec3_t src, vec3_t dst, uint32_t mask, entity_t* ignore, int collisiongroup, trace_t* trace) {
	ray_t ray;
	ray.initialize(src, dst);
	trace_filter_world_and_props_only filter;
	
	interfaces::trace_ray->trace_ray(ray, mask, &filter, trace);
}

class grenadewarning : public singleton< grenadewarning > {
public:
	struct data_t {
		data_t() = default;

		data_t(player_t* owner, int index, const vec3_t& origin, const vec3_t& velocity, float throw_time, int offset) : data_t() {
			m_owner = owner;
			m_index = index;

			predict(origin, velocity, throw_time, offset);
		}

		bool physics_simulate() {
			if (m_detonated)
				return true;

			const auto new_velocity_z = m_velocity.z - (interfaces::console->get_convar("sv_gravity")->GetFloat() * 0.4f) * interfaces::globals->interval_per_tick;

			const auto move = vec3_t(
				m_velocity.x * interfaces::globals->interval_per_tick,
				m_velocity.y * interfaces::globals->interval_per_tick,
				(m_velocity.z + new_velocity_z) / 2.f * interfaces::globals->interval_per_tick
			);

			m_velocity.z = new_velocity_z;

			auto trace = trace_t();

			physics_push_entity(move, trace);

			if (m_detonated)
				return true;

			if (trace.flFraction != 1.f) {
				update_path< true >();

				perform_fly_collision_resolution(trace);
			}

			return false;
		}

		void physics_trace_entity(const vec3_t& src, const vec3_t& dst, std::uint32_t mask, trace_t& trace) {
			tracehull(
				src, dst, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f },
				mask, m_owner, m_collision_group, &trace
			);

			if (trace.startSolid
				&& (trace.contents & CONTENTS_CURRENT_90)) {
				trace.clear();

				tracehull(
					src, dst, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f },
					mask & ~CONTENTS_CURRENT_90, m_owner, m_collision_group, &trace
				);
			}

			if (!trace.did_hit()
				|| !trace.entity
				|| !reinterpret_cast<entity_t*>(trace.entity)->is_player())
				return;

			trace.clear();

			traceline(src, dst, mask, m_owner, m_collision_group, &trace);
		}

		void physics_push_entity(const vec3_t& push, trace_t& trace) {
			physics_trace_entity(m_origin, m_origin + push,
				m_collision_group == COLLISION_GROUP_DEBRIS
				? (MASK_SOLID | CONTENTS_CURRENT_90) & ~CONTENTS_MONSTER
				: MASK_SOLID | CONTENTS_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE | CONTENTS_CURRENT_90 | CONTENTS_HITBOX,
				trace
			);

			if (trace.startSolid) {
				m_collision_group = collision_group_t::COLLISION_GROUP_INTERACTIVE_DEB;

				traceline(
					m_origin - push, m_origin + push,
					(MASK_SOLID | CONTENTS_CURRENT_90) & ~CONTENTS_MONSTER,
					m_owner, m_collision_group, &trace
				);
			}

			if (trace.flFraction) {
				m_origin = trace.end;
			}

			if (!trace.entity)
				return;

			if (reinterpret_cast<entity_t*>(trace.entity)->is_player()
				|| m_index != WEAPON_TAGRENADE && m_index != WEAPON_MOLOTOV && m_index != WEAPON_INCGRENADE)
				return;

			if (m_index != WEAPON_TAGRENADE
				&& trace.plane.normal.z < std::cos(DEG2RAD(30.0f))) //g_ctx.convars.weapon_molotov_maxdetonateslope->GetFloat()
				return;

			detonate< true >();
		}

		void perform_fly_collision_resolution(trace_t& trace) {
			auto surface_elasticity = 1.f;

			if (trace.entity) {
				if (trace.entity->breakable()) {
				BREAKTROUGH:
					m_velocity *= 0.4f;

					return;
				}

				const auto is_player = reinterpret_cast<entity_t*>(trace.entity)->is_player();
				if (is_player) {
					surface_elasticity = 0.3f;
				}

				if (trace.entity->index()) {
					if (is_player
						&& m_last_hit_entity == trace.entity) {
						m_collision_group = COLLISION_GROUP_DEBRIS;

						return;
					}

					m_last_hit_entity = trace.entity;
				}
			}

			auto velocity = vec3_t();

			const auto back_off = m_velocity.dot(trace.plane.normal) * 2.f;

			for (auto i = 0u; i < 3u; i++) {
				const auto change = trace.plane.normal[i] * back_off;

				velocity[i] = m_velocity[i] - change;

				if (std::fabs(velocity[i]) >= 1.f)
					continue;

				velocity[i] = 0.f;
			}

			velocity *= std::clamp< float >(surface_elasticity * 0.45f, 0.f, 0.9f);

			if (trace.plane.normal.z > 0.7f) {
				const auto speed_sqr = velocity.length_sqr();
				if (speed_sqr > 96000.f) {
					const auto l = velocity.normalized().dot(trace.plane.normal);
					if (l > 0.5f) {
						velocity *= 1.f - l + 0.5f;
					}
				}

				if (speed_sqr < 400.f) {
					m_velocity = vec3_t(0,0,0);
				}
				else {
					m_velocity = velocity;

					physics_push_entity(velocity * ((1.f - trace.flFraction) * interfaces::globals->interval_per_tick), trace);
				}
			}
			else {
				m_velocity = velocity;

				physics_push_entity(velocity * ((1.f - trace.flFraction) * interfaces::globals->interval_per_tick), trace);
			}

			if (m_bounces_count > 20)
				return detonate< false >();

			++m_bounces_count;
		}

		void think() {
			switch (m_index) {
			case WEAPON_SMOKEGRENADE:
				if (m_velocity.length_sqr() <= 0.01f) {
					detonate< false >();
				}

				break;
			case WEAPON_DECOY:
				if (m_velocity.length_sqr() <= 0.04f) {
					detonate< false >();
				}

				break;
			case WEAPON_FLASHBANG:
			case WEAPON_HEGRENADE:
			case WEAPON_MOLOTOV:
			case WEAPON_INCGRENADE:
				if (TICKS_TO_TIME(m_tick) > m_detonate_time) {
					detonate< false >();
				}

				break;
			}

			m_next_think_tick = m_tick + TIME_TO_TICKS(0.2f);
		}

		template < bool _bounced >
		void detonate() {
			m_detonated = true;

			update_path< _bounced >();
		}

		template < bool _bounced >
		void update_path() {
			m_last_update_tick = m_tick;

			m_path.emplace_back(m_origin, _bounced);
		}

		void predict(const vec3_t& origin, const vec3_t& velocity, float throw_time, int offset) {
			m_origin = origin;
			m_velocity = velocity;
			m_collision_group = COLLISION_GROUP_PROJECTILE;

			const auto tick = TIME_TO_TICKS(1.f / 30.f);

			m_last_update_tick = -tick;

			switch (m_index) {
			case WEAPON_SMOKEGRENADE: m_next_think_tick = TIME_TO_TICKS(1.5f); break;
			case WEAPON_DECOY: m_next_think_tick = TIME_TO_TICKS(2.f); break;
			case WEAPON_FLASHBANG:
			case WEAPON_HEGRENADE:
				m_detonate_time = 1.5f;
				m_next_think_tick = TIME_TO_TICKS(0.02f);

				break;
			case WEAPON_MOLOTOV:
			case WEAPON_INCGRENADE:
				m_detonate_time = 2.0f; // g_ctx.convars.molotov_throw_detonate_time->GetFloat()
				m_next_think_tick = TIME_TO_TICKS(0.02f);

				break;
			}

			for (; m_tick < TIME_TO_TICKS(60.f); ++m_tick) {
				if (m_next_think_tick <= m_tick) {
					think();
				}

				if (m_tick < offset)
					continue;

				if (physics_simulate())
					break;

				if (m_last_update_tick + tick > m_tick)
					continue;

				update_path< false >();
			}

			if (m_last_update_tick + tick <= m_tick) {
				update_path< false >();
			}

			m_expire_time = throw_time + TICKS_TO_TIME(m_tick);
		}

		bool draw() const;

		bool											m_detonated{};
		player_t* m_owner{};
		vec3_t											m_origin{}, m_velocity{};
		entity_t* m_last_hit_entity{};
		collision_group_t								m_collision_group{};
		float											m_detonate_time{}, m_expire_time{};
		int												m_index{}, m_tick{}, m_next_think_tick{},
			m_last_update_tick{}, m_bounces_count{};
		std::vector< std::pair< vec3_t, bool > >		m_path{};
	} m_data{};

	std::unordered_map< unsigned long, data_t > m_list{};
public:
	grenadewarning() = default;

	const data_t& get_local_data() const { return m_data; }

	std::unordered_map< unsigned long, data_t >& get_list() { return m_list; }

	void do_warning(weapon_t* entity);
};