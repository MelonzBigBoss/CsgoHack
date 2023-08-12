
#include "grenade_warning.hpp"

static const char* index_to_grenade_name_icon(int index)
{
	switch (index)
	{
	case WEAPON_SMOKEGRENADE: return "k"; break;
	case WEAPON_HEGRENADE: return "j"; break;
	case WEAPON_MOLOTOV: return "l"; break;
	case WEAPON_INCGRENADE: return "n"; break;
	}
}

void draw_arc(int x, int y, int radius, int start_angle, int percent, int thickness, color color) {
	auto precision = (2 * M_PI) / 30;
	auto step = M_PI / 180;
	auto inner = radius - thickness;
	auto end_angle = (start_angle + percent) * step;
	auto start_angles = (start_angle * M_PI) / 180;

	for (; radius > inner; --radius) {
		for (auto angle = start_angles; angle < end_angle; angle += precision) {
			auto cx = std::round(x + radius * std::cos(angle));
			auto cy = std::round(y + radius * std::sin(angle));

			auto cx2 = std::round(x + radius * std::cos(angle + precision));
			auto cy2 = std::round(y + radius * std::sin(angle + precision));

			render::draw_line(cx, cy, cx2, cy2, color);
			//render::get().line(cx, cy, cx2, cy2, color);
		}
	}
}

bool grenadewarning::data_t::draw() const
{

	if (m_path.size() <= 1 || interfaces::globals->cur_time >= m_expire_time)
		return false;
	
	vec3_t vLocalOrigin = csgo::local_player->origin();

	float distance = vLocalOrigin.distance_to(m_origin) / 12;

	///if (distance > 200.f)
	//	return false;

	static auto size = vec2_t(35.0f, 5.0f);

	int alpha_damage = 0;

	if (m_index == WEAPON_HEGRENADE && distance <= 20)
		alpha_damage = 255 - 255 * (distance / 20);
	else if ((m_index == WEAPON_MOLOTOV || m_index == WEAPON_INCGRENADE) && distance <= 15)
		alpha_damage = 255 - 255 * (distance / 15);

	float percent = ((m_expire_time - interfaces::globals->cur_time) / TICKS_TO_TIME(m_tick));
	auto prev_screen = vec2_t(0,0);
	auto prev_on_screen = math::world_to_screen(std::get< vec3_t >(m_path.front()), prev_screen);

	vec2_t screenPos;
	vec3_t vExplodeOrigin = std::get< vec3_t >(m_path.back());

	if (!csgo::local_player->is_alive())
		vLocalOrigin = interfaces::input->vecCameraOffset;

	for (auto i = 1; i < m_path.size(); ++i)
	{
		auto cur_screen = vec2_t(0,0), last_cur_screen = vec2_t(0,0);
		const auto cur_on_screen = math::world_to_screen(std::get< vec3_t >(m_path.at(i)), cur_screen);

		if (prev_on_screen && cur_on_screen)
		{
			render::draw_line(prev_screen.x, prev_screen.y, cur_screen.x, cur_screen.y, color::white(255));
		}

		prev_screen = cur_screen;
		prev_on_screen = cur_on_screen;
	}
	static auto fontsize = render::get_text_size(render::fonts::weapon_font, index_to_grenade_name_icon(m_index));
	//render::get().circle_filled(prev_screen.x, prev_screen.y - size.y * 0.5f, 60, 20, g_cfg.esp.grenade_proximity_warning_inner_color);
	//render::get().circle_filled(prev_screen.x, prev_screen.y - size.y * 0.5f, 60, 20, Color(g_cfg.esp.grenade_proximity_warning_inner_danger_color.r(), g_cfg.esp.grenade_proximity_warning_inner_danger_color.g(), g_cfg.esp.grenade_proximity_warning_inner_danger_color.b(), alpha_damage));
	draw_arc(prev_screen.x, prev_screen.y - size.y * 0.5f, 14, 0, 360 * percent, 1, color::white());
	render::text(prev_screen.x, prev_screen.y - fontsize.y / 2, render::fonts::weapon_font, index_to_grenade_name_icon(m_index), true, color::white());

	// off screen proximity 
	/*
	int x, y;
	interfaces::engine->get_screen_size(x, y);
	const int screen_center_x = x / 2, screen_center_y = y / 2;

	vec3_t screen_point, viewangles;
	interfaces::debug_overlay->screen_position(vExplodeOrigin, screen_point);
	if (screen_point.x < 0 || screen_point.y < 0 || screen_point.x > x || screen_point.y > y)
	{
		interfaces::engine->get_view_angles(viewangles);
		float desired_offset = viewangles.y - math::calculate_angle(csgo::local_player->get_eye_pos(), vExplodeOrigin).y - 90;

		const auto angle_yaw_rad1 = DEG2RAD(desired_offset);

		const auto new_point_x =
			screen_center_x + ((((x - (59 * 3)) * .5f) * (59 / 100.0f)) * cosf(angle_yaw_rad1)) + (int)(6.0f * (((float)59 - 4.f) / 16.0f));

		const auto new_point_y =
			screen_center_y + ((((y - (59 * 3)) * .5f) * (59 / 100.0f)) * sinf(angle_yaw_rad1));

		//render::get().circle_filled(new_point_x, new_point_y - size.y * 0.5f, 60, 20, g_cfg.esp.grenade_proximity_warning_inner_color);
		//render::get().circle_filled(new_point_x, new_point_y - size.y * 0.5f, 60, 20, Color(g_cfg.esp.grenade_proximity_warning_inner_danger_color.r(), g_cfg.esp.grenade_proximity_warning_inner_danger_color.g(), g_cfg.esp.grenade_proximity_warning_inner_danger_color.b(), alpha_damage));
		draw_arc(new_point_x, new_point_y - size.y * 0.5f, 20, 0, 360 * percent, 2, color::white());
		render::text(new_point_x, new_point_y - size.y * 0.5f, render::fonts::weapon_font, index_to_grenade_name_icon(m_index), true, color::white());
	}
	*/
	return true;
}

void grenadewarning::do_warning(weapon_t* entity)
{
	auto& predicted_nades = this->get_list();
	
	static auto last_server_tick = interfaces::clientstate->server_tick;

	if (last_server_tick != interfaces::clientstate->server_tick) {
		if (!predicted_nades.empty())
			predicted_nades.clear();

		last_server_tick = interfaces::clientstate->server_tick;
	}

	if (entity->dormant())
		return;

	const auto client_class = entity->client_class();
	if (!client_class
		|| client_class->class_id != cmolotovprojectile && client_class->class_id != cbasecsgrenadeprojectile && client_class->class_id != csmokegrenadeprojectile)
		return;

	if (client_class->class_id == cbasecsgrenadeprojectile) {
		const auto model = entity->model();
		if (!model)
			return;
		
		const auto studio_model = interfaces::model_info->get_studio_model(model);
		if (!studio_model
			|| std::string_view(studio_model->name_char_array).find("fraggrenade") == std::string::npos)
			return;
	}

	const auto handle = entity->GetRefEHandle().ToLong();

	player_t* thrower = (player_t*)interfaces::entity_list->get_client_entity_handle(entity->thrower());

	if (entity->explodeeffecttickbegin() )
	{
		predicted_nades.erase(handle);
		return;
	} 
	//const float spawn_time = *reinterpret_cast<float*>(uintptr_t(entity) + 0xA370);
	if (predicted_nades.find(handle) == predicted_nades.end()) {
		predicted_nades.emplace(
			std::piecewise_construct,
			std::forward_as_tuple(handle),
			std::forward_as_tuple(
				thrower,
				client_class->class_id == cmolotovprojectile ? WEAPON_MOLOTOV : client_class->class_id == cbasecsgrenadeprojectile ? WEAPON_HEGRENADE : WEAPON_SMOKEGRENADE,
				entity->origin(), entity->grenade_velocity(),
				entity->grenade_spawntime(), TIME_TO_TICKS(reinterpret_cast<player_t*>(entity)->simulation_time() - entity->grenade_spawntime()) //entity->grenade_spawntime()
			)
		);
	}

	if (predicted_nades.at(handle).draw())
		return;

	predicted_nades.erase(handle);
}
