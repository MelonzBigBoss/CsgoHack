#pragma once
#include <vector>
#include "../math/utl_vector.hpp"
class convar;
using fn_change_callback_t = void(*)(convar* var, const char* old_value, float f_old_value);

enum cvar_flags {
	fcvar_none = 0,
	fcvar_unregistered = (1 << 0),
	fcvar_developmentonly = (1 << 1),
	fcvar_gamedll = (1 << 2),
	fcvar_clientdll = (1 << 3),
	fcvar_hidden = (1 << 4),
	fcvar_protected = (1 << 5),
	fcvar_sponly = (1 << 6),
	fcvar_archive = (1 << 7),
	fcvar_notify = (1 << 8),
	fcvar_userinfo = (1 << 9),
	fcvar_printableonly = (1 << 10),
	fcvar_unlogged = (1 << 11),
	fcvar_never_as_string = (1 << 12),
	fcvar_replicated = (1 << 13),
	fcvar_cheat = (1 << 14),
	fcvar_ss = (1 << 15),
	fcvar_demo = (1 << 16),
	fcvar_dontrecord = (1 << 17),
	fcvar_ss_added = (1 << 18),
	fcvar_release = (1 << 19),
	fcvar_reload_materials = (1 << 20),
	fcvar_reload_textures = (1 << 21),
	fcvar_not_connected = (1 << 22),
	fcvar_material_system_thread = (1 << 23),
	fcvar_archive_xbox = (1 << 24),
	fcvar_accessible_from_threads = (1 << 25),
	fcvar_server_can_execute = (1 << 28),
	fcvar_server_cannot_query = (1 << 29),
	fcvar_clientcmd_can_execute = (1 << 30),
	fcvar_unused = (1 << 31),
	fcvar_material_thread_mask = (fcvar_reload_materials | fcvar_reload_textures | fcvar_material_system_thread)
};

class convar {
public:
	void set_value(const char* value) {
		using original_fn = void(__thiscall*)(convar*, const char*);
		return (*(original_fn * *)this)[14](this, value);
	}
	void set_value(float value) {
		using original_fn = void(__thiscall*)(convar*, float);
		return (*(original_fn * *)this)[15](this, value);
	}
	void set_value(int value) {
		using original_fn = void(__thiscall*)(convar*, int);
		return (*(original_fn * *)this)[16](this, value);
	}
	void set_value(bool value) {
		using original_fn = void(__thiscall*)(convar*, int);
		return (*(original_fn * *)this)[16](this, static_cast<int>(value));
	}
	float GetFloat() // idx @ 11
	{
		std::uint32_t uXored = *reinterpret_cast<std::uint32_t*>(&pParent->float_value) ^ reinterpret_cast<std::uint32_t>(this);
		return *reinterpret_cast<float*>(&uXored);
	}

	int GetInt() // idx @ 12
	{
		return static_cast<int>(pParent->numerical_value ^ reinterpret_cast<int>(this));
	}
	bool GetBool() // idx @ 13
	{
		return !!GetInt();
	}

private:
	char pad_0x0000[0x4];

public:
	convar* next;
	bool is_registered;
	char* name;
	char* help_string;
	int flags;

private:
	char pad_0x0018[0x4];

public:
	convar* pParent;									//0x1C
	const char* szDefaultValue;							//0x20
	char* szString;										//0x24
	int								iStringLength;		//0x28
	float							float_value;		//0x2C
	int								numerical_value;				//0x30
	bool							bHasMin;			//0x34
	float							flMinValue;			//0x38
	bool							bHasMax;			//0x3C
	float							flMaxValue;			//0x40
	utl_vector<fn_change_callback_t> callbacks;
};
