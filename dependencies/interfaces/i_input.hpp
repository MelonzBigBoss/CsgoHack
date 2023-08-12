#pragma once
#include "../../source-sdk/classes/c_usercmd.hpp"

class i_input {
public:
	char			    pad0[0xC];				// 0x00
	bool				bTrackIRAvailable;		// 0x0C
	bool				bMouseInitialized;		// 0x0D
	bool				bMouseActive;			// 0x0E
	char			    pad1[0x9A];				// 0x0F
	bool				bCameraInThirdPerson;	// 0xA9
	char			    pad2[0x2];				// 0xAA
	vec3_t				vecCameraOffset;		// 0xAC
	char			    pad3[0x38];				// 0xB8
	c_usercmd*          pCommands;				// 0xF0
	c_vusercmd*				pVerifiedCommands;		// 0xF4

	c_usercmd *get_user_cmd( int sequence_num ) {
		return &pCommands[sequence_num % 150];
	}
	c_vusercmd* get_vuser_cmd(int sequence_num) {
		return &pVerifiedCommands[sequence_num % 150];
	}
};
static_assert(sizeof(i_input) == 0xF8);