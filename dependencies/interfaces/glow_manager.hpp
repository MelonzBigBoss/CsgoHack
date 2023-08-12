#pragma once
#include "../../source-sdk/math/vector3d.hpp"

class entity_t;

class glow_object_definition_t {
public:
	void set(float r, float g, float b, float a) {
		color = vec3_t(r, g, b);
		alpha = a;
		render_when_occluded = true;
		render_when_unoccluded = false;
		bloom_amount = 1.0f;
	}
	bool unused() {
		return next_free_slot != -2;
	}

	int next_free_slot;
	
	entity_t* entity;
	vec3_t color;
	float alpha;
	char unknown0[4];
	float flUnk;
	float bloom_amount;
	float local_player_is_zero_pointer_3;
	bool render_when_occluded;
	bool render_when_unoccluded;
	bool full_bloom_render;
	char unknown2[1];
	int full_bloom_stencil_test_value;
	int style;
	int splitscreenslot;


};

class glow_manager_t {
public:
	utl_vector<glow_object_definition_t> vecGlowObjectDefinitions;
	int nFirstFreeSlot;
};