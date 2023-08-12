#pragma once

#include "../../dependencies/utilities/csgo.hpp"
#include "../menu/variables.hpp"
#include "../features/features.hpp"

#define TICKS_TO_TIME(t) (interfaces::globals->interval_per_tick * (t))
#define TIME_TO_TICKS(t) ((int)(0.5f + (float)(t) /interfaces::globals->interval_per_tick))

enum ACT
{
	ACT_NONE,
	ACT_THROW,
	ACT_LOB,
	ACT_DROP,
};

class GrenadePrediction : public singleton<GrenadePrediction>
{
private:
	std::vector<vec3_t> path;

	struct Data
	{
		int type = 0;
		int act = 0;
		float throw_strength = 0;
	} data;

	void Setup(vec3_t& vecSrc, vec3_t& vecThrow, const vec3_t& viewangles);
	void Simulate(view_setup_t* setup);

	int Step(vec3_t& vecSrc, vec3_t& vecThrow, int tick, float interval);
	bool CheckDetonate(vec3_t& vecThrow, trace_t& tr, int tick, float interval);

	void TraceHull(vec3_t& src, vec3_t& end, trace_t& tr);
	void AddGravityMove(vec3_t& move, vec3_t& vel, float frametime, bool onground);
	void PushEntity(vec3_t& src, const vec3_t& move, trace_t& tr);
	void ResolveFlyCollisionCustom(trace_t& tr, vec3_t& vecVelocity, const vec3_t& move, float interval);
	int PhysicsClipVelocity(const vec3_t& in, const vec3_t& normal, vec3_t& out, float overbounce);
public:
	void Tick(int buttons);
	void View(view_setup_t* setup, weapon_t* weapon);
	void Paint();
};