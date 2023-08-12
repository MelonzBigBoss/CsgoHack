#include "visuals.hpp"
#define TICKS_TO_TIME(t) (interfaces::globals->interval_per_tick * (t))

void visuals::do_oschams() {}

void visuals::custommodels() {
	const int playermodel_index = interfaces::model_info->get_model_index(path);
	if (playermodel_index != -1) {
		csgo::local_player->set_model_index(playermodel_index);
	}
	else {
		precache_model(path);
	}

	if (interfaces::model_info->get_model_index(vpath) != -1) {
		strcpy_s(csgo::local_player->arm_model(), 256, vpath);
	}
	else {
		precache_model(vpath);
	}

	//int worldweaponmodel_index = interfaces::model_info->get_model_index(wwpath);
	//int wworldweaponmodel_index = interfaces::model_info->get_model_index(wwwpath);
	//if (worldweaponmodel_index != -1) {
	//	for (int i = 1; i < interfaces::entity_list->get_highest_index(); i++) {
	//		entity_t* e = (entity_t*)interfaces::entity_list->get_client_entity(i);
	//		if (!e)
	//			continue;

	//		model_t* model = e->model();

	//		if (model && strstr(model->name, "snip_awp_dropped")) {
	//			e->set_model_index(worldweaponmodel_index);
	//		} else if (model && strstr(model->name, "snip_awp")) {
	//			e->set_model_index(wworldweaponmodel_index);
	//		}
	//	}
	//}
	//else {
	//	precache_model(wwpath);
	//}

	const auto view_model_handle = csgo::local_player->view_model();
	if (!view_model_handle)
		return;
	const auto view_model = static_cast<base_view_model_t*>(interfaces::entity_list->get_client_entity_handle(view_model_handle));

	weapon_t* weapon = csgo::local_player->active_weapon();
	if (!weapon)
		return;

	if (weapon->item_definition_index() != rweapon)
		return;
	
	int weaponmodel_index = interfaces::model_info->get_model_index(wpath);
	if (weaponmodel_index != -1) {
		if ((weapon_t*)interfaces::entity_list->get_client_entity_handle(view_model->weapon()) == weapon)
			view_model->set_model_index(weaponmodel_index);
	}
	else {
		precache_model(wpath);
	}

}

void visuals::player_esp() {
	for (int i = 0; i < interfaces::globals->max_clients; ++i) {
		entity_t* entity = (entity_t*)interfaces::entity_list->get_client_entity(i);
		if (entity == nullptr || !entity->is_player())
			continue;

		player_t* player = (player_t*)entity;
		if (!player->is_alive() || player->team() == csgo::local_player->team() || player == csgo::local_player)
			continue;

		box_t box;
		if (!player->get_bounding_box(box)) {
			continue;
		}

		vec3_t origin = player->get_eye_pos();
		vec2_t screen_pos;
		if (math::world_to_screen(origin, screen_pos)) {

			player_info_t pinfo{};
			interfaces::engine->get_player_info(i, &pinfo);

			weapon_t* weap = player->active_weapon();

			if (weap == nullptr)
				continue;

			weapon_info_t* winfo = weap->get_weapon_data();

			if (winfo == nullptr)
				continue;

			const int alpha = (player->dormant() ? 50 : 255);

			render::text(box.left + box.width / 2, box.top - 10, render::fonts::watermark_font, pinfo.name, true, color::white(alpha)); // name
			std::string weapon_name = ((std::string)(winfo->szWeaponName)).substr(min(7, strlen(winfo->szWeaponName)));
			render::text(box.left + box.width / 2, box.bottom, render::fonts::watermark_font, weapon_name, true, color::white(alpha)); // weapon
			render::draw_filled_rect(box.left, box.top, 4, box.height, color::black(alpha)); // health bar background
			float health = min((float)player->health(), 100) / 100;
			render::draw_filled_rect(box.left + 1, box.top + 1 + box.height * (1 - health), 2, (box.height - 2) * (health), color(255, 255, 255, alpha)); //health bar foreground
			if (player->health() != 100) {
				render::text(box.left + 1, box.top + box.height * (1 - health) - 5, render::fonts::small_font, std::to_string(player->health()), true, color::white(alpha));
			}
			if (player->has_helmet()) {
				render::text(box.right, box.top, render::fonts::watermark_font, "H", false, color::white(alpha)); // helmet flag
			}
			else if (player->armor() > 0) {
				render::text(box.right, box.top, render::fonts::watermark_font, "K", false, color::white(alpha)); // kevlar flag
			}
		}
	}
}

void visuals::world_esp() {

	for (int i = 1; i <= interfaces::entity_list->get_highest_index(); i++) {


		auto e = (entity_t*)interfaces::entity_list->get_client_entity(i);
		if (!e)
			continue;
		grenadewarning::get().do_warning((weapon_t*)e);

		c_client_class* cclass = e->client_class();

		if (!cclass)
			continue;
		const float server_time = TICKS_TO_TIME(csgo::local_player->get_tick_base());
		switch (cclass->class_id) {
			case csmokegrenadeprojectile: {
				smoke_grenade_t* smoke = (smoke_grenade_t*)e;
				const float time = (TICKS_TO_TIME(smoke->effect_tick_begin()) + smoke->max_time()) - server_time;
				vec2_t screen;
				if (math::world_to_screen(e->origin(), screen)) {
					render::text(screen.x, screen.y, render::fonts::watermark_font, "smoke", true, color(255, 255, 255, 255));
					if (time >= 1)
						render::text(screen.x, screen.y + 10, render::fonts::watermark_font, std::to_string((int)time), true, color(255, 255, 255, 255));
				}
				break;
			}
			case cmolotovprojectile: {
				vec2_t screen;
				if (math::world_to_screen(e->origin(), screen)) {
					render::text(screen.x, screen.y, render::fonts::watermark_font, "molly", true, color(255, 255, 255, 255));
				}
				break;
			}
			case cinferno: {
				inferno_t* inferno = (inferno_t*)e;
				const float time = (TICKS_TO_TIME(inferno->effect_tick_begin()) + inferno->max_time()) - server_time;
				vec2_t screen;
				if (math::world_to_screen(e->origin(), screen)) {
					render::text(screen.x, screen.y, render::fonts::watermark_font, "fire", true, color(255, 255, 255, 255));
					if (time > 0)
						render::text(screen.x, screen.y + 10, render::fonts::watermark_font, std::to_string((int)time + 1), true, color(255, 255, 255, 255));
				}
				break;
			}
			case cplantedc4: {
				plantedc4_t* c4 = (plantedc4_t*)e;

				if (c4->defused())
					break;

				float timer = 0.0f;
				const float cur_time = c4->timer(server_time);
				timer = cur_time;
				const player_t* defuser = (player_t*)interfaces::entity_list->get_client_entity_handle(c4->defuser());
				color col = color::white(255);
				if (defuser) {
					const float d_cur_time = c4->defuse_timer(server_time);
					timer = d_cur_time;
					col = d_cur_time <= cur_time ? color::green(255) : color::red(255);
				}
				vec2_t screen;
				if (math::world_to_screen(c4->origin(), screen)) {
					render::text(screen.x, screen.y, render::fonts::watermark_font, std::to_string((int)timer), true, col);
				}
				break;
			}
			case cbasecsgrenadeprojectile: {
				vec2_t screen;
				if (math::world_to_screen(e->origin(), screen)) {
					render::text(screen.x, screen.y, render::fonts::watermark_font, "frag", true, color(255, 255, 255, 255));
				}
				break;
			}
			case chostage: {
				vec2_t screen;
				if (math::world_to_screen(e->origin(), screen)) {
					render::text(screen.x, screen.y, render::fonts::watermark_font, "hostage", true, color(255, 255, 255, 255));
				}
			}
			default: {
				//vec2_t screen;
				//if (math::world_to_screen(e->origin(), screen)) {
				//	render::text(screen.x, screen.y, render::fonts::watermark_font, cclass->network_name, true, color(255, 255, 255, 255));
				//}
				break;
			}
		}
	}
}

void visuals::knifechanger() {
	if (csgo::local_player->active_weapon()) {
		weapon_t* weapon = csgo::local_player->active_weapon();
		weapon_info_t* weapon_info = weapon->get_weapon_data();

		//if (weapon->client_class()->class_id == 105) {
		if (weapon_info->bIsMeleeWeapon) {
			int index = interfaces::model_info->get_model_index("models/weapons/v_knife_m9_bay.mdl");

			base_view_model_t* viewmodel = (base_view_model_t*)interfaces::entity_list->get_client_entity_handle(csgo::local_player->view_model());
			weapon_t* view_model_weapon = (weapon_t*)interfaces::entity_list->get_client_entity_handle(viewmodel->weapon());

			weapon_info_t* viewmodelweaponinfo = view_model_weapon->get_weapon_data();

			weapon_t* worldmodel = (weapon_t*)interfaces::entity_list->get_client_entity_handle(view_model_weapon->weapon_world_model());
			viewmodel->set_model_index(index);
			worldmodel->set_model_index(index + 1);
			weapon->item_id_high() = -1;
			weapon->entity_quality() = 3;
			weapon->item_definition_index() = WEAPON_KNIFE_M9_BAYONET;
			weapon->set_model_index(index);
		}
	}
}