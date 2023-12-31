#pragma once
#include "../math/vector3d.hpp"

using MDLHandle_t = std::uint16_t;

struct model_t {
	void	*handle;
	char	name[260];
	int	load_flags;
	int	server_count;
	int	type;
	int	flags;
	vec3_t	vec_mins;
	vec3_t	vec_maxs;
	float	radius;
	void* key_values;
	union
	{
		void* brush;
		MDLHandle_t studio;
		void* sprite;
	};
};

struct model_render_info_t {
	vec3_t		origin;
	vec3_t		angles;
	char		u0[0x4];
	i_client_renderable* renderable;
	const model_t	*model;
	const matrix_t	*model_to_world;
	const matrix_t	*lighting_offset;
	const vec3_t	*lighting_origin;
	int		flags;
	int		entity_index;
	int		skin;
	int		body;
	int		hitboxset;
	unsigned short	instance;

	model_render_info_t() {
		model_to_world = nullptr;
		lighting_offset = nullptr;
		lighting_origin = nullptr;
	}
};
