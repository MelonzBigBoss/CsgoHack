#pragma once
#include "../../dependencies/utilities/csgo.hpp"
#include "../menu/variables.hpp"
#include "../features/features.hpp"
#include "../../source-sdk/math/utl_vector.hpp"
namespace ragebot {
	
	struct record {
		player_t* player;
		int tickcount;
		float simtime;
		std::vector<vec3_t> points;
		vec3_t absOrigin;
		vec3_t vecOrigin;
		vec3_t absAngles;
		vec3_t vecMins;
		vec3_t vecMaxs;
		std::array<matrix_t, 128> matrixdata;
		draw_model_state_t state;
		model_render_info_t info;
		matrix_t model_to_world;
		bool dontdoos;
	};
	struct aimbotpoint {
		record rec;
		vec3_t point;
		int hitbox;
	};
	struct oscham_t {
		std::array<matrix_t, 128> matrixdata;
		draw_model_state_t state;
		model_render_info_t info;
		matrix_t model_to_world;
		float cur_time;
	};

	inline std::vector<ragebot::oscham_t> oschams;

	inline std::deque<ragebot::record> records[32];

	static std::deque<aimbotpoint> points;
	
	constexpr int hitboxes[3] = { hitbox_stomach,hitbox_pelvis,hitbox_head };

	record lastvalidrecord(std::deque<record> recorddeque);
	void aimbot(c_usercmd* cmd);
}