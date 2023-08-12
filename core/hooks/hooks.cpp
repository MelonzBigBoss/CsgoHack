#include "../../dependencies/utilities/csgo.hpp"
#include "../features/features.hpp"
#include "../features/misc/engine_prediction.hpp"
#include "../menu/menu.hpp"
#include "../features/ragebot.hpp"
#include "../features/grenade_prediction.hpp"
#include "../features/grenade_warning.hpp"
#include "../features/antiaim.hpp"
#include "../features/visuals.hpp"

#define TIME_TO_TICKS(t) ((int)(0.5f + (float)(t) / interfaces::globals->interval_per_tick))

hooks::create_move::fn create_move_original = nullptr;
hooks::render_smoke_overlay::fn render_smoke_overlay_original = nullptr;
hooks::paint_traverse::fn paint_traverse_original = nullptr;
hooks::frame_stage_notify::fn frame_stage_notify_original = nullptr;
hooks::override_view::fn override_view_original = nullptr;
hooks::draw_model_execute::fn draw_model_execute_original = nullptr;
hooks::create_move_proxy::fn create_move_proxy_original = nullptr;
hooks::should_skip_animation_frame::fn should_skip_animation_frame_original = nullptr;
hooks::do_procedural_foot_plant::fn do_procedural_foot_plant_original = nullptr;
hooks::check_for_sequence_change::fn check_for_sequence_change_original = nullptr;
hooks::standard_blending_rules::fn standard_blending_rules_original = nullptr;
hooks::calculate_view::fn calculate_view_original = nullptr;
hooks::modify_eye_position::fn modify_eye_position_original = nullptr;
hooks::update_animation_state::fn update_animation_state_original = nullptr;
hooks::update_client_side_animation::fn update_client_side_animation_original = nullptr;
hooks::build_transformations::fn build_transformations_original = nullptr;
hooks::is_hltv::fn is_hltv_original = nullptr;
hooks::calc_view_bob::fn calc_view_bob_original = nullptr;
hooks::is_paused::fn is_paused_original = nullptr;
hooks::should_draw_view_model::fn should_draw_view_model_original = nullptr;
hooks::cl_move::fn cl_move_original = nullptr;
hooks::run_command::fn run_command_original = nullptr;
hooks::physics_simulate::fn physics_simulate_original = nullptr;
hooks::list_leaves_in_box::fn list_leaves_in_box_original = nullptr;
hooks::do_post_screen_effects::fn do_post_screen_effects_original = nullptr;
hooks::packet_end::fn packet_end_original = nullptr;
hooks::packet_start::fn packet_start_original = nullptr;
hooks::scene_end::fn scene_end_original = nullptr;
hooks::get_exposure_range::fn get_exposure_range_original = nullptr;
hooks::alloc_key_values_memory::fn alloc_key_values_memory_original = nullptr;
hooks::find_mdl::fn find_mdl_original = nullptr;
hooks::sendnetmsg::fn sendnetmsg_original = nullptr;

i_material* flat;
i_material* glow;
i_material* whiteglow;
i_material* wire;
i_material* metal;
bool m_should_animate = true;
bool thirdperson = false;

vec3_t rotation;
animation_layer m_layers[13];
float m_poses[24];

float m_flVelocityModifier = 1.0f;

int charge = 0;
constexpr int maxtickshift = 16;
bool shifting = false;
bool shouldshift = false;
int lastshift = 0;
int shiftcmd = 0;

std::array<matrix_t, 128> desyncmatrix;

class EventListener : public i_game_event_listener2
{
public:
	inline void setup() {

		interfaces::event_manager->add_listener(this, "bullet_impact", false);

		if (!interfaces::event_manager->find_listener(this, "bullet_impact"))
			throw std::runtime_error("failed add listener");

		console::log("[setup] events registered!\n");
	}
	inline void destroy() {
		interfaces::event_manager->remove_listener(this);
	}
	virtual void fire_game_event(i_game_event* event) override
	{
		
		console::log(event->get_name());
		console::log("\n");

		if (strcmp(event->get_name() , "bullet_impact") == 0)
		{
			player_t* player = static_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_player_for_user_id(event->get_int("userid"))));
			if (csgo::local_player == nullptr || player != csgo::local_player)
				return;

			beam_info_t beamInfo;
			beamInfo.nType = TE_BEAMPOINTS;
			beamInfo.pszModelName = "sprites/white.vmt";
			beamInfo.flHaloScale = -1;
			beamInfo.flHaloScale = 0.0f;
			beamInfo.vecStart = player->get_eye_pos() - vec3_t(0,0,0.1f);
			beamInfo.vecEnd = vec3_t(event->get_float("x"), event->get_float("y"), event->get_float("z") - 0.1f);
			beamInfo.flLife = 10.f;

			beamInfo.flFadeLength = 0.f;
			beamInfo.flAmplitude = 0.f;
			beamInfo.nSegments = 2;
			beamInfo.bRenderable = true;
			beamInfo.flBrightness = 255.f;
			beamInfo.flSpeed = 0.5f;
			beamInfo.iStartFrame = 0;
			beamInfo.flFrameRate = 0.f;
			beamInfo.flWidth = 0.5f;
			beamInfo.flEndWidth = 0.5f;

			beamInfo.flRed = 85.0f;
			beamInfo.flGreen = 54.0f;
			beamInfo.flBlue = 224.0f;

			beamInfo.nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

			Beam_t* myBeam = interfaces::view_render_beam->CreateBeamPoints(beamInfo);

			if (myBeam)
				interfaces::view_render_beam->DrawBeam(myBeam); 


		}
	}

	virtual int get_event_debug_id() override
	{
		return 42;
	}
};

	EventListener eventlist = EventListener();



bool hooks::initialize() {

#if 0 // #ifdeg _DEBUG

	Address debugbreak = utilities::pattern_scan("client.dll", "CC F3 0F 10 4D ? 0F 57 C0");

	DWORD old;


	VirtualProtect(debugbreak, 1, PAGE_EXECUTE_READWRITE, &old);

	debugbreak.set< uint8_t >(0x90);

	VirtualProtect(debugbreak, 1, old, &old);

	console::log("[setup] bopfix initialized!\n");

#endif

	const auto create_move_target = reinterpret_cast<void*>(get_virtual(interfaces::clientmode, hooks::create_move::num));
	const auto create_move_proxy_target = reinterpret_cast<void*>(get_virtual(interfaces::client, hooks::create_move_proxy::num));
	const auto paint_traverse_target = reinterpret_cast<void*>(get_virtual(interfaces::panel, hooks::paint_traverse::num));
	const auto frame_stage_notify_target = reinterpret_cast<void*>(get_virtual(interfaces::client, frame_stage_notify::num));
	const auto override_view_target = reinterpret_cast<void*>(get_virtual(interfaces::clientmode, override_view::num));
	const auto draw_model_execute_target = reinterpret_cast<void*>(get_virtual(interfaces::model_render, draw_model_execute::num));
	const auto should_skip_animation_frame_target = reinterpret_cast<void*>(utilities::pattern_scan("client.dll", hooks::should_skip_animation_frame::sig)); 
	const auto do_procedural_foot_plant_target = reinterpret_cast<void*>(utilities::pattern_scan("client.dll", hooks::do_procedural_foot_plant::sig));
	const auto check_for_sequence_change_target = reinterpret_cast<void*>(utilities::pattern_scan("client.dll", hooks::check_for_sequence_change::sig));
	const auto standard_blending_rules_target = reinterpret_cast<void*>(utilities::pattern_scan("client.dll", hooks::standard_blending_rules::sig));
	const auto calculate_view_target = reinterpret_cast<void*>(utilities::pattern_scan("client.dll", hooks::calculate_view::sig));
	const auto modify_eye_position_target = reinterpret_cast<void*>(utilities::pattern_scan("client.dll", hooks::modify_eye_position::sig));
	const auto update_animation_state_target = reinterpret_cast<void*>(utilities::pattern_scan("client.dll", hooks::update_animation_state::sig));
	const auto update_client_side_animation_target = reinterpret_cast<void*>(utilities::pattern_scan("client.dll", hooks::update_client_side_animation::sig));
	const auto build_transformations_target = reinterpret_cast<void*>(utilities::pattern_scan("client.dll", hooks::build_transformations::sig));
	const auto render_smoke_overlay_target = reinterpret_cast<void*>(get_virtual(interfaces::view_render, hooks::render_smoke_overlay::num));
	const auto is_hltv_target = reinterpret_cast<void*>(get_virtual(interfaces::engine, hooks::is_hltv::num));
	const auto calc_view_bob_target = reinterpret_cast<void*>(utilities::pattern_scan("client.dll", hooks::calc_view_bob::sig));
	const auto is_paused_target = reinterpret_cast<void*>(get_virtual(interfaces::engine, hooks::is_paused::num));
	const auto should_draw_view_model_target = reinterpret_cast<void*>(get_virtual(interfaces::clientmode, hooks::should_draw_view_model::num));
	const auto cl_move_target = reinterpret_cast<void*>(utilities::pattern_scan("engine.dll", hooks::cl_move::sig));
	const auto run_command_target = reinterpret_cast<void*>(get_virtual(interfaces::prediction, hooks::run_command::num));
	const auto physics_simulate_target = reinterpret_cast<void*>(utilities::pattern_scan("client.dll", hooks::physics_simulate::sig));
	const auto list_leaves_in_box_target = reinterpret_cast<void*>(get_virtual(interfaces::engine->get_bsp_tree_query(), hooks::list_leaves_in_box::num));
	const auto do_post_screen_effects_target = reinterpret_cast<void*>(get_virtual(interfaces::clientmode, hooks::do_post_screen_effects::num));
	const auto packet_end_target = reinterpret_cast<void*>(utilities::pattern_scan("engine.dll", hooks::packet_end::sig));
	//const auto packet_start_target = reinterpret_cast<void*>(get_virtual(interfaces::clientstate, hooks::packet_start::num));
	const auto scene_end_target = reinterpret_cast<void*>(get_virtual(interfaces::render_view, hooks::scene_end::num));
	const auto get_exposure_range_target = reinterpret_cast<void*>(utilities::pattern_scan("client.dll", hooks::get_exposure_range::sig));
	const auto alloc_key_values_memory_target = reinterpret_cast<void*>(get_virtual(interfaces::key_value_system, hooks::alloc_key_values_memory::num));
	const auto find_mdl_target = reinterpret_cast<void*>(get_virtual(interfaces::mdl_cache, hooks::find_mdl::num));

	if (MH_Initialize() != MH_OK)
		throw std::runtime_error("failed to initialize MH_Initialize.");

	if (MH_CreateHook(create_move_target, &create_move::hook, reinterpret_cast<void**>(&create_move_original)) != MH_OK)
		throw std::runtime_error("failed to initialize create_move. (outdated index?)");

	if (MH_CreateHook(create_move_proxy_target, &create_move_proxy::hook, reinterpret_cast<void**>(&create_move_proxy_original)) != MH_OK)
		throw std::runtime_error("failed to initialize create_move_proxy. (outdated index?)");

	if (MH_CreateHook(paint_traverse_target, &paint_traverse::hook, reinterpret_cast<void**>(&paint_traverse_original)) != MH_OK)
		throw std::runtime_error("failed to initialize paint_traverse. (outdated index?)");

	if (MH_CreateHook(frame_stage_notify_target, &frame_stage_notify::hook, reinterpret_cast<void**>(&frame_stage_notify_original)) != MH_OK)
		throw std::runtime_error("failed to initialize frame_stage_notify. (outdated index?)");

	if (MH_CreateHook(override_view_target, &override_view::hook, reinterpret_cast<void**>(&override_view_original)) != MH_OK)
		throw std::runtime_error("failed to initialize override_view. (outdated index?)");

	if (MH_CreateHook(draw_model_execute_target, &draw_model_execute::hook, reinterpret_cast<void**>(&draw_model_execute_original)) != MH_OK)
		throw std::runtime_error("failed to initialize draw_model_execute. (outdated index?)");

	if (MH_CreateHook(should_skip_animation_frame_target, &should_skip_animation_frame::hook, reinterpret_cast<void**>(&should_skip_animation_frame_original)) != MH_OK)
		throw std::runtime_error("failed to initialize should_skip_animation_frame (outdated sig?)");

	if (MH_CreateHook(do_procedural_foot_plant_target, &do_procedural_foot_plant::hook, reinterpret_cast<void**>(&do_procedural_foot_plant_original)) != MH_OK)
		throw std::runtime_error("failed to initialize do_procedural_foot_plant (outdated sig?)");

	if (MH_CreateHook(check_for_sequence_change_target, &check_for_sequence_change::hook, reinterpret_cast<void**>(&check_for_sequence_change_original)) != MH_OK)
		throw std::runtime_error("failed to initialize check_for_sequence_change (outdated sig?)"); 

	if (MH_CreateHook(standard_blending_rules_target, &standard_blending_rules::hook, reinterpret_cast<void**>(&standard_blending_rules_original)) != MH_OK)
		throw std::runtime_error("failed to initialize standard_blending_rules (outdated sig?)"); 

	if (MH_CreateHook(calculate_view_target, &calculate_view::hook, reinterpret_cast<void**>(&calculate_view_original)) != MH_OK)
		throw std::runtime_error("failed to initialize calculate_view (outdated sig?)");

	if (MH_CreateHook(modify_eye_position_target, &modify_eye_position::hook, reinterpret_cast<void**>(&modify_eye_position_original)) != MH_OK)
		throw std::runtime_error("failed to initialize modify_eye_position (outdated sig?)");

	if (MH_CreateHook(update_animation_state_target, &update_animation_state::hook, reinterpret_cast<void**>(&update_animation_state_original)) != MH_OK)
		throw std::runtime_error("failed to initialize update_animation_state (outdated sig?)");
	
	if (MH_CreateHook(update_client_side_animation_target, &update_client_side_animation::hook, reinterpret_cast<void**>(&update_client_side_animation_original)) != MH_OK)
		throw std::runtime_error("failed to initialize update_client_side_animation (outdated sig?)");

	if (MH_CreateHook(build_transformations_target, &build_transformations::hook, reinterpret_cast<void**>(&build_transformations_original)) != MH_OK)
		throw std::runtime_error("failed to initialize build_transformations (outdated sig?)");

	if (MH_CreateHook(render_smoke_overlay_target, &render_smoke_overlay::hook, reinterpret_cast<void**>(&render_smoke_overlay_original)) != MH_OK)
		throw std::runtime_error("failed to initialize render_smoke_overlay (outdated sig?)");

	if (MH_CreateHook(is_hltv_target, &is_hltv::hook, reinterpret_cast<void**>(&is_hltv_original)) != MH_OK)
		throw std::runtime_error("failed to initialize is_hltv (outdated index?)");

	if (MH_CreateHook(calc_view_bob_target, &calc_view_bob::hook, reinterpret_cast<void**>(&calc_view_bob_original)) != MH_OK)
		throw std::runtime_error("failed to initialize calc_view_bob (outdated sig?)");

	if (MH_CreateHook(is_paused_target, &is_paused::hook, reinterpret_cast<void**>(&is_paused_original)) != MH_OK)
		throw std::runtime_error("failed to initialize is_paused (outdated index?)");

	if (MH_CreateHook(should_draw_view_model_target, &should_draw_view_model::hook, reinterpret_cast<void**>(&should_draw_view_model_original)) != MH_OK)
		throw std::runtime_error("failed to initialize should_draw_view_model (outdated index?)");

	if (MH_CreateHook(cl_move_target, &cl_move::hook, reinterpret_cast<void**>(&cl_move_original)) != MH_OK)
		throw std::runtime_error("failed to initialize cl_move (outdated sig?)");

	if (MH_CreateHook(run_command_target, &run_command::hook, reinterpret_cast<void**>(&run_command_original)) != MH_OK)
		throw std::runtime_error("failed to initialize run_command (outdated index?)");

	if (MH_CreateHook(physics_simulate_target, &physics_simulate::hook, reinterpret_cast<void**>(&physics_simulate_original)) != MH_OK)
		throw std::runtime_error("failed to initialize physics_simulate (outdated sig?)");

	if (MH_CreateHook(list_leaves_in_box_target, &list_leaves_in_box::hook, reinterpret_cast<void**>(&list_leaves_in_box_original)) != MH_OK)
		throw std::runtime_error("failted to initialize list_leaves_in_box (outdated index?)");

	if (MH_CreateHook(do_post_screen_effects_target, &do_post_screen_effects::hook, reinterpret_cast<void**>(&do_post_screen_effects_original)) != MH_OK)
		throw std::runtime_error("failted to initialize do_post_screen_effects (outdated index?)");

	if (MH_CreateHook(packet_end_target, &packet_end::hook, reinterpret_cast<void**>(&packet_end_original)) != MH_OK)
		throw std::runtime_error("failed to initialize packet_end (outdated sig?)");

	//if (MH_CreateHook(packet_start_target, &packet_start::hook, reinterpret_cast<void**>(&packet_start_original)) != MH_OK)
	//	throw std::runtime_error("failed to initialize packet_start (outdated index?)");

	if (MH_CreateHook(scene_end_target, &scene_end::hook, reinterpret_cast<void**>(&scene_end_original)) != MH_OK)
		throw std::runtime_error("failed to initialize scene_end (outdated index?)");

	if (MH_CreateHook(get_exposure_range_target, &get_exposure_range::hook, reinterpret_cast<void**>(&get_exposure_range_original)) != MH_OK)
		throw std::runtime_error("failed to initialize get_exposure_range (outdated sig?)");

	if (MH_CreateHook(alloc_key_values_memory_target, &alloc_key_values_memory::hook, reinterpret_cast<void**>(&alloc_key_values_memory_original)) != MH_OK)
		throw std::runtime_error("failed to initialize alloc_key_values_memory (outdated index?)");

	if (MH_CreateHook(find_mdl_target, &find_mdl::hook, reinterpret_cast<void**>(&find_mdl_original)) != MH_OK)
		throw std::runtime_error("failed to initialize find_mdl (outdated index?)");

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		throw std::runtime_error("failed to enable hooks.");

	console::log("[setup] hooks initialized!\n");

	eventlist.setup();

	return true;
}

void hooks::release() {

	MH_Uninitialize();

	MH_DisableHook(MH_ALL_HOOKS);

	eventlist.destroy();
}
std::uintptr_t GetAbsoluteAddress(const std::uintptr_t uRelativeAddress)
{
	return uRelativeAddress + 0x4 + *reinterpret_cast<std::int32_t*>(uRelativeAddress);
}
void* __fastcall hooks::alloc_key_values_memory::hook(void* ecx, void* edx, int size)
{
	static const std::uintptr_t uAllocKeyValuesEngine = GetAbsoluteAddress((uintptr_t)utilities::pattern_scan("engine.dll", "E8 ? ? ? ? 83 C4 08 84 C0 75 10 FF 75 0C") + 0x1) + 0x4A;
	static const std::uintptr_t uAllocKeyValuesClient = GetAbsoluteAddress((uintptr_t)utilities::pattern_scan("client.dll", "E8 ? ? ? ? 83 C4 08 84 C0 75 10") + 0x1) + 0x3E;

	if (const std::uintptr_t uReturnAddress = reinterpret_cast<std::uintptr_t>(_ReturnAddress()); uReturnAddress == uAllocKeyValuesEngine || uReturnAddress == uAllocKeyValuesClient)
		return nullptr;
	
	return alloc_key_values_memory_original(ecx, size);
}
bool __fastcall hooks::sendnetmsg::hook(void* ecx, void* edx, i_net_message* message, bool force_reliable, bool voice) {
	if (message->GetType() == 14)
		return false;

	if (message->GetGroup() == i_net_channel_info::VOICE)
		voice = true;

	return sendnetmsg_original(ecx, message, force_reliable, voice);
}
MDLHandle_t __fastcall hooks::find_mdl::hook(void* ecx, void* edx, const char* name) {
//	console::log(name);
//	console::log("\n");
//
//	if (strstr(name, "arms"))
//		return find_mdl_original(ecx, "models/player/custom_player/maoling/re0/emilia_arms.mdl");
//
	return find_mdl_original(ecx, name);
}

void __fastcall hooks::get_exposure_range::hook(float* min, float* max) {
	*min = 1.f;
	*max = 1.f;

	get_exposure_range_original(min, max);
}
void __fastcall hooks::scene_end::hook(void* ecx, void* edx) {

	if (!interfaces::model_render || !interfaces::material_system->get_render_context() || whiteglow == nullptr)
		return scene_end_original(ecx);

	whiteglow->set_material_var_flag(material_var_additive, true);
	whiteglow->set_material_var_flag(material_var_ignorez, true);
	interfaces::model_render->override_material(whiteglow);

	auto it = ragebot::oschams.begin();
	while (it != ragebot::oschams.end()) {
		if (!it->state.model_to_world || !it->state.renderable || !it->state.studio_hdr || !it->state.studio_hw_data ||
			!it->info.renderable || !it->info.model_to_world || !it->info.model) {
			++it;
			continue;
		}
		
		it->cur_time += interfaces::globals->frame_time;
		if (it->cur_time > 5) {
			it = ragebot::oschams.erase(it);
			continue;
		}

		whiteglow->alpha_modulate(std::clamp(5 - it->cur_time, 0.f , 0.7f));
		draw_model_execute_original(interfaces::model_render, interfaces::material_system->get_render_context(), it->state, it->info, it->matrixdata.data());
		
		++it;
	}

	interfaces::model_render->override_material(nullptr);
	whiteglow->alpha_modulate(1.f);

	scene_end_original(ecx);
}
//void __fastcall hooks::packet_start::hook(void* ecx, void* edx, int incoming, int outgoing) {
//	packet_start_original(ecx, incoming, outgoing);
//}
void __fastcall hooks::packet_end::hook(void* ecx, void* edx) {
	packet_end_original(ecx);

	prediction::restore_netvar_data(interfaces::clientstate->last_command_ack);
}
int __fastcall    hooks::do_post_screen_effects::hook       (void* ecx, void* edx, void* setup) {
	if (!interfaces::engine->is_in_game() || interfaces::engine->is_taking_screenshot() || csgo::local_player == nullptr)
		return do_post_screen_effects_original(ecx, setup);

	for (int i = 0; i < interfaces::glow_manager->vecGlowObjectDefinitions.count(); i++) {
		glow_object_definition_t& hGlowObject = interfaces::glow_manager->vecGlowObjectDefinitions[i];

		// is current object not used
		if (hGlowObject.unused())
			continue;

		// get current entity from object handle
		entity_t* pEntity = hGlowObject.entity;

		if (pEntity == nullptr)
			continue;

		if (pEntity->is_player() && (pEntity->team() != csgo::local_player->team() || csgo::local_player == pEntity)) { //&& csgo::local_player != pEntity
			hGlowObject.color = vec3_t(1.f, 1.f, 1.f);
			hGlowObject.alpha = csgo::local_player != pEntity ? 0.35f : 0.3f;
			hGlowObject.bloom_amount = 1.0f;
			hGlowObject.render_when_occluded = true;
			hGlowObject.render_when_unoccluded = false;
			hGlowObject.full_bloom_render = false;
			hGlowObject.style = csgo::local_player != pEntity ? 0 : 3;
		}
	}
	return do_post_screen_effects_original(ecx, setup);
}
void __fastcall   hooks::list_leaves_in_box::hook           (void* ecx, void* edx, const vec3_t& vecmins, const vec3_t& vecmaxs, void* pulist, void* listmax) {

	static uint8_t* uInsertIntoTree = (utilities::pattern_scan("client.dll", "56 52 FF 50 18") + 0x5); // @xref: "<unknown renderable>"

	// check for esp state and call from CClientLeafSystem::InsertIntoTree
	if (reinterpret_cast<uint8_t*>(_ReturnAddress()) == uInsertIntoTree)
	{
		// get current renderable info from stack https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L1470
		if (const auto pInfo = *reinterpret_cast<renderable_info_t**>(reinterpret_cast<std::uintptr_t>(_AddressOfReturnAddress()) + 0x14); pInfo != nullptr)
		{
			if (const auto pRenderable = pInfo->pRenderable; pRenderable != nullptr)
			{
				// check if disabling occlusion for players
				if (const auto pEntity = pRenderable->GetIClientUnknown()->GetBaseEntity(); pEntity != nullptr && pEntity->is_player()) // && pEntity->is_player() <- this shit broke as fuck
				{
					// fix render order, force translucent group (https://www.unknowncheats.me/forum/2429206-post15.html)
					// AddRenderablesToRenderLists: https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L2473
					// @ida addrenderablestorenderlists: 55 8B EC 83 EC 24 53 56 8B 75 08 57 8B 46
					pInfo->uFlags &= ~256U;
					pInfo->uFlags2 |= 64U;

					// extend world space bounds to maximum https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L707
					const static vec3_t vecMapMin(-16'384.f, -16'384.f, -16'384.f);
					const static vec3_t vecMapMax( 16'384.f,  16'384.f,  16'384.f);
					return list_leaves_in_box_original(ecx, vecMapMin, vecMapMax, pulist, listmax);
				}
			}
		}
	}

	list_leaves_in_box_original(ecx, vecmins, vecmaxs, pulist, listmax);
}
void __fastcall   hooks::render_smoke_overlay::hook         (void* ecx, int edx, bool previewmodel) {
	return;
}
bool __fastcall   hooks::should_skip_animation_frame::hook  (void* this_pointer, void* edx) {
	return false;
}
void __fastcall   hooks::do_procedural_foot_plant::hook     (void* this_pointer, void* edx, void* bone_to_world, void* left_foot_chain, void* right_foot_chain, void* pos) {
	return;
}
void __fastcall   hooks::check_for_sequence_change::hook    (void* this_pointer, void* edx, void* hdr, int cur_sequence, bool force_new_sequence, bool interpolate) {
	return check_for_sequence_change_original(this_pointer, edx, hdr, cur_sequence, force_new_sequence, false);
}
void __fastcall   hooks::standard_blending_rules::hook      (void* this_pointer, void* edx, void* hdr, void* pos, void* q, float current_time, int bone_mask) {
	if (csgo::local_player == nullptr)
		return standard_blending_rules_original(this_pointer, edx, hdr, pos, q, current_time, bone_mask);

	if (!(csgo::local_player->get_effects() & 8))
		csgo::local_player->get_effects() |= 8;

	standard_blending_rules_original(this_pointer, edx, hdr, pos, q, current_time, bone_mask);

	csgo::local_player->get_effects() &= ~8;
}
void __fastcall   hooks::calculate_view::hook               (void* this_pointer, void* edx, vec3_t& eye_origin, vec3_t& eye_angles, float& z_near, float& z_far, float& fov) {
	const auto player = reinterpret_cast<player_t*>(this_pointer);

	if (player != csgo::local_player)
		return calculate_view_original(this_pointer, edx, eye_origin, eye_angles, z_near, z_far, fov);

	const auto old_use_new_animation_state = player->use_new_animstate();

	player->use_new_animstate() = false;

	calculate_view_original(this_pointer, edx, eye_origin, eye_angles, z_near, z_far, fov);	

	player->use_new_animstate() = old_use_new_animation_state;
}
void __fastcall   hooks::modify_eye_position::hook          (void* this_pointer, void* edx, vec3_t& input_eye_position) {

	const auto animation_state = reinterpret_cast<CCSGOPlayerAnimState*>(this_pointer);
	
	animation_state->m_bSmoothHeightValid = false;

	return modify_eye_position_original(this_pointer, edx, input_eye_position);

}
void __vectorcall hooks::update_animation_state::hook       (void* this_pointer, void* unknown, float z, float y, float x, void* unknown1) {
	const auto animation_state = reinterpret_cast<CCSGOPlayerAnimState*>(this_pointer);

	// allow animations to be animated in the same frame
	if (animation_state->m_nLastUpdateFrame == interfaces::globals->frame_count)
		animation_state->m_nLastUpdateFrame -= 1;
	
	const auto player = animation_state->m_player;

	if (player != csgo::local_player)
		return update_animation_state_original(this_pointer, unknown, z, y, x, unknown1);

	const auto angle = csgo::realangle;

	return update_animation_state_original(this_pointer, unknown, z, angle.y, angle.x, unknown1);

}
void __fastcall   hooks::update_client_side_animation::hook (void* this_pointer, void* edx)																											 {
	const auto player = reinterpret_cast<player_t*>(this_pointer);

	if (player != csgo::local_player)
		return update_client_side_animation_original(this_pointer, edx);

	// only update local animations each tick
	if (m_should_animate)
		update_client_side_animation_original(this_pointer, edx);

}
void __fastcall   hooks::build_transformations::hook        (void* this_pointer, void* edx, studio_hdr_t* hdr, void* pos, void* q, const void* camera_transform, int bone_mask, void* bone_computed) {
	const auto player = reinterpret_cast<player_t*>(this_pointer);

	player->is_jiggle_bones_enabled() = false;

	build_transformations_original(this_pointer, edx, hdr, pos, q, camera_transform, bone_mask, bone_computed);
}
void __fastcall	  hooks::calc_view_bob::hook                (void* ecx, void* edx, void* origin)
{
	return calc_view_bob_original(ecx, origin);
}
bool __fastcall	  hooks::is_hltv::hook						(void* this_pointer, void* edx)	{ 
	Stack stack;

	static Address SetupVelocity{ utilities::pattern_scan("client.dll", "55 8B EC 83 E4 F8 83 EC 30 56 57 8B ? ? ? ? ? 8B") };

	if (stack.ReturnAddress() == SetupVelocity)
		return true;

	static Address accumulate_layers{ utilities::pattern_scan("client.dll", "84 C0 75 0D F6 87") };

	if (stack.ReturnAddress() == accumulate_layers)
		return true;

	return is_hltv_original(this_pointer, edx);
}
bool __stdcall    hooks::is_paused::hook					() {
	static auto AdrOfIsPausedExtrapolate = reinterpret_cast<uintptr_t*>(utilities::pattern_scan("client.dll", "0F B6 0D ? ? ? ? 84 C0 0F 44"));

	if (_ReturnAddress() == (void*)AdrOfIsPausedExtrapolate)
		return true;

	return is_paused_original();
}
bool __stdcall	  hooks::should_draw_view_model::hook		() {
	return true;
}

void doubletap(c_usercmd* cmd) {
	//if (GetAsyncKeyState(VK_XBUTTON1))
	//	csgo::bsendpacket = true;
	
	//shouldshift = ( (cmd->buttons & in_attack) || !csgo::local_player->is_alive());

	if (shifting) {
		cmd->buttons &= ~in_attack;
		csgo::bsendpacket = charge == 1;
	}
	if (interfaces::globals->tick_count - lastshift < 0)
		lastshift = interfaces::globals->tick_count;

}
void mousefix(c_usercmd* cmd) {
	if (!csgo::local_player || !cmd)
		return;

	static vec3_t delta_viewangles{ };
	vec3_t delta = cmd->viewangles - delta_viewangles;
	delta.clamp();

	static convar* sensitivity = interfaces::console->get_convar("sensitivity");

	if (!sensitivity)
		return;

	if (delta.x != 0.f) {
		static convar* m_pitch = interfaces::console->get_convar("m_pitch");

		if (!m_pitch)
			return;

		int final_dy = static_cast<int>((delta.x / m_pitch->GetFloat()) / sensitivity->GetFloat());

		(final_dy > 0 ? std::clamp(final_dy, 1, 32767) : std::clamp(final_dy, -32678, -1));

		cmd->mousedy = static_cast<short>(final_dy);
	}

	if (delta.y != 0.f) {
		static convar* m_yaw = interfaces::console->get_convar("m_yaw");

		if (!m_yaw)
			return;

		int final_dx = static_cast<int>((delta.y / m_yaw->GetFloat()) / sensitivity->GetFloat());
	
		(final_dx > 0 ? std::clamp(final_dx, 1, 32767) : std::clamp(final_dx, -32678, -1));

		cmd->mousedx = static_cast<short>(final_dx);
	}

	delta_viewangles = cmd->viewangles;
}
void __cdecl hooks::cl_move::hook(float accumulatedExtraSamples, bool finalTick) {
	if (csgo::pcmd == nullptr || csgo::local_player == nullptr || interfaces::engine->is_taking_screenshot() || !interfaces::engine->is_in_game())
		return cl_move_original(accumulatedExtraSamples, finalTick);
	if (false && charge < maxtickshift && interfaces::globals->tick_count - lastshift > 32) {
		++charge;
		csgo::bsendpacket = false;
		return;
	}

	cl_move_original(accumulatedExtraSamples, finalTick);

	if (shouldshift && charge > 0)
	{
		shiftcmd = csgo::pcmd->command_number;
		shifting = true;
		for (; charge > 0; --charge)
		{
			cl_move_original(accumulatedExtraSamples, finalTick);
			console::log("%d", charge);
		}
		lastshift = interfaces::globals->tick_count;
		
		shifting = false;
	}
}

void __fastcall hooks::physics_simulate::hook(player_t* player) {
	if (!csgo::local_player || !csgo::pcmd || player != csgo::local_player)
		return physics_simulate_original(player);

	physics_simulate_original(player);

	prediction::store_viewmodel_data();
	prediction::store_netvar_data(csgo::pcmd->command_number);
}

void __fastcall hooks::run_command::hook(void* ecx, void* edx, player_t* player, c_usercmd* cmd, void* movehelper) {
	if (player == nullptr || player != csgo::local_player)
		return run_command_original(ecx, edx, player, cmd, movehelper);

	if (cmd->command_number == shiftcmd + 1) {
		int simulation_ticks = interfaces::clientstate->choked_commands + maxtickshift ; //+ 1

		float flCorrectionSeconds = 0.03f;//std::clamp(g_csgo.sv_clockcorrection_msecs->GetFloat() / 1000.0f, 0.0f, 1.0f);
		int nCorrectionTicks = TIME_TO_TICKS(flCorrectionSeconds);
		//if (GetAsyncKeyState(VK_LBUTTON) || !GetAsyncKeyState(VK_XBUTTON1)) {
		//	nCorrectionTicks += TIME_TO_TICKS(interfaces::engine->get_net_channel_info()->get_latency(0));
		//}
	//	else {
			nCorrectionTicks += TIME_TO_TICKS(interfaces::engine->get_net_channel_info()->get_latency(1));
	//	}
		int    nIdealFinalTick = interfaces::globals->tick_count + nCorrectionTicks;

		int nEstimatedFinalTick = player->get_tick_base() + simulation_ticks;

		int     too_fast_limit = nIdealFinalTick + nCorrectionTicks;
		int     too_slow_limit = nIdealFinalTick - nCorrectionTicks;
		if (nEstimatedFinalTick > too_fast_limit || nEstimatedFinalTick < too_slow_limit) {
			auto tick = player->get_tick_base();
			int nCorrectedTick = nIdealFinalTick - simulation_ticks + interfaces::globals->sim_ticks_this_frame;
			player->get_tick_base() = nCorrectedTick;
		}

		interfaces::globals->cur_time = player->get_tick_base() * interfaces::globals->interval_per_tick;
		
	}

	return run_command_original(ecx, edx, player, cmd, movehelper);
}

static void __stdcall CreateMove(int nSequenceNumber, float flInputSampleFrametime, bool bIsActive, bool& bSendPacket)
{
	create_move_proxy_original(interfaces::client, 0, nSequenceNumber, flInputSampleFrametime, bIsActive);
	c_usercmd* cmd = interfaces::input->get_user_cmd(nSequenceNumber);
	c_vusercmd* vcmd = interfaces::input->get_vuser_cmd(nSequenceNumber);
	if (!cmd || !vcmd || !bIsActive)
		return;

	csgo::pcmd = cmd;
	interfaces::console->get_convar("sv_cheats")->set_value(1);
	//interfaces::console->get_convar("sv_max_allowed_developer")->set_value(1);
	//interfaces::console->get_convar("developer")->set_value(4);
	csgo::local_player = static_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	vec3_t old_engineviewangles;
	vec3_t old_viewangles = cmd->viewangles;
	interfaces::engine->get_view_angles(old_engineviewangles);
	auto old_forwardmove = cmd->forwardmove;
	auto old_sidemove = cmd->sidemove;

	misc::movement::bunny_hop(cmd);
	//misc::movement::autostrafe(cmd);
	misc::movement::autostrafegs(cmd, old_viewangles);
	if (csgo::local_player && csgo::local_player->active_weapon())
		GrenadePrediction::get().Tick(cmd->buttons);
	mousefix(cmd);

	csgo::bsendpacket = true;
	
	if (interfaces::clientstate->delta_tick > 0) // fixes prediction with low fps
		interfaces::prediction->update(interfaces::clientstate->delta_tick, interfaces::clientstate->delta_tick > 0, interfaces::clientstate->last_command_ack, interfaces::clientstate->last_outgoing_command + interfaces::clientstate->choked_commands);
	
	prediction::start(cmd); {
		if (csgo::local_player->is_alive())
			csgo::bsendpacket = (interfaces::clientstate->choked_commands >= 14);

		antiaim::run(cmd);

		if (GetAsyncKeyState(0x58)) {
			if (interfaces::clientstate->choked_commands <= 7) {
				cmd->buttons &= ~in_duck;
			}
			else {
				cmd->buttons |= in_duck;
			}
			cmd->buttons |= in_bullrush;
		}
		if (!shifting) {
			ragebot::aimbot(cmd);
		}

		doubletap(cmd);
		
		
	} prediction::end();

	math::correct_movement(old_viewangles, cmd, cmd->forwardmove, cmd->sidemove);
	//misc::movement::FixMove(cmd, old_viewangles);

	cmd->forwardmove = std::clamp(cmd->forwardmove, -450.0f, 450.0f);
	cmd->sidemove = std::clamp(cmd->sidemove, -450.0f, 450.0f);
	cmd->upmove = std::clamp(cmd->upmove, -320.0f, 320.0f);

	cmd->viewangles.normalize();
	cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.0f, 89.0f);
	cmd->viewangles.y = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
	cmd->viewangles.z = 0.0f;

	m_should_animate = true;
 
	if (interfaces::clientstate->net_channel != nullptr) {
		if (sendnetmsg_original == nullptr) {
			const auto sendnetmsg_target = reinterpret_cast<void*>(hooks::get_virtual(interfaces::clientstate->net_channel, hooks::sendnetmsg::num));

			MH_CreateHook(sendnetmsg_target, &hooks::sendnetmsg::hook, reinterpret_cast<void**>(&sendnetmsg_original));

			if (MH_EnableHook(sendnetmsg_target) != MH_OK)
				throw std::runtime_error("failed to initialize sendnetmsg_target. (outdated index?)");
		}
	}

	int maxi = 16 - 2;
	if (interfaces::clientstate->choked_commands >= maxi) {
		bSendPacket = true;
	}
	else {
		bSendPacket = csgo::bsendpacket;
	}

	csgo::realangle = cmd->viewangles;
	vcmd->userCmd = *cmd;
	vcmd->uHashCRC = cmd->GetChecksum();
}

bool __stdcall hooks::create_move::hook(float input_sample_frametime, c_usercmd* cmd) {
	return create_move_original(input_sample_frametime, cmd);
}

__declspec(naked) void __fastcall hooks::create_move_proxy::hook([[maybe_unused]] i_base_client_dll* thisptr, [[maybe_unused]] int edx, [[maybe_unused]] int nSequenceNumber, [[maybe_unused]] float flInputSampleFrametime, [[maybe_unused]] bool bIsActive)
{
	__asm
	{
		push	ebp
		mov		ebp, esp; // store the stack
		push	ebx; // bSendPacket
		push	esp; // restore the stack
		push	dword ptr[bIsActive]; // ebp + 16
		push	dword ptr[flInputSampleFrametime]; // ebp + 12
		push	dword ptr[nSequenceNumber]; // ebp + 8
		call	CreateMove
		pop		ebx
		pop		ebp
		retn	0Ch
	}
}

bool nightmoded = false;
void __stdcall hooks::paint_traverse::hook(unsigned int panel, bool force_repaint, bool allow_force) {
	auto panel_to_draw = fnv::hash(interfaces::panel->get_panel_name(panel));

	switch (panel_to_draw) {
		case fnv::hash("MatSystemTopPanel"): {

			render::text(10, 10, render::fonts::watermark_font, "csgo-cheat", false, color::white(255));

			interfaces::console->get_convar("@panorama_disable_blur")->set_value(1);

			menu::toggle();
			menu::render();

			render::text(10, 540, render::fonts::watermark_font, "send packet", false, (csgo::bsendpacket ? color::green() : color::red()));
			if (csgo::local_player)
				render::text(10, 550, render::fonts::watermark_font, "can shoot", false, (csgo::local_player->can_shoot() ? color::green() : color::red()));

			if (csgo::local_player && csgo::local_player->active_weapon()) {
				int idx = csgo::local_player->active_weapon()->item_definition_index();

				if (idx == WEAPON_FLASHBANG || idx == WEAPON_HEGRENADE || idx == WEAPON_SMOKEGRENADE || idx == WEAPON_MOLOTOV || idx == WEAPON_DECOY || idx == WEAPON_INCGRENADE)
					GrenadePrediction::get().Paint();
			}
			if (!csgo::local_player) {
				break;
			}
			if (!interfaces::engine->is_connected() || !interfaces::engine->is_in_game()) {
				csgo::local_player = nullptr;
				ragebot::oschams.clear();
				break;
			}

			weapon_t* lpweap = csgo::local_player->active_weapon();

			if (lpweap != nullptr) {
				weapon_info_t* lpwinfo = lpweap->get_weapon_data();


				if (lpwinfo->nWeaponType == WEAPONTYPE_SNIPER_RIFLE && csgo::local_player->is_scoped()) {
					int x, y;
					interfaces::engine->get_screen_size(x, y);
					render::draw_filled_rect(x / 2, 0, 1, y, color::black(255));
					render::draw_filled_rect(0, y / 2, x, 1, color::black(255));
				}
			}

			visuals::world_esp();
			visuals::player_esp();
			
			break;
		}
		case fnv::hash("FocusOverlayPanel"): {
			//interfaces::panel->set_keyboard_input_enabled(panel, variables::menu::opened);
			interfaces::panel->set_mouse_input_enabled(panel, variables::menu::opened);

			break;
		}
		case fnv::hash("HudZoom"):
			return;
			break;
	}
	paint_traverse_original(interfaces::panel, panel, force_repaint, allow_force);
}
static constexpr std::array<const char*, 3U> arrSmokeMaterials = { "particle/vistasmokev1/vistasmokev1_smokegrenade", "particle/vistasmokev1/vistasmokev1_emods", "particle/vistasmokev1/vistasmokev1_emods_impactdust"};

static vec3_t old_aim_punch = { }, old_view_punch = { };

std::uintptr_t* FindHudElement(const char* szName)
{
	// @note: https://www.unknowncheats.me/forum/counterstrike-global-offensive/342743-finding-sigging-chud-pointer-chud-findelement.html

	static auto pHud = *reinterpret_cast<void**>(utilities::pattern_scan("client.dll", "B9 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 89") + 0x1); // @xref: "CHudWeaponSelection"

	using FindHudElementFn = std::uintptr_t* (__thiscall*)(void*, const char*);
	static auto oFindHudElement = reinterpret_cast<FindHudElementFn>(utilities::pattern_scan("client.dll", "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28")); // @xref: "[%d] Could not find Hud Element: %s\n"
	assert(oFindHudElement != nullptr);

	return oFindHudElement(pHud, szName);
}

void ForceFullUpdate()
{
	using ClearHudWeaponIconFn = int(__thiscall*)(void*, int);
	static auto oClearHudWeaponIcon = reinterpret_cast<ClearHudWeaponIconFn>(utilities::pattern_scan("client.dll", "55 8B EC 51 53 56 8B 75 08 8B D9 57 6B")); // @xref: "WeaponIcon--itemcount"
	assert(oClearHudWeaponIcon != nullptr);

	// get hud weapons
	if (const auto pHudWeapons = FindHudElement("CCSGO_HudWeaponSelection") - 0x28; pHudWeapons != nullptr)
	{
		// go through all weapons
		for (std::size_t i = 0; i < *(pHudWeapons + 0x20); i++)
			i = oClearHudWeaponIcon(pHudWeapons, i);
	}

	interfaces::clientstate->delta_tick = -1;
}

void __stdcall hooks::frame_stage_notify::hook(i_net_channel_info::EClientFrameStage stage) {
	if (interfaces::engine->is_taking_screenshot() || !csgo::local_player || !interfaces::engine->is_in_game()) {
		nightmoded = false;
		return frame_stage_notify_original(stage);
	}
	switch (stage)
	{
		case FRAME_NET_UPDATE_POSTDATAUPDATE_START: {
			
			visuals::knifechanger();

			//visuals::custommodels();

			for (int i = 0; i < min(interfaces::globals->max_clients, 32); ++i) {
				entity_t* ent = (entity_t*)interfaces::entity_list->get_client_entity(i);
				if (ent == nullptr) continue;
				player_t* player = (player_t*)ent;
				player->set_abs_origin(player->origin());
			}

			break;
		}
		case FRAME_NET_UPDATE_POSTDATAUPDATE_END: 
			for (int i = 0; i < min(interfaces::globals->max_clients, 32); ++i) {
				entity_t* ent = (entity_t*)interfaces::entity_list->get_client_entity(i);
				if (ent == nullptr) continue;
				player_t* player = (player_t*)ent;
				player->disable_interpolation();
			}
			prediction::adjust_viewmodel_data();
			break;
		
		case FRAME_NET_UPDATE_END: 
			for (int i = 0; i < min(interfaces::globals->max_clients, 32); ++i) {
				entity_t* ent = (entity_t*)interfaces::entity_list->get_client_entity(i);

				if (ent == nullptr || !ent->is_player()) {
					ragebot::records[i].clear();
					continue;
				}
				player_t* player = (player_t*)ent;

				if (!csgo::local_player->is_alive() || !player->is_alive() || player->dormant() || player->team() == csgo::local_player->team() || player == csgo::local_player || player->has_gun_game_immunity()) {
					ragebot::records[i].clear();
					continue;
				}

				if (!ragebot::records[i].empty() && player->simulation_time() - ragebot::records[i].front().simtime <= 0) continue;
				
				ragebot::record& rec = ragebot::records[i].emplace_front();
				rec.player = player;
				rec.simtime = player->simulation_time();
				rec.tickcount = interfaces::globals->tick_count;
				rec.absAngles = player->abs_angles();
				rec.vecOrigin = player->origin();
				rec.absOrigin = player->abs_origin();
				rec.vecMaxs = player->collideable()->maxs();
				rec.vecMins = player->collideable()->mins();

				rec.state.renderable = player->GetClientRenderable();
				rec.state.studio_hdr = interfaces::model_info->get_studio_model(player->model());
				rec.state.drawflags = 0;
				rec.state.model_to_world = &rec.model_to_world;
				rec.state.studio_hw_data = interfaces::mdl_cache->GetHardwareData(player->model()->studio);
				static int m_nSkin = netvar_manager::find_in_datamap(player->GetPredDescMap(), "m_nSkin");
				static int m_nBody = netvar_manager::find_in_datamap(player->GetPredDescMap(), "m_nBody");
				rec.info.renderable = player->GetClientRenderable();
				rec.info.model = player->model();
				rec.info.lighting_offset = nullptr;
				rec.info.lighting_origin = nullptr;
				rec.info.hitboxset = player->hitbox_set();
				rec.info.skin = m_nSkin;
				rec.info.body = m_nBody;
				rec.info.entity_index = i;
				rec.info.model_to_world = &rec.model_to_world;
				rec.info.instance = utilities::call_virtual<model_instance_handle_t(__thiscall*)(void*)>(player->GetClientRenderable(), 30u)(player->GetClientRenderable());

				rec.info.flags = 0x1;
				
				math::angle_matrix(player->abs_angles(), rec.model_to_world, player->origin());

				player->setup_bones(rec.matrixdata.data(), 128, BONE_USED_BY_ANYTHING, 0);

				if (!player->GetClientRenderable())
					rec.dontdoos = true;

				if (ragebot::records[i].size() > 64)
					ragebot::records[i].pop_back();
			}
			break;
		
		case FRAME_RENDER_START: {
			//visuals::custommodels();
			if (!nightmoded) {
				interfaces::console->get_convar("sv_skyname")->set_value("sky_csgo_night02");
				interfaces::console->get_convar("r_drawspecificstaticprop")->set_value(0);
				interfaces::console->get_convar("mat_force_tonemap_scale")->set_value(0.3f);
				interfaces::console->get_convar("cl_csm_shadows")->set_value(1);
				interfaces::console->get_convar("cl_foot_contact_shadows")->set_value(0);
				interfaces::console->get_convar("cl_csm_rot_override")->set_value(1);
				interfaces::console->get_convar("cl_csm_rot_x")->set_value(0);
				interfaces::console->get_convar("cl_csm_rot_y")->set_value(0);
				interfaces::console->get_convar("cl_csm_rot_z")->set_value(0);
				for (auto i = interfaces::material_system->first_material(); i != interfaces::material_system->invalid_material_handle(); i = interfaces::material_system->next_material(i)) {
					i_material* mat = interfaces::material_system->get_material(i);

					if (mat == nullptr || mat->is_error_material())
						continue;
					if (strstr(mat->get_texture_group_name(), "World")) {
						mat->color_modulate(0.15, 0.15, 0.15);
					}
					if (strstr(mat->get_texture_group_name(), "StaticProp")) {
						mat->color_modulate(0.4, 0.4, 0.4);
					}
				}
				nightmoded = true;
			}
			csgo::local_player->flash_duration() = 0;
			for (const auto& szSmokeMaterial : arrSmokeMaterials)
			{
				if (i_material* pMaterial = interfaces::material_system->find_material(szSmokeMaterial, TEXTURE_GROUP_OTHER); pMaterial != nullptr && !pMaterial->is_error_material())
					pMaterial->set_material_var_flag(material_var_no_draw, true);
			}
			old_aim_punch = csgo::local_player->aim_punch_angle();
			old_view_punch = csgo::local_player->punch_angle();
			csgo::local_player->aim_punch_angle() = vec3_t{};
			csgo::local_player->punch_angle() = vec3_t{};

			if (GetAsyncKeyState(VK_MBUTTON) & 1)
				thirdperson = !thirdperson;

			interfaces::input->bCameraInThirdPerson = csgo::local_player->is_alive() ? thirdperson : false;

			if (!csgo::local_player->is_alive())
				break;
			
			const auto animation_state = csgo::local_player->get_animation_stateb();

			if (!animation_state)
				break;
			if (m_should_animate) {

				// get real layers
				csgo::local_player->get_animation_layers(m_layers);
				// update animations
				csgo::local_player->update_client_side_animations();

				// the choke cycle has reset
				if (!interfaces::clientstate->choked_commands) {

					rotation.y = animation_state->m_flFootYaw;

					csgo::local_player->get_pose_parameters(m_poses);
				}
			

				m_should_animate = false;
			}


			// update layers, poses, and rotation
			csgo::local_player->set_animation_layers(m_layers);
			csgo::local_player->set_pose_parameters(m_poses);
			csgo::local_player->set_abs_angles(rotation);

			break;
		}
		case FRAME_RENDER_END: 
			if (csgo::local_player->is_alive()) {
				csgo::local_player->aim_punch_angle() = old_aim_punch;
				csgo::local_player->punch_angle() = old_view_punch;
			}
			break;
		
		default:

			break;
	}

	static int m_iLastCmdAck = 0;
	static float m_flNextCmdTime = 0.f;
	int framstage_minus2 = stage - 2;

	if (!framstage_minus2) {

		if ((m_iLastCmdAck != interfaces::clientstate->last_command_ack || m_flNextCmdTime != interfaces::clientstate->next_cmd_time))
		{
			if (m_flVelocityModifier != csgo::local_player->velocitymodifier())
			{

				*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(interfaces::prediction + 0x24)) = 1;
				m_flVelocityModifier = csgo::local_player->velocitymodifier();
			}

			m_iLastCmdAck = interfaces::clientstate->last_command_ack;
			m_flNextCmdTime = interfaces::clientstate->next_cmd_time;
		}
	}

	frame_stage_notify_original(stage);
}

void __fastcall hooks::override_view::hook(void* ecx, int edx, view_setup_t* setup) {
	if (!csgo::local_player || !interfaces::engine->is_connected() || !interfaces::engine->is_in_game())
		return override_view_original(ecx, setup);

	if (!csgo::local_player->is_alive()) {
		*csgo::local_player->observer_mode() = 5;
	}
	interfaces::console->get_convar("zoom_sensitivity_ratio_mouse")->set_value(0);
	interfaces::console->get_convar("r_aspectratio")->set_value(1);
	setup->fov = 135;

	if (GetAsyncKeyState(0x58)) {
		setup->origin.z = csgo::local_player->abs_origin().z + 64;
	}

	if (csgo::local_player->active_weapon()) {
		int idx = csgo::local_player->active_weapon()->item_definition_index();

		if (idx == WEAPON_FLASHBANG || idx == WEAPON_HEGRENADE || idx == WEAPON_SMOKEGRENADE || idx == WEAPON_MOLOTOV || idx == WEAPON_DECOY || idx == WEAPON_INCGRENADE)
			GrenadePrediction::get().View(setup, csgo::local_player->active_weapon());
	}

	override_view_original(ecx, setup);
}

bool init = false;
void __fastcall hooks::draw_model_execute::hook(void* ecx, int edx, i_mat_render_context* context, const draw_model_state_t& state, const model_render_info_t& info, matrix_t* matrix) {
	

	if (!init) {
	std::ofstream("csgo/materials/glowOverlay.vmt") << R"#("VertexLitGeneric" {
		"ignorez" "1"
		"$envmapsaturation" "1"
		"$selfillum" "1"
		"$envmap" "models/effects/cube_white"
		"$envmaptint" "[0.25 0.22 1]"
		"$envmapfresnel" "1"
		"$envmapfresnelminmaxexp" "[0 1 2]"
		"$alpha" "1"
	})#";

	std::ofstream("csgo/materials/whiteglowOverlay.vmt") << R"#("VertexLitGeneric" {
		"ignorez" "1"
		"$envmapsaturation" "1"
		"$selfillum" "1"
		"$envmap" "models/effects/cube_white"
		"$envmaptint" "[0.6 0.6 0.6]"
		"$envmapfresnel" "1"
		"$envmapfresnelminmaxexp" "[0 0.1 0.8]"
		"$alpha" "1"
	})#";

	std::ofstream("csgo/materials/animWire.vmt") << R"#( "VertexLitGeneric" {
		"$basetexture" "nature/urban_puddle01a_ssbump"
		"$additive" "1"
		"$selfillum" "1"
		"$wireframe" "0"
		"$ignorez" "0"
		"$alpha" "1"
		Proxies
		{
			TextureScroll {
				texturescrollvar $BasetextureTransform
				texturescrollrate 0.25
				texturescrollangle 135
		   }
		}
	})#";

	std::ofstream("csgo\\materials\\simple_regular_reflective.vmt") << R"#("VertexLitGeneric"
	{
		"$basetexture" "vgui/white_additive"
		"$ignorez"      "0"
		"$envmap"       "env_cubemap"
		"$normalmapalphaenvmapmask"  "1"
		"$envmapcontrast"             "1"
		"$nofog"        "1"
		"$model"        "1"
		"$nocull"       "0"
		"$selfillum"    "1"
		"$halflambert"  "1"
		"$znearer"      "0"
		"$flat"         "1"
		}
	)#";

		flat = interfaces::material_system->find_material("debug/debugdrawflat", TEXTURE_GROUP_MODEL);
		flat->increment_reference_count();
		flat->refresh();

		glow = interfaces::material_system->find_material("glowOverlay", TEXTURE_GROUP_MODEL);
		glow->increment_reference_count();
		glow->refresh();

		whiteglow = interfaces::material_system->find_material("whiteglowOverlay", TEXTURE_GROUP_MODEL);
		whiteglow->increment_reference_count();
		whiteglow->refresh();

		wire = interfaces::material_system->find_material("animWire", TEXTURE_GROUP_MODEL);
		wire->increment_reference_count();
		wire->refresh();

		metal = interfaces::material_system->find_material("simple_regular_reflective", TEXTURE_GROUP_MODEL);
		metal->increment_reference_count();
		metal->refresh();

		i_material* xblur_mat = interfaces::material_system->find_material("dev/blurfilterx_nohdr", TEXTURE_GROUP_OTHER, true);
		xblur_mat->increment_reference_count();
		i_material* yblur_mat = interfaces::material_system->find_material("dev/blurfiltery_nohdr", TEXTURE_GROUP_OTHER, true);
		yblur_mat->increment_reference_count();
		i_material* scope = interfaces::material_system->find_material("dev/scope_bluroverlay", TEXTURE_GROUP_OTHER, true);
		scope->increment_reference_count();

		xblur_mat->set_material_var_flag(material_var_no_draw, true);
		yblur_mat->set_material_var_flag(material_var_no_draw, true);
		scope->set_material_var_flag(material_var_no_draw, true);

		init = true;
	}

	if (interfaces::model_render->is_override_material())
		return 	draw_model_execute_original(ecx, context, state, info, matrix);

	entity_t* entity = (entity_t*)interfaces::entity_list->get_client_entity(info.entity_index);
	
	if (csgo::local_player && entity != nullptr && entity->is_player() && entity->team() != csgo::local_player->team() && entity != csgo::local_player) {
		flat->set_material_var_flag(material_var_ignorez, true);
		flat->alpha_modulate(0.3f);
		flat->color_modulate(1.0f, 1.0f, 1.0f);
		interfaces::model_render->override_material(flat);
		draw_model_execute_original(ecx, context, state, info, matrix);

		flat->alpha_modulate(0.1f);

		if (ragebot::records[entity->index()].size() != 0)
			draw_model_execute_original(ecx, context, state, info, ragebot::lastvalidrecord(ragebot::records[entity->index()]).matrixdata.data());

		interfaces::model_render->override_material(nullptr);
		return;
	}
	if (entity == csgo::local_player && false) {
		glow->alpha_modulate(0.3);
		interfaces::model_render->override_material(glow);
		draw_model_execute_original(ecx, context, state, info, desyncmatrix.data());
		glow->alpha_modulate(1);
		interfaces::model_render->override_material(nullptr);

		draw_model_execute_original(ecx, context, state, info, matrix);
		return;
	}
	if (false && strstr(info.model->name, ("arms")) && strstr(info.model->name, ("sleeve"))) {
		glow->set_material_var_flag(material_var_additive, true);
		glow->alpha_modulate(0.3f);

		interfaces::model_render->override_material(glow);
	}

	if (false && strstr(info.model->name, ("arms")) && !strstr(info.model->name, ("sleeve"))) {
		glow->set_material_var_flag(material_var_additive, true);
		glow->alpha_modulate(0.3f);

	interfaces::model_render->override_material(glow);
	}

	if ((strstr(info.model->name, ("weapons")) && !strstr(info.model->name, ("arms"))) || (strstr(info.model->name, "weapons/w_") && ((player_t*)interfaces::entity_list->get_client_entity_handle(((entity_t*)interfaces::entity_list->get_client_entity(info.entity_index))->owner_handle())) == csgo::local_player ))
	{
		//wire->set_material_var_flag(material_var_wireframe, false);
		//wire->color_modulate(0.25f, 0.22f, 1.f);
		//draw_model_execute_original(ecx, context, state, info, matrix);
		//interfaces::model_render->override_material(wire);
		metal->color_modulate(5.f/255.f, 182.f/255.f, 252.f/255.f);
		interfaces::model_render->override_material(metal);
	}

	if (!strstr(info.renderable->GetModel()->name, visuals::vpath) && strstr(info.model->name, ("arms"))) {
		return;
	}

	draw_model_execute_original(ecx, context, state, info, matrix);
	interfaces::model_render->override_material(nullptr);
	
	flat->alpha_modulate(1);
}

