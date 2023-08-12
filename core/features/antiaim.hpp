#include "../../dependencies/utilities/csgo.hpp"
#include "../menu/variables.hpp"
#include "../features/features.hpp"

namespace antiaim {
	void run(c_usercmd* cmd);
	float at_target(c_usercmd* cmd);

	inline float nextlbyupdate = 0.0f;
	inline vec3_t sentview = vec3_t();
}