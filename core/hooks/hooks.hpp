#pragma once
#include "../utilities/csgo.hpp"

namespace hooks {
	bool initialize();
	void release();
	
	inline unsigned int get_virtual(void* _class, unsigned int index) { return static_cast<unsigned int>((*static_cast<int**>(_class))[index]); }

	namespace create_move {
		constexpr int num = 24;
		using fn = bool(__stdcall*)(float, c_usercmd*);
		bool __stdcall hook(float input_sample_frametime, c_usercmd* cmd);
	};
	namespace create_move_proxy {
		constexpr int num = 22;
		using fn = void(__fastcall*)(i_base_client_dll* , int , int , float , bool );
		void __fastcall hook(i_base_client_dll* thisptr, int edx, int nSequenceNumber, float flInputSampleFrametime, bool bIsActive);
	}
	namespace paint_traverse {
		constexpr int num = 41;
		using fn = void(__thiscall*)(i_panel*, unsigned int, bool, bool);
		void __stdcall hook(unsigned int panel, bool force_repaint, bool allow_force);
	}

	namespace frame_stage_notify { // MAKE INTO __STDCALL
		constexpr int num = 37;
		using fn = void(__stdcall*)(i_net_channel_info::EClientFrameStage);
		void __stdcall hook(i_net_channel_info::EClientFrameStage stage);
	}

	namespace render_smoke_overlay {
		constexpr int num = 41;
		using fn = void(__thiscall*)(void*, bool);
		void __fastcall hook(void* ecx, int edx, bool previewmodel);
	}
	
	namespace draw_model_execute {
		constexpr int num = 21;
		using fn = void(__thiscall*)(void*, i_mat_render_context*, const draw_model_state_t&, const model_render_info_t&, matrix_t*);
		void __fastcall hook(void* ecx, int edx, i_mat_render_context* context, const draw_model_state_t& state, const model_render_info_t& info, matrix_t* matrix);
	}
	namespace should_skip_animation_frame {
		constexpr char sig[] = "57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02";
		using fn = void(__thiscall*)(void*, void*);
		bool __fastcall hook(void* this_pointer, void* edx);
	}
	namespace do_procedural_foot_plant {
		constexpr char sig[] = "55 8B EC 83 E4 F0 83 EC 78 56 8B F1 57 8B 56";
		using fn = void(__thiscall*)(void*, void*, void*, void*, void*, void*);
		void __fastcall hook(void* this_pointer, void* edx, void* bone_to_world, void* left_foot_chain, void* right_foot_chain, void* pos);
	}
	namespace check_for_sequence_change {
		constexpr char sig[] = "55 8B EC 51 53 8B 5D 08 56 8B F1 57 85";
		using fn = void(__fastcall*)(void*, void*, void*, int, bool, bool);
		void __fastcall hook(void* this_pointer, void* edx, void* hdr, int cur_sequence, bool force_new_sequence, bool interpolate);
	}
	namespace standard_blending_rules {
		constexpr char sig[] = "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6";
		using fn = void(__fastcall*)(void*, void*, void*, void*, void*, float, int);
		void __fastcall hook(void* this_pointer, void* edx, void* hdr, void* pos, void* q, float current_time, int bone_mask);
	}
	namespace calculate_view {
		constexpr char sig[] = "55 8B EC 83 EC 14 53 56 57 FF 75 18";
		using fn = void(__fastcall*)(void*, void*, vec3_t&, vec3_t&, float&, float&, float&);
		void __fastcall hook(void* this_pointer, void* edx, vec3_t& eye_origin, vec3_t& eye_angles, float& z_near, float& z_far, float& fov);
	}
	namespace modify_eye_position {
		constexpr char sig[] = "55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 14";
		using fn = void(__fastcall*)(void*, void*, vec3_t&);
		void __fastcall hook(void* this_pointer, void* edx, vec3_t& input_eye_position);
	}
	namespace update_animation_state {
		constexpr char sig[] = "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3";
		using fn = void(__vectorcall*)(void*, void*, float, float, float, void*);
		void __vectorcall hook(void* this_pointer, void* unknown, float z, float y, float x, void* unknown1);
	}
	namespace update_client_side_animation {
		constexpr char sig[] = "55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74";
		using fn = void(__fastcall*)(void*, void*);
		void __fastcall hook(void* this_pointer, void* edx);
	}
	
	namespace build_transformations {
		constexpr char sig[] = "55 8B EC 83 E4 F0 81 ? ? ? ? ? 56 57 8B F9 8B";
		using fn = void(__fastcall*)(void*, void*, studio_hdr_t*, void*, void*, const void*, int, void*);
		void __fastcall hook(void* this_pointer, void* edx, studio_hdr_t* hdr, void* pos, void* q, const void* camera_transform, int bone_mask, void* bone_computed);
	}
	namespace override_view {
		constexpr int num = 18; 
		using fn = void(__thiscall*) (void*, view_setup_t*);
		void __fastcall hook(void* ecx, int edx, view_setup_t* setup);
	}
	namespace is_hltv {
		constexpr int num = 93;
		using fn = bool(__fastcall*)(void*, void*);
		bool __fastcall hook(void* this_pointer, void* edx);
	}
	namespace calc_view_bob {
		constexpr char sig[] = "55 8B EC A1 ? ? ? ? 83 EC 10 56 8B F1 B9";
		using fn = void(__thiscall*)(void*, void*);
		void __fastcall hook(void* ecx, void* edx, void* origin);
	}
	namespace is_paused {
		constexpr int num = 90;
		using fn = bool(__stdcall*)();
		bool __stdcall hook();
	}
	namespace should_draw_view_model {
		constexpr int num = 27;
		using fn = bool(__stdcall*)();
		bool __stdcall hook();
	}
	namespace cl_move {
		constexpr char sig[] = "55 8B EC 81 EC ? ? ? ? 53 56 8A";
		using fn = void(__cdecl*)(float, bool);
		void __cdecl hook(float accumulatedExtraSamples, bool finalTick);
	}
	namespace run_command { 
		constexpr int num = 19;
		using fn = void(__fastcall*)(void*, void*, player_t*, c_usercmd*, void*);
		void __fastcall hook(void* ecx, void* edx, player_t* player, c_usercmd* cmd, void* movehelper);
	}
	namespace physics_simulate {
		constexpr char sig[] = "56 8B F1 8B ? ? ? ? ? 83 F9 FF 74 23";
		using fn = void(__fastcall*)(player_t*);
		void __fastcall hook(player_t* player);
	}
	namespace list_leaves_in_box {
		constexpr int num = 6;
		using fn = void(__thiscall*)(void*, const vec3_t&, const vec3_t&, void*, void*);
		void __fastcall hook(void* ecx, void* edx, const vec3_t& vecmins, const vec3_t& vecmaxs, void* pulist, void* listmax);
	}
	namespace do_post_screen_effects {
		constexpr int num = 44;
		using fn = int(__thiscall*)(void*, void*);
		int __fastcall hook(void* ecx, void* edx, void* setup);
	}
	namespace packet_end {
		constexpr char sig[] = "55 8B EC 83 EC 18 53 8B D9 E8 ? ? ? ? 83 ? ? ? ? ? ? 74";
		using fn = void(__thiscall*)(void*);
		void __fastcall hook(void* ecx, void* edx);
	}
	namespace packet_start {
		constexpr int num = 6;
		using fn = void(__thiscall*)(void*, int, int);
		void __fastcall hook(void* ecx, void* edx, int incoming, int outgoing);
	}
	namespace scene_end {
		constexpr int num = 9;
		using fn = void(__thiscall*)(void*);
		void __fastcall hook(void* ecx, void* edx);
	}
	namespace get_exposure_range {
		constexpr char sig[] = "55 8B EC 51 80 3D ? ? ? ? ? 0F 57";
		using fn = void(__fastcall*)(float*, float*);
		void __fastcall hook(float* min, float* max);
	}
	namespace alloc_key_values_memory {
		constexpr int num = 2;
		using fn = void* (__thiscall*)(void*, int);
		void* __fastcall hook(void* ecx, void* edx, int size);
	}
	namespace find_mdl {
		constexpr int num = 10;
		using fn = MDLHandle_t(__thiscall*)(void*, const char*);
		MDLHandle_t __fastcall hook(void* ecx, void* edx, const char* name);
	}
	namespace sendnetmsg {
		constexpr int num = 40;
		using fn = bool(__thiscall*)(void*, i_net_message*, bool, bool);
		bool __fastcall hook(void* ecx, void* edx, i_net_message* message, bool force_reliable, bool voice);
	}

}

class Address {
protected:
	uint8_t* m_addr;

public:
	// default ctor/dtor.
	__forceinline  Address() : m_addr{} {};
	__forceinline ~Address() {};

	// ctors.
	__forceinline Address(uint8_t* a) : m_addr{ a } {}
	__forceinline Address(const void* a) : m_addr{ (uint8_t*)a } {}

	// arithmetic operators for native types.
	__forceinline operator uint8_t*() { return m_addr; }
	__forceinline operator void* () { return (void*)m_addr; }
	__forceinline operator const void* () { return (const void*)m_addr; }

	// is-equals-operator.
	__forceinline bool operator==(Address a) const {
		return as< uint8_t* >() == a.as< uint8_t* >();
	}
	__forceinline bool operator!=(Address a) const {
		return as< uint8_t* >() != a.as< uint8_t* >();
	}

	// cast / add offset and cast.
	template< typename t = Address >
	__forceinline t as() const {
		return (m_addr) ? (t)m_addr : t{};
	}

	template< typename t = Address >
	__forceinline t as(size_t offset) const {
		return (m_addr) ? (t)(m_addr + offset) : t{};
	}

	template< typename t = Address >
	__forceinline t as(ptrdiff_t offset) const {
		return (m_addr) ? (t)(m_addr + offset) : t{};
	}

	// add offset and dereference.
	template< typename t = Address >
	__forceinline t at(size_t offset) const {
		return (m_addr) ? *(t*)(m_addr + offset) : t{};
	}

	template< typename t = Address >
	__forceinline t at(ptrdiff_t offset) const {
		return (m_addr) ? *(t*)(m_addr + offset) : t{};
	}

	// add offset.
	template< typename t = Address >
	__forceinline t add(size_t offset) const {
		return (m_addr) ? (t)(m_addr + offset) : t{};
	}

	template< typename t = Address >
	__forceinline t add(ptrdiff_t offset) const {
		return (m_addr) ? (t)(m_addr + offset) : t{};
	}

	// subtract offset.
	template< typename t = Address >
	__forceinline t sub(size_t offset) const {
		return (m_addr) ? (t)(m_addr - offset) : t{};
	}

	template< typename t = Address >
	__forceinline t sub(ptrdiff_t offset) const {
		return (m_addr) ? (t)(m_addr - offset) : t{};
	}

	// dereference.
	template< typename t = Address >
	__forceinline t to() const {
		return *(t*)m_addr;
	}

	// verify adddress and dereference n times.
	template< typename t = Address >
	__forceinline t get(size_t n = 1) {
		uint8_t* out;

		if (!m_addr)
			return t{};

		out = m_addr;

		for (size_t i{ n }; i > 0; --i) {
			// can't dereference, return null.
			if (!valid(out))
				return t{};

			out = *(uint8_t**)out;
		}

		return (t)out;
	}

	// follow relative8 and relative16/32 offsets.
	template< typename t = Address >
	__forceinline t rel8(size_t offset) {
		uint8_t*   out;
		uint8_t     r;

		if (!m_addr)
			return t{};

		out = m_addr + offset;

		// get relative offset.
		r = *(uint8_t*)out;
		if (!r)
			return t{};

		// relative to address of next instruction.
		// short jumps can go forward and backward depending on the size of the second byte.
		// if the second byte is below 128, the jmp goes forwards.
		// if the second byte is above 128, the jmp goes backwards ( subtract two's complement of the relative offset from the address of the next instruction ).
		if (r < 128)
			out = (out + 1) + r;
		else
			out = (out + 1) - (uint8_t)(~r + 1);

		return (t)out;
	}

	template< typename t = Address >
	__forceinline t rel32(size_t offset) {
		uint8_t*   out;
		uint32_t    r;

		if (!m_addr)
			return t{};

		out = m_addr + offset;

		// get rel32 offset.
		r = *(uint32_t*)out;
		if (!r)
			return t{};

		// relative to address of next instruction.
		out = (out + 4) + r;

		return (t)out;
	}

	// set.
	template< typename t = uint8_t* > __forceinline void set(const t& value) {
		if (!m_addr)
			return;

		*(t*)m_addr = value;
	}

	// checks if address is not null and has correct page protection.
	static __forceinline bool valid(uint8_t* addr) {
		MEMORY_BASIC_INFORMATION mbi;

		// check for invalid address.
		if (!addr)
			return false;

		// check for invalid page protection.
		if (!VirtualQuery((const void*)addr, &mbi, sizeof(mbi)))
			return false;

		// todo - dex; fix this, its wrong... check for rwe or something too
		if ( /*!( mbi.State & MEM_COMMIT ) ||*/ (mbi.Protect & PAGE_NOACCESS) || (mbi.Protect & PAGE_GUARD))
			return false;

		return true;
	}

	// relative virtual address.
	template< typename t = Address >
	static __forceinline t RVA(Address base, size_t offset) {
		return base.as< t >(offset);
	}
};

namespace mem {
	__forceinline Address copy(Address dst, Address src, size_t size) {
		__movsb(
			dst.as<uint8_t*>(),
			src.as<uint8_t*>(),
			size
		);

		return dst;
	}

	// memset
	__forceinline Address set(Address dst, uint8_t val, size_t size) {
		__stosb(
			dst.as<uint8_t*>(),
			val,
			size
		);

		return dst;
	}

	template< typename o = void*, typename i = void* >
	__forceinline o force_cast(i in) {
		union { i in; o out; }
		u = { in };
		return u.out;
	};

	template < typename t = Address >
	__forceinline static t get_method(Address this_ptr, size_t index) {
		return (t)this_ptr.to< t* >()[index];
	}

	// get base ptr ( EBP (x86_32) / RBP (x86_64) ).
	__forceinline uint8_t* GetBasePointer() {
		return (uint8_t*)_AddressOfReturnAddress() - sizeof(uint8_t*);
	}

	// wide -> multi-byte
	__forceinline std::string WideToMultiByte(const std::wstring& str) {
		std::string ret;
		int         str_len;

		// check if not empty str
		if (str.empty())
			return {};

		// count size
		str_len = WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.size(), 0, 0, 0, 0);

		// setup return value
		ret = std::string(str_len, 0);

		// final conversion
		WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.size(), &ret[0], str_len, 0, 0);

		return ret;
	}

	// multi-byte -> wide
	__forceinline std::wstring MultiByteToWide(const std::string& str) {
		std::wstring    ret;
		int		        str_len;

		// check if not empty str
		if (str.empty())
			return {};

		// count size
		str_len = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0);

		// setup return value
		ret = std::wstring(str_len, 0);

		// final conversion
		MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &ret[0], str_len);

		return ret;
	}
};

class Stack : public Address {
public:
	// ctor.
	__forceinline Stack() : Address{ mem::GetBasePointer() } {}

	// get return address.
	__forceinline Address ReturnAddress() {
		if (!m_addr)
			return Address{};

		return *(uint8_t**)(m_addr + sizeof(uint8_t*));
	}

	// get address of return address.
	__forceinline Address AddressOfReturnAddress() {
		if (!m_addr)
			return Address{};

		return (m_addr + sizeof(uint8_t*));
	}

	// go up stack frame.
	__forceinline Stack& next() {
		return *(Stack*)m_addr;
	}

	// get local variable.
	template< typename t = Address >
	__forceinline t local(size_t offset) {
		return (t)(m_addr - offset);
	}

	// get arg.
	template< typename t = Address >
	__forceinline t arg(size_t offset) {
		return (t)(m_addr + offset);
	}
};


inline bool precache_model(const char* szModelName)
{
	if (auto pModelPrecache = interfaces::network_container->FindTable("modelprecache"); pModelPrecache != nullptr)
	{
		if (interfaces::model_info->find_or_load_model(szModelName) == nullptr)
			return false;

		if (pModelPrecache->AddString(false, szModelName) == INVALID_STRING_INDEX)
			return false;
	}

	return true;
}
