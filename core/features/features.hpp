#pragma once
#include "../../dependencies/utilities/csgo.hpp"
#include "../menu/variables.hpp"

namespace misc {
	namespace movement {
		void bunny_hop(c_usercmd* cmd);
		void autostrafe(c_usercmd* cmd);
		void FixMove(c_usercmd* cmd, const vec3_t& wish_angles);
		void autostrafegs(c_usercmd* cmd, vec3_t old_angs);
		void stop(c_usercmd* cmd);
	};
}
namespace autowall {
	struct fire_bullet_data_t
	{
		vec3_t			vecPosition = { };
		vec3_t			vecDirection = { };
		trace_t			enterTrace = { };
		float			flCurrentDamage = 0.0f;
		int				iPenetrateCount = 0;
	};
	float GetDamage(player_t* pLocal, vec3_t& vecPoint, fire_bullet_data_t* pDataOut = nullptr);
	void ScaleDamage(const int iHitGroup, player_t* pEntity, const float flWeaponArmorRatio, const float flWeaponHeadShotMultiplier, float &flDamage);
	bool SimulateFireBullet(player_t* pLocal, weapon_t* pWeapon, fire_bullet_data_t& data);
	void ClipTraceToPlayers(const vec3_t& vecAbsStart, const vec3_t& vecAbsEnd, const unsigned int fMask, i_trace_filter* pFilter, trace_t* pTrace, const float flMinRange = 0.0f);
	bool TraceToExit(trace_t& enterTrace, trace_t& exitTrace, vec3_t& vecPosition, vec3_t& vecDirection, player_t* pClipPlayer);
	bool HandleBulletPenetration(player_t* pLocal, weapon_info_t* pWeaponData, surfacedata_t* pEnterSurfaceData, fire_bullet_data_t& data);
}