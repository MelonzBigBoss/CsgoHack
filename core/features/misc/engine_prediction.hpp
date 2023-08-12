#pragma once
#include "../../../dependencies/utilities/csgo.hpp"

namespace prediction {

	struct netvars_data
	{
		uintptr_t m_hGroundEntity;

		float m_flPostponeFireReadyTime = 0.f;
		float m_flRecoilIndex = 0.f;
		float m_flAccuracyPenalty = 0.f;
		float m_flDuckAmount = 0.f;
		float m_flDuckSpeed = 0.f;
		float m_flFallVelocity = 0.f;

		int m_nRenderMode = 0;
		int m_nTickbase = 0;
		int m_fFlags = 0;

		vec3_t m_vecOrigin = vec3_t();
		vec3_t m_vecVelocity = vec3_t();
		vec3_t m_vecBaseVelocity = vec3_t();
		vec3_t m_vecViewOffset = vec3_t();
		vec3_t m_vecAimPunchAngleVel = vec3_t();

		vec3_t m_angAimPunchAngle = vec3_t();
		vec3_t m_angViewPunchAngle = vec3_t();
	};

	void start(c_usercmd* cmd);
	void end();
	void store_viewmodel_data();
	void adjust_viewmodel_data();
	void store_netvar_data(int command_number);
	void restore_netvar_data(int command_number);

	inline player_move_data data;
	inline float old_cur_time;
	inline float old_frame_time;
	inline int* prediction_random_seed;

	inline int animation_parity = 0.0f;
	inline int sequence = 0.0f;
	inline float cycle = 0;
	inline float animtime = 0;

	inline netvars_data compress_data[150];
};
