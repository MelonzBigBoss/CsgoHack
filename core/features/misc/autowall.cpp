#include "../features.hpp"

float autowall::GetDamage(player_t* pLocal, vec3_t& vecPoint, fire_bullet_data_t* pDataOut) {
	const vec3_t vecPosition = pLocal->get_eye_pos();

	// setup data
	autowall::fire_bullet_data_t data = { };
	data.vecPosition = vecPosition;
	data.vecDirection = (vecPoint - vecPosition);
	data.vecDirection.normalizebetter();

	if (weapon_t* pWeapon = pLocal->active_weapon(); pWeapon == nullptr || !SimulateFireBullet(pLocal, pWeapon, data))
		return -1.0f;

	if (pDataOut != nullptr)
		*pDataOut = data;

	return data.flCurrentDamage;
}
void autowall::ScaleDamage(const int iHitGroup, player_t* pEntity, const float flWeaponArmorRatio, const float flWeaponHeadShotMultiplier, float& flDamage) {
	const bool bHeavyArmor = pEntity->has_heavy_armor();
	float mp_damage_scale_ct_head = 1;// interfaces::console->get_convar("mp_damage_scale_ct_head")->GetInt();
	float mp_damage_scale_t_head = 1;// interfaces::console->get_convar("mp_damage_scale_t_head")->GetInt();

	float mp_damage_scale_ct_body = 1;// interfaces::console->get_convar("mp_damage_scale_ct_body")->GetInt();
	float mp_damage_scale_t_body = 1;//interfaces::console->get_convar("mp_damage_scale_t_body")->GetInt();
	
	float flHeadDamageScale = pEntity->team() == 3 ? mp_damage_scale_ct_head: pEntity->team() == 2 ? mp_damage_scale_t_head : 1.0f;
	const float flBodyDamageScale = pEntity->team() == 3 ? mp_damage_scale_ct_body: pEntity->team() == 2 ? mp_damage_scale_t_body : 1.0f;

	if (bHeavyArmor)
		flHeadDamageScale *= 0.5f;
	
	switch (iHitGroup)
	{
	case hitgroup_head:
		flDamage *= flWeaponHeadShotMultiplier * flHeadDamageScale;
		break;
	case hitgroup_chest:
	case hitgroup_leftarm:
	case hitgroup_rightarm:
	case hitgroup_neck:
		flDamage *= flBodyDamageScale;
		break;
	case hitgroup_stomach:
		flDamage *= 1.25f * flBodyDamageScale;
		break;
	case hitgroup_leftleg:
	case hitgroup_rightleg:
		flDamage *= 0.75f * flBodyDamageScale;
		break;
	default:
		break;
	}
	float newdamage = 0;
	if (pEntity->IsArmored(iHitGroup))
	{
		// @ida ontakedamage: server.dll @ 80 BF ? ? ? ? ? F3 0F 10 5C 24 ? F3 0F 10 35
		const int iArmor = pEntity->armor();
		float flHeavyArmorBonus = 1.0f, flArmorBonus = 0.5f, flArmorRatio = flWeaponArmorRatio * 0.5f;
		if (bHeavyArmor)
		{
			flHeavyArmorBonus = 0.25f;
			flArmorBonus = 0.33f;
			flArmorRatio *= 0.20f;
			newdamage = (flDamage * flArmorRatio) * 0.85;
		}
		else {
			newdamage = flDamage * flArmorRatio;
		}
		if (((flDamage - newdamage) * (flHeavyArmorBonus * flArmorBonus)) > iArmor)
			newdamage = flDamage - (iArmor / flArmorBonus);

		flDamage = newdamage;
	}

}
bool autowall::SimulateFireBullet(player_t* pLocal, weapon_t* pWeapon, fire_bullet_data_t& data) {
	weapon_info_t* pWeaponData = pWeapon->get_weapon_data();

	if (pWeaponData == nullptr)
		return false;

	float flMaxRange = pWeaponData->flRange;

	// the total number of surfaces any bullet can penetrate in a single flight is capped at 4
	data.iPenetrateCount = 4;
	// set our current damage to what our gun's initial damage reports it will do
	data.flCurrentDamage = (float)(pWeaponData->iDamage);

	float flTraceLenght = 0.0f;
	trace_filter_skip_one_entity filter(pLocal);

	while (data.iPenetrateCount > 0 && data.flCurrentDamage >= 1.0f)
	{
		// max bullet range
		flMaxRange -= flTraceLenght;
		// end position of bullet
		vec3_t vecEnd = data.vecPosition + data.vecDirection * flMaxRange;

		ray_t ray;
		ray.initialize(data.vecPosition, vecEnd);

		interfaces::trace_ray->trace_ray(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &data.enterTrace);

		// check for player hitboxes extending outside their collision bounds
		ClipTraceToPlayers(data.vecPosition, vecEnd + data.vecDirection * 40.0f, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &data.enterTrace);
		
		surfacedata_t* pEnterSurfaceData = interfaces::physics_props->GetSurfaceData(data.enterTrace.surface.surfaceProps);
		const float flEnterPenetrationModifier = pEnterSurfaceData->game.flPenetrationModifier;

		// we didn't hit anything, stop tracing shoot
		if (data.enterTrace.flFraction == 1.0f)
			break;

		// calculate the damage based on the distance the bullet traveled
		flTraceLenght += data.enterTrace.flFraction * flMaxRange;
		data.flCurrentDamage = data.flCurrentDamage * std::pow(pWeaponData->flRangeModifier, flTraceLenght / 500.0f);
		
		// check is actually can shoot through
		if (flTraceLenght > 3000.f || flEnterPenetrationModifier < 0.1f)
			break;

		// check is can do damage
		if (data.enterTrace.hitGroup != hitgroup_generic && data.enterTrace.hitGroup != hitgroup_gear && pLocal->team() != data.enterTrace.entity->team())
		{
			// we got target - scale damage
			ScaleDamage(data.enterTrace.hitGroup, data.enterTrace.entity, pWeaponData->flArmorRatio, pWeaponData->flHeadShotMultiplier, data.flCurrentDamage);
			return true;
		}

		// calling handlebulletpenetration here reduces our penetration ñounter, and if it returns true, we can't shoot through it
		if (!HandleBulletPenetration(pLocal, pWeaponData, pEnterSurfaceData, data))
			break;
	}

	return false;
}
void autowall::ClipTraceToPlayers(const vec3_t& vecAbsStart, const vec3_t& vecAbsEnd, const unsigned int fMask, i_trace_filter* pFilter, trace_t* pTrace, const float flMinRange) {
	trace_t trace = { };
	float flSmallestFraction = pTrace->flFraction;

	ray_t ray;
	ray.initialize(vecAbsStart, vecAbsEnd);
	for (int i = 1; i < interfaces::globals->max_clients; i++)
	{
		
		player_t* pEntity = (player_t*)interfaces::entity_list->get_client_entity(i);
		
		if (pEntity == nullptr || !pEntity->is_alive() || pEntity->dormant())
			continue;

		if (pFilter != nullptr && !pFilter->ShouldHitEntity(pEntity, fMask))
			continue;

		collideable_t* pCollideable = pEntity->collideable();

		if (pCollideable == nullptr)
			continue;
		
		// get bounding box
		vec3_t vecMin = pCollideable->mins();
		vec3_t vecMax = pCollideable->maxs();

		// calculate world space center
		vec3_t vecCenter = (vecMax + vecMin) * 0.5f;
		vec3_t vecPosition = vecCenter + pEntity->origin();

		vec3_t vecTo = vecPosition - vecAbsStart;
		vec3_t vecDirection = vecAbsEnd - vecAbsStart;
		float flLength = vecDirection.length();
		vecDirection.normalizebetter();

		float flRangeAlong = vecDirection.dot(vecTo);
		float flRange = 0.0f;

		// calculate distance to ray
		if (flRangeAlong < 0.0f)
			// off start point
			flRange = -vecTo.length();
		else if (flRangeAlong > flLength)
			// off end point
			flRange = -(vecPosition - vecAbsEnd).length();
		else
			// within ray bounds
			flRange = (vecPosition - (vecDirection * flRangeAlong + vecAbsStart)).length();

		constexpr float flMaxRange = 60.f;
		if (flRange < flMinRange || flRange > flMaxRange)
			continue;

		interfaces::trace_ray->clip_ray_to_entity(ray, fMask | CONTENTS_HITBOX, pEntity, &trace);

		if (trace.flFraction < flSmallestFraction)
		{
			// we shortened the ray - save off the trace
			*pTrace = trace;
			flSmallestFraction = trace.flFraction;
		}
	}
}
bool autowall::TraceToExit(trace_t& enterTrace, trace_t& exitTrace, vec3_t& vecPosition, vec3_t& vecDirection, player_t* pClipPlayer) {
	float flDistance = 0.0f;
	int iStartContents = 0;

	while (flDistance <= 90.0f)
	{
		// add extra distance to our ray
		flDistance += 4.0f;
		
		// multiply the direction vector to the distance so we go outwards, add our position to it
		vec3_t vecStart = vecPosition + vecDirection * flDistance;

		if (!iStartContents)
			iStartContents = interfaces::trace_ray->get_point_contents(vecStart, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);
		
		const int iCurrentContents = interfaces::trace_ray->get_point_contents(vecStart, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		if (!(iCurrentContents & MASK_SHOT_HULL) || (iCurrentContents & CONTENTS_HITBOX && iCurrentContents != iStartContents))
		{
			// setup our end position by deducting the direction by the extra added distance
			const vec3_t vecEnd = vecStart - (vecDirection * 4.0f);

			// trace ray to world
			ray_t rayWorld;
			rayWorld.initialize(vecStart, vecEnd);
			interfaces::trace_ray->trace_ray(rayWorld, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exitTrace);
			
			if (static convar* sv_clip_penetration_traces_to_players = interfaces::console->get_convar("sv_clip_penetration_traces_to_players"); sv_clip_penetration_traces_to_players != nullptr && sv_clip_penetration_traces_to_players->GetBool())
			{
				trace_filter_skip_one_entity filter(pClipPlayer);
				ClipTraceToPlayers(vecEnd, vecStart, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &exitTrace, -60.f);
			}

			// check if a hitbox is in-front of our enemy and if they are behind of a solid wall
			if (exitTrace.startSolid && exitTrace.surface.flags & SURF_HITBOX)
			{
				// trace ray to entity
				ray_t ray;
				ray.initialize(vecStart, vecPosition);
				trace_filter_skip_one_entity filter(exitTrace.entity);

				interfaces::trace_ray->trace_ray(ray, MASK_SHOT_HULL, &filter, &exitTrace);

				if (exitTrace.did_hit() && !exitTrace.startSolid)
				{
					vecStart = exitTrace.end;
					return true;
				}

				continue;
			}
			
			if (exitTrace.did_hit() && !exitTrace.startSolid)
			{
				if (enterTrace.entity->breakable() && exitTrace.entity->breakable())
					return true;

				if (enterTrace.surface.flags & SURF_NODRAW || (!(exitTrace.surface.flags & SURF_NODRAW) && exitTrace.plane.normal.dot(vecDirection) <= 1.0f))
				{
					const float flMultiplier = exitTrace.flFraction * 4.0f;
					vecStart -= vecDirection * flMultiplier;
					return true;
				}

				continue;
			}

			if (!exitTrace.did_hit() || exitTrace.startSolid)
			{
				if (enterTrace.entity != nullptr && enterTrace.entity->index() != 0 && enterTrace.entity->breakable())
				{
					// did hit breakable non world entity
					exitTrace = enterTrace;
					exitTrace.end = vecStart + vecDirection;
					return true;
				}

				continue;
			}
		}
	}

	return false;
}
bool autowall::HandleBulletPenetration(player_t* pLocal, weapon_info_t* pWeaponData, surfacedata_t* pEnterSurfaceData, fire_bullet_data_t& data) {
	
	static convar* ff_damage_reduction_bullets = interfaces::console->get_convar("ff_damage_reduction_bullets");
	static convar* ff_damage_bullet_penetration = interfaces::console->get_convar("ff_damage_bullet_penetration");

	const float flReductionDamage = 0;//ff_damage_reduction_bullets->float_value;
	const float flPenetrateDamage = 0; // ff_damage_bullet_penetration->float_value;

	const material_handle_t hEnterMaterial = pEnterSurfaceData->game.hMaterial;

	if (data.iPenetrateCount == 0 && hEnterMaterial != CHAR_TEX_GRATE && hEnterMaterial != CHAR_TEX_GLASS && !(data.enterTrace.surface.flags & SURF_NODRAW))
		return false;

	if (pWeaponData->flPenetration <= 0.0f || data.iPenetrateCount <= 0)
		return false;

	trace_t exitTrace = { };
	if (!TraceToExit(data.enterTrace, exitTrace, data.enterTrace.end, data.vecDirection, pLocal) && !(interfaces::trace_ray->get_point_contents(data.enterTrace.end, MASK_SHOT_HULL, nullptr) & MASK_SHOT_HULL))
		return false;
	
	const surfacedata_t* pExitSurfaceData = interfaces::physics_props->GetSurfaceData(exitTrace.surface.surfaceProps);
	const material_handle_t hExitMaterial = pExitSurfaceData->game.hMaterial;

	const float flEnterPenetrationModifier = pEnterSurfaceData->game.flPenetrationModifier;
	const float flExitPenetrationModifier = pExitSurfaceData->game.flPenetrationModifier;

	float flDamageLostModifier = 0.16f;
	float flPenetrationModifier = 0.0f;

	if ((hEnterMaterial == CHAR_TEX_GRATE) || (hEnterMaterial == CHAR_TEX_GLASS))
	{
		flDamageLostModifier = 0.05f;
		flPenetrationModifier = 3.0f;
	}
	else if (((data.enterTrace.contents >> 3) & CONTENTS_SOLID) || ((data.enterTrace.surface.flags >> 7) & SURF_LIGHT))
	{
		flDamageLostModifier = 0.16f;
		flPenetrationModifier = 1.0f;
	}
	else if ((hEnterMaterial == CHAR_TEX_FLESH) && (flReductionDamage == 0.0f) && (data.enterTrace.entity != nullptr) && (data.enterTrace.entity->is_player()) && (pLocal->team() == data.enterTrace.entity->team()))
	{
		if (flPenetrateDamage == 0.0f)
			return false;

		// shoot through teammates
		flDamageLostModifier = flPenetrateDamage;
		flPenetrationModifier = flPenetrateDamage;
	}
	else
	{
		flDamageLostModifier = 0.16f;
		flPenetrationModifier = (flEnterPenetrationModifier + flExitPenetrationModifier) * 0.5f;
	}

	if (hEnterMaterial == hExitMaterial)
	{
		if (hExitMaterial == CHAR_TEX_CARDBOARD || hExitMaterial == CHAR_TEX_WOOD)
			flPenetrationModifier = 3.0f;
		else if (hExitMaterial == CHAR_TEX_PLASTIC)
			flPenetrationModifier = 2.0f;
	}

	const float flTraceDistance = (exitTrace.end - data.enterTrace.end).length_sqr();

	// penetration modifier
	const float flModifier = (flPenetrationModifier > 0.0f ? 1.0f / flPenetrationModifier : 0.0f);

	// this calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
	float flLostDamage = (data.flCurrentDamage * flDamageLostModifier + (pWeaponData->flPenetration > 0.0f ? 3.75f / pWeaponData->flPenetration : 0.0f) * (flModifier * 3.0f)) + ((flModifier * flTraceDistance) / 24.0f);

	// did we loose too much damage?
	if (flLostDamage > data.flCurrentDamage) {
		return false;
	}

	// we can't use any of the damage that we've lost
	if (flLostDamage > 0.0f)
		data.flCurrentDamage -= flLostDamage;

	// do we still have enough damage to deal?
	if (data.flCurrentDamage < 1.0f)
		return false;

	data.vecPosition = exitTrace.end;
	--data.iPenetrateCount;
	return true;
}