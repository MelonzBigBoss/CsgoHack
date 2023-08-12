#include "../../dependencies/utilities/csgo.hpp"
#include "../menu/variables.hpp"
#include "../features/features.hpp"
#include "grenade_warning.hpp"
namespace visuals {
	void do_oschams();
	void custommodels();
	void knifechanger();
	void player_esp();
	void world_esp();

	constexpr char path[] = "models/player/custom_player/kuristaja/motoko/motoko.mdl";
	constexpr char vpath[] = "models/player/custom_player/kuristaja/motoko/motoko_arms.mdl";
	constexpr char wpath[] = "models/weapons/counter-strike-source/eminem/v_pist_deagle_fix.mdl";
	constexpr int rweapon = WEAPON_DEAGLE;
	//constexpr char wwpath[] = "models/weapons/eminem/dsr_50/w_dsr_50_dropped_v4.mdl";
}
