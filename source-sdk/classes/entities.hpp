#pragma once
#include "../../dependencies/math/math.hpp"
#include <array>
#include "collideable.hpp"
#include "client_class.hpp"
#include "../structs/animstate.hpp"
#include "../../dependencies/utilities/netvars/netvars.hpp"
#include "../structs/box_t.hpp"
#include "../structs/datamap.hpp"

#define NUM_ENT_ENTRY_BITS         (11 + 2)
#define NUM_ENT_ENTRIES            (1 << NUM_ENT_ENTRY_BITS)
#define INVALID_EHANDLE_INDEX       0xFFFFFFFF
#define NUM_SERIAL_NUM_BITS        16 // (32 - NUM_ENT_ENTRY_BITS)
#define NUM_SERIAL_NUM_SHIFT_BITS (32 - NUM_SERIAL_NUM_BITS)
#define ENT_ENTRY_MASK             (( 1 << NUM_SERIAL_NUM_BITS) - 1)

enum data_update_type_t {
	DATA_UPDATE_CREATED = 0,
	DATA_UPDATE_DATATABLE_CHANGED,
};

enum cs_weapon_type {
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_PLACEHOLDER,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN
};

enum client_frame_stage_t {
	FRAME_UNDEFINED = -1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};

enum move_type {
	movetype_none = 0,
	movetype_isometric,
	movetype_walk,
	movetype_step,
	movetype_fly,
	movetype_flygravity,
	movetype_vphysics,
	movetype_push,
	movetype_noclip,
	movetype_ladder,
	movetype_observer,
	movetype_custom,
	movetype_last = movetype_custom,
	movetype_max_bits = 4,
	max_movetype
};

enum entity_flags {
	fl_onground = (1 << 0),
	fl_ducking = (1 << 1),
	fl_waterjump = (1 << 2),
	fl_ontrain = (1 << 3),
	fl_inrain = (1 << 4),
	fl_frozen = (1 << 5),
	fl_atcontrols = (1 << 6),
	fl_client = (1 << 7),
	fl_fakeclient = (1 << 8),
	fl_inwater = (1 << 9),
	fl_fly = (1 << 10),
	fl_swim = (1 << 11),
	fl_conveyor = (1 << 12),
	fl_npc = (1 << 13),
	fl_godmode = (1 << 14),
	fl_notarget = (1 << 15),
	fl_aimtarget = (1 << 16),
	fl_partialground = (1 << 17),
	fl_staticprop = (1 << 18),
	fl_graphed = (1 << 19),
	fl_grenade = (1 << 20),
	fl_stepmovement = (1 << 21),
	fl_donttouch = (1 << 22),
	fl_basevelocity = (1 << 23),
	fl_worldbrush = (1 << 24),
	fl_object = (1 << 25),
	fl_killme = (1 << 26),
	fl_onfire = (1 << 27),
	fl_dissolving = (1 << 28),
	fl_transragdoll = (1 << 29),
	fl_unblockable_by_player = (1 << 30)
};
enum item_definition_indexes {
	WEAPON_NONE = 0,
	WEAPON_DEAGLE,
	WEAPON_ELITE,
	WEAPON_FIVESEVEN,
	WEAPON_GLOCK,
	WEAPON_AK47 = 7,
	WEAPON_AUG,
	WEAPON_AWP,
	WEAPON_FAMAS,
	WEAPON_G3SG1,
	WEAPON_GALILAR = 13,
	WEAPON_M249,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10,
	WEAPON_P90 = 19,
	WEAPON_MP5SD = 23,
	WEAPON_UMP45,
	WEAPON_XM1014,
	WEAPON_BIZON,
	WEAPON_MAG7,
	WEAPON_NEGEV,
	WEAPON_SAWEDOFF,
	WEAPON_TEC9,
	WEAPON_TASER,
	WEAPON_HKP2000,
	WEAPON_MP7,
	WEAPON_MP9,
	WEAPON_NOVA,
	WEAPON_P250,
	WEAPON_SHIELD,
	WEAPON_SCAR20,
	WEAPON_SG556,
	WEAPON_SSG08,
	WEAPON_KNIFEGG,
	WEAPON_KNIFE,
	WEAPON_FLASHBANG,
	WEAPON_HEGRENADE,
	WEAPON_SMOKEGRENADE,
	WEAPON_MOLOTOV,
	WEAPON_DECOY,
	WEAPON_INCGRENADE,
	WEAPON_C4,
	WEAPON_HEALTHSHOT = 57,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER,
	WEAPON_USP_SILENCER,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER,
	WEAPON_TAGRENADE = 68,
	WEAPON_FISTS,
	WEAPON_BREACHCHARGE,
	WEAPON_TABLET = 72,
	WEAPON_MELEE = 74,
	WEAPON_AXE,
	WEAPON_HAMMER,
	WEAPON_SPANNER = 78,
	WEAPON_KNIFE_GHOST = 80,
	WEAPON_FIREBOMB,
	WEAPON_DIVERSION,
	WEAPON_FRAG_GRENADE,
	WEAPON_SNOWBALL,
	WEAPON_BUMPMINE,
	WEAPON_BAYONET = 500,
	WEAPON_KNIFE_CSS = 503,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT,
	WEAPON_KNIFE_KARAMBIT,
	WEAPON_KNIFE_M9_BAYONET,
	WEAPON_KNIFE_TACTICAL,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY,
	WEAPON_KNIFE_PUSH,
	WEAPON_KNIFE_URSUS = 519,
	WEAPON_KNIFE_GYPSY_JACKKNIFE,
	WEAPON_KNIFE_STILETTO = 522,
	WEAPON_KNIFE_WIDOWMAKER,
	GLOVE_STUDDED_BLOODHOUND = 5027,
	GLOVE_T_SIDE = 5028,
	GLOVE_CT_SIDE = 5029,
	GLOVE_SPORTY = 5030,
	GLOVE_SLICK = 5031,
	GLOVE_LEATHER_WRAP = 5032,
	GLOVE_MOTORCYCLE = 5033,
	GLOVE_SPECIALIST = 5034,
	GLOVE_HYDRA = 5035
};

class entity_t;
class collideable_t;
class i_client_networkable;
class i_client_renderable;
class i_client_entity;
class i_client_thinkable;
class i_handle_entity;

class CBaseHandle { //-V690
public:
	CBaseHandle();
	CBaseHandle(const CBaseHandle& other);
	CBaseHandle(unsigned long value);
	CBaseHandle(int iEntry, int iSerialNumber);

	void Init(int iEntry, int iSerialNumber);
	void Term();

	// Even if this returns true, Get() still can return return a non-null value.
	// This just tells if the handle has been initted with any values.
	bool IsValid() const;

	int GetEntryIndex() const;
	int GetSerialNumber() const;

	int ToInt() const;
	unsigned long ToLong() const;

	bool operator !=(const CBaseHandle& other) const;
	bool operator ==(const CBaseHandle& other) const;
	bool operator ==(const i_handle_entity* e) const;
	bool operator !=(const i_handle_entity* e) const;
	bool operator <(const CBaseHandle& other) const;

	// Assign a value to the handle.
	const CBaseHandle& operator=(const i_handle_entity* pEntity);
	const CBaseHandle& Set(const i_handle_entity* pEntity);

	// Use this to dereference the handle.
	// Note: this is implemented in game code (ehandle.h)
	i_handle_entity* Get() const;

protected:
	// The low NUM_SERIAL_BITS hold the index. If this value is less than MAX_EDICTS, then the entity is networkable.
	// The high NUM_SERIAL_NUM_BITS bits are the serial number.
	unsigned long	m_Index;
};

inline CBaseHandle::CBaseHandle() {
	m_Index = INVALID_EHANDLE_INDEX;
}

inline CBaseHandle::CBaseHandle(const CBaseHandle& other) {
	m_Index = other.m_Index;
}

inline CBaseHandle::CBaseHandle(unsigned long value) {
	m_Index = value;
}

inline CBaseHandle::CBaseHandle(int iEntry, int iSerialNumber) {
	Init(iEntry, iSerialNumber);
}

inline void CBaseHandle::Init(int iEntry, int iSerialNumber) {
	m_Index = iEntry | (iSerialNumber << NUM_ENT_ENTRY_BITS);
}

inline void CBaseHandle::Term() {
	m_Index = INVALID_EHANDLE_INDEX;
}

inline bool CBaseHandle::IsValid() const {
	return m_Index != INVALID_EHANDLE_INDEX;
}

inline int CBaseHandle::GetEntryIndex() const {
	return m_Index & ENT_ENTRY_MASK;
}

inline int CBaseHandle::GetSerialNumber() const {
	return m_Index >> NUM_ENT_ENTRY_BITS;
}

inline int CBaseHandle::ToInt() const {
	return (int)m_Index;
}

inline unsigned long CBaseHandle::ToLong() const {
	return (unsigned long)m_Index;
}

inline bool CBaseHandle::operator !=(const CBaseHandle& other) const {
	return m_Index != other.m_Index;
}

inline bool CBaseHandle::operator ==(const CBaseHandle& other) const {
	return m_Index == other.m_Index;
}

inline bool CBaseHandle::operator ==(const i_handle_entity* e) const {
	return Get() == e;
}

inline bool CBaseHandle::operator !=(const i_handle_entity* e) const {
	return Get() != e;
}

inline bool CBaseHandle::operator <(const CBaseHandle& other) const {
	return m_Index < other.m_Index;
}

inline const CBaseHandle& CBaseHandle::operator=(const i_handle_entity* pEntity) {
	return Set(pEntity);
}


class i_handle_entity
{
public:
	virtual							~i_handle_entity() { }
	virtual void					SetRefEHandle(const std::uintptr_t& hRef) = 0;
	virtual const CBaseHandle& GetRefEHandle() const = 0;
};

inline const CBaseHandle& CBaseHandle::Set(const i_handle_entity* pEntity) {
	if (pEntity) {
		*this = pEntity->GetRefEHandle();
	}
	else {
		m_Index = INVALID_EHANDLE_INDEX;
	}

	return *this;
}


class IClientAlphaProperty;
class i_client_unknown : public i_handle_entity {
public:
    virtual collideable_t*              GetCollideable() = 0;
    virtual i_client_networkable*        GetClientNetworkable() = 0;
    virtual i_client_renderable*         GetClientRenderable() = 0;
    virtual i_client_entity*             GetIClientEntity() = 0;
    virtual entity_t*              GetBaseEntity() = 0;
    virtual i_client_thinkable*          GetClientThinkable() = 0;
    //virtual IClientModelRenderable*  GetClientModelRenderable() = 0;
    virtual IClientAlphaProperty*      GetClientAlphaProperty() = 0;
};
class i_client_networkable
{
public:
	virtual i_client_unknown* GetIClientUnknown() = 0;
	virtual void             Release() = 0;
	virtual client_class* GetClientClass() = 0;
	virtual void             NotifyShouldTransmit(int state) = 0;
	virtual void             OnPreDataChanged(int updateType) = 0;
	virtual void             OnDataChanged(int updateType) = 0;
	virtual void             PreDataUpdate(int updateType) = 0;
	virtual void             PostDataUpdate(int updateType) = 0;
	virtual void             __unkn(void) = 0;
	virtual bool             IsDormant(void) = 0;
	virtual int              EntIndex(void) const = 0;
	virtual void             ReceiveMessage(int classID, bf_read& msg) = 0;
	virtual void*			GetDataTableBasePtr() = 0;
	virtual void             SetDestroyedOnRecreateEntities(void) = 0;
};
typedef unsigned short ClientShadowHandle_t;
typedef unsigned short ClientRenderHandle_t;
class i_client_renderable {
public:
	virtual i_client_unknown* GetIClientUnknown() = 0;
	virtual vec3_t const& GetRenderOrigin(void) = 0;
	virtual vec3_t const& GetRenderAngles(void) = 0;
	virtual bool                       ShouldDraw(void) = 0;
	virtual int                        GetRenderFlags(void) = 0; // ERENDERFLAGS_xxx
	virtual void                       Unused(void) const {}
	virtual ClientShadowHandle_t       GetShadowHandle() const = 0;
	virtual ClientRenderHandle_t& RenderHandle() = 0;
	virtual const model_t* GetModel() const = 0;
	virtual int                        DrawModel(int flags, const int /*RenderableInstance_t*/& instance) = 0;
	virtual int                        GetBody() = 0;
	virtual void                       GetColorModulation(float* color) = 0;
	virtual bool                       LODTest() = 0;
	virtual bool                       SetupBones(matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) = 0;
	virtual void                       SetupWeights(const matrix3x4_t* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights) = 0;
	virtual void                       DoAnimationEvents(void) = 0;
	virtual void* GetPVSNotifyInterface() = 0;
	virtual void                       GetRenderBounds(vec3_t& mins, vec3_t& maxs) = 0;
	virtual void                       GetRenderBoundsWorldspace(vec3_t& mins, vec3_t& maxs) = 0;
	virtual void                       GetShadowRenderBounds(vec3_t& mins, vec3_t& maxs, int /*ShadowType_t*/ shadowType) = 0;
	virtual bool                       ShouldReceiveProjectedTextures(int flags) = 0;
	virtual bool                       GetShadowCastDistance(float* pDist, int /*ShadowType_t*/ shadowType) const = 0;
	virtual bool                       GetShadowCastDirection(vec3_t* pDirection, int /*ShadowType_t*/ shadowType) const = 0;
	virtual bool                       IsShadowDirty() = 0;
	virtual void                       MarkShadowDirty(bool bDirty) = 0;
	virtual i_client_renderable* GetShadowParent() = 0;
	virtual i_client_renderable* FirstShadowChild() = 0;
	virtual i_client_renderable* NextShadowPeer() = 0;
	virtual int /*ShadowType_t*/       ShadowCastType() = 0;
	virtual void                       CreateModelInstance() = 0;
	virtual model_instance_handle_t      GetModelInstance() = 0;
	virtual const matrix3x4_t& RenderableToWorldTransform() = 0;
	virtual int                        LookupAttachment(const char* pAttachmentName) = 0;
	virtual  bool                      GetAttachment(int number, vec3_t& origin, vec3_t& angles) = 0;
	virtual bool                       GetAttachment(int number, matrix3x4_t& matrix) = 0;
	virtual float* GetRenderClipPlane(void) = 0;
	virtual int                        GetSkin() = 0;
	virtual void                       OnThreadedDrawSetup() = 0;
	virtual bool                       UsesFlexDelayedWeights() = 0;
	virtual void                       RecordToolMessage() = 0;
	virtual bool                       ShouldDrawForSplitScreenUser(int nSlot) = 0;
	virtual uint8_t                    OverrideAlphaModulation(uint8_t nAlpha) = 0;
	virtual uint8_t                    OverrideShadowAlphaModulation(uint8_t nAlpha) = 0;
};
class CClientThinkHandlePtr;
typedef CClientThinkHandlePtr* ClientThinkHandle_t;
class i_client_thinkable
{
public:
	virtual i_client_unknown* GetIClientUnknown() = 0;
	virtual void                ClientThink() = 0;
	virtual ClientThinkHandle_t GetThinkHandle() = 0;
	virtual void                SetThinkHandle(ClientThinkHandle_t hThink) = 0;
	virtual void                Release() = 0;
};
class i_client_entity : public i_client_unknown, public i_client_renderable, public i_client_networkable, public i_client_thinkable
{
public:
	// Delete yourself.
	virtual void			Release(void) = 0;

	// Network origin + angles
	virtual const vec3_t& GetAbsOrigin(void) const = 0;
	virtual const vec3_t& GetAbsAngles(void) const = 0;
};

struct renderable_info_t
{
	i_client_renderable* pRenderable;		// 0x00
	char					pAlphaProperty[0x4];// 0x04
	int						iEnumCount;			// 0x08
	int						nRenderFrame;		// 0x0C
	unsigned short			uFirstShadow;		// 0x10
	unsigned short			uLeafList;			// 0x12
	short					sArea;				// 0x14
	std::uint16_t			uFlags;				// 0x16
	std::uint16_t			uFlags2;			// 0x18
	vec3_t					vecBloatedAbsMins;	// 0x1A
	vec3_t					vecBloatedAbsMaxs;	// 0x26
	vec3_t					vecAbsMins;			// 0x32
	vec3_t					vecAbsMaxs;			// 0x3E
	char				pad0[0x4];			// 0x4A
};

class entity_t : public i_client_entity {
public:

	datamap_t* GetDataDescMap()
	{
		using original_fn = datamap_t * (__thiscall*)(void*);
		return (*(original_fn**)this)[15](this);;
	}

	datamap_t* GetPredDescMap()
	{
		using original_fn = datamap_t * (__thiscall*)(void*);
		return (*(original_fn**)this)[17](this);;
	}
	void* animating() {
		return reinterpret_cast<void*>(uintptr_t(this) + 0x4);
	}
	void* networkable() {
		return reinterpret_cast<void*>(uintptr_t(this) + 0x8);
	}
	collideable_t* collideable() {
		using original_fn = collideable_t * (__thiscall*)(void*);
		return (*(original_fn * *)this)[3](this);
	}
	c_client_class* client_class() {
		using original_fn = c_client_class * (__thiscall*)(void*);
		return (*(original_fn * *)networkable())[2](networkable());
	}

	int index() {
		using original_fn = int(__thiscall*)(void*);
		return (*(original_fn * *)networkable())[10](networkable());
	}
	bool is_player() {
		using original_fn = bool(__thiscall*)(void*);
		return (*(original_fn * *)this)[158](this);
	}
	bool is_weapon() {
		using original_fn = bool(__thiscall*)(entity_t*);
		return (*(original_fn * *)this)[165](this);
	}
	bool setup_bones(matrix_t * out, int max_bones, int mask, float time) {
		if (!this)
			return false;

		using original_fn = bool(__thiscall*)(void*, matrix_t*, int, int, float);
		return (*(original_fn * *)animating())[13](animating(), out, max_bones, mask, time);
	}
	model_t* model() {
		using original_fn = model_t * (__thiscall*)(void*);
		return (*(original_fn * *)animating())[8](animating());
	}
	void update() {
		using original_fn = void(__thiscall*)(entity_t*);
		(*(original_fn * *)this)[218](this);
	}
	int draw_model(int flags, uint8_t alpha) {
		using original_fn = int(__thiscall*)(void*, int, uint8_t);
		return (*(original_fn * *)animating())[9](animating(), flags, alpha);
	}
	void set_angles(vec3_t angles) {
		using original_fn = void(__thiscall*)(void*, const vec3_t&);
		static original_fn set_angles_fn = (original_fn)((DWORD)utilities::pattern_scan("client.dll", "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1"));
		set_angles_fn(this, angles);
	}
	void set_position(vec3_t position) {
		using original_fn = void(__thiscall*)(void*, const vec3_t&);
		static original_fn set_position_fn = (original_fn)((DWORD)utilities::pattern_scan("client.dll", "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8"));
		set_position_fn(this, position);
	}

	void set_model_index(int index) {
		using original_fn = void(__thiscall*)(void*, int);
		return (*(original_fn * *)this)[75](this, index);
	}

	void net_pre_data_update(int update_type) {
		using original_fn = void(__thiscall*)(void*, int);
		return (*(original_fn * *)networkable())[6](networkable(), update_type);
	}

	void net_release() {
		using original_fn = void(__thiscall*)(void*);
		return (*(original_fn * *)networkable())[1](networkable());
	}

	int net_set_destroyed_on_recreate_entities() {
		using original_fn = int(__thiscall*)(void*);
		return (*(original_fn * *)networkable())[13](networkable());
	}

	bool dormant( ) {
		using original_fn = bool( __thiscall* )( void* );
		return ( *static_cast< original_fn** >( networkable( ) ) )[ 9 ]( networkable( ) );
	}

	
	NETVAR("DT_CSPlayer", "m_fFlags", flags, int)
	NETVAR("DT_BaseEntity", "m_hOwnerEntity", owner_handle, unsigned long)
	NETVAR("DT_CSPlayer", "m_flSimulationTime", simulation_time, float)
	NETVAR("DT_BasePlayer", "m_vecOrigin", origin, vec3_t)
	NETVAR("DT_BasePlayer", "m_vecViewOffset[0]", view_offset, vec3_t)
	NETVAR("DT_CSPlayer", "m_iTeamNum", team, int)
	NETVAR("DT_BaseEntity", "m_bSpotted", spotted, bool)
	NETVAR("DT_CSPlayer", "m_nSurvivalTeam", survival_team, int)
	NETVAR("DT_CSPlayer", "m_flHealthShotBoostExpirationTime", health_boost_time, float)
	NETVAR("CBasePlayer", "m_iHealth", health, int)
	NETVAR("DT_BaseGrenade", "m_vecVelocity", grenade_velocity, vec3_t)
	NETVAR("DT_BaseAnimating", "m_nHitboxSet", hitbox_set, int)
	NETVAR_OFFSET("DT_BaseCSGrenadeProjectile", "m_vecExplodeEffectOrigin", grenade_spawntime, float, 0xC)
	NETVAR("DT_BaseAnimating", "m_nSequence", sequence, int)

	matrix_t& GetCoordinateFrame()
	{
		static unsigned int m_GetCoordinateFrame = netvar_manager::find_in_datamap(GetDataDescMap(), "m_rgflCoordinateFrame");
		return *(matrix_t*)((uintptr_t)this + m_GetCoordinateFrame);
	}
	float& m_surfaceFriction()
	{
		static unsigned int m_surfaceFrictionFrame = netvar_manager::find_in_datamap(GetPredDescMap(), "m_surfaceFriction");
		return *(float*)((uintptr_t)this + m_surfaceFrictionFrame);
	}

	bool get_bounding_box(box_t& pBox) {
		collideable_t* collideable = this->collideable();
		if (collideable == nullptr)
			return false;

		vec3_t vecMin = collideable->mins();
		vec3_t vecMax = collideable->maxs();

		std::array<vec3_t, 8U> arrPoints =
		{
			vec3_t(vecMin.x, vecMin.y, vecMin.z),
			vec3_t(vecMin.x, vecMax.y, vecMin.z),
			vec3_t(vecMax.x, vecMax.y, vecMin.z),
			vec3_t(vecMax.x, vecMin.y, vecMin.z),
			vec3_t(vecMax.x, vecMax.y, vecMax.z),
			vec3_t(vecMin.x, vecMax.y, vecMax.z),
			vec3_t(vecMin.x, vecMin.y, vecMax.z),
			vec3_t(vecMax.x, vecMin.y, vecMax.z)
		};

		if (arrPoints.data() == nullptr)
			return false;

		matrix_t matTransformed = this->GetCoordinateFrame();

		float flLeft = 3.40282e+38f;
		float flTop = 3.40282e+38f;
		float flRight = std::numeric_limits<float>::lowest();
		float flBottom = std::numeric_limits<float>::lowest();

		std::array<vec2_t, 8U> arrScreen = { };
		for (std::size_t i = 0U; i < 8U; i++)
		{
			vec3_t temp;
			math::transform_vector(arrPoints.at(i), matTransformed, temp);
			if (!math::world_to_screen(temp, arrScreen.at(i)))
				return false;

			flLeft = min(flLeft, arrScreen.at(i).x);
			flTop = min(flTop, arrScreen.at(i).y);
			flRight = max(flRight, arrScreen.at(i).x);
			flBottom = max(flBottom, arrScreen.at(i).y);
		}

		pBox.left = flLeft;
		pBox.top = flTop;
		pBox.right = flRight;
		pBox.bottom = flBottom;
		pBox.width = flRight - flLeft;
		pBox.height = flBottom - flTop;
		return true;
	}
	inline bool breakable() {
		/*
		const int iHealth = this->health();

		// first check to see if it's already broken
		if (iHealth < 0 && this->IsMaxHealth() > 0)
			return true;

		if (this->GetTakeDamage() != DAMAGE_YES)
		{
			const EClassIndex nClassIndex = this->GetClientClass()->nClassID;

			// force pass cfuncbrush
			if (nClassIndex != EClassIndex::CFuncBrush)
				return false;
		}

		if (const int nCollisionGroup = this->GetCollisionGroup(); nCollisionGroup != COLLISION_GROUP_PUSHAWAY && nCollisionGroup != COLLISION_GROUP_BREAKABLE_GLASS && nCollisionGroup != COLLISION_GROUP_NONE)
			return false;

		if (iHealth > 200)
			return false;

		if (IMultiplayerPhysics* pPhysicsInterface = dynamic_cast<IMultiplayerPhysics*>(this); pPhysicsInterface != nullptr)
		{
			if (pPhysicsInterface->GetMultiplayerPhysicsMode() != PHYSICS_MULTIPLAYER_SOLID)
				return false;
		}
		else
		{
			if (const char* szClassName = this->GetClassname(); !strcmp(szClassName, XorStr("func_breakable")) || !strcmp(szClassName, XorStr("func_breakable_surf")))
			{
				if (!strcmp(szClassName, XorStr("func_breakable_surf")))
				{
					CBreakableSurface* pSurface = static_cast<CBreakableSurface*>(this);

					// don't try to break it if it has already been broken
					if (pSurface->IsBroken())
						return false;
				}
			}
			else if (this->PhysicsSolidMaskForEntity() & CONTENTS_PLAYERCLIP)
			{
				// hostages and players use CONTENTS_PLAYERCLIP, so we can use it to ignore them
				return false;
			}
		}

		if (IBreakableWithPropData* pBreakableInterface = dynamic_cast<IBreakableWithPropData*>(this); pBreakableInterface != nullptr)
		{
			// bullets don't damage it - ignore
			if (pBreakableInterface->GetDmgModBullet() <= 0.0f)
				return false;
		}
		
		return true;
		*/
		return false;
	}
	
};

class econ_view_item_t {
public:
	NETVAR("DT_ScriptCreatedItem", "m_bInitialized", is_initialized, bool)
	NETVAR("DT_ScriptCreatedItem", "m_iEntityLevel", entity_level, int)
	NETVAR("DT_ScriptCreatedItem", "m_iAccountID", account_id, int)
	NETVAR("DT_ScriptCreatedItem", "m_iItemIDLow", item_id_low, int)
};

class base_view_model_t : public entity_t {
public:
	NETVAR("DT_BaseViewModel", "m_nModelIndex", model_index, int)
	NETVAR("DT_BaseViewModel", "m_nViewModelIndex", view_model_index, int)
	NETVAR("DT_BaseViewModel", "m_hWeapon", weapon, int)
	NETVAR("DT_BaseViewModel", "m_hOwner", owner, int)
	NETVAR("DT_BaseViewModel", "m_nAnimationParity", m_nAnimationParity, int)

	float& cycle()
	{
		static unsigned int m_GetCoordinateFrame = netvar_manager::find_in_datamap(GetDataDescMap(), "m_flCycle");
		return *(float*)((uintptr_t)this + m_GetCoordinateFrame);
	}
	float& animtime()
	{
		static unsigned int m_surfaceFrictionFrame = netvar_manager::find_in_datamap(GetPredDescMap(), "m_flAnimTime");
		return *(float*)((uintptr_t)this + m_surfaceFrictionFrame);
	}
};

class weapon_t : public entity_t {
public:
	NETVAR("DT_BaseCombatWeapon", "m_flNextPrimaryAttack", next_primary_attack, float)
	NETVAR("DT_BaseCombatWeapon", "m_flNextSecondaryAttack", next_secondary_attack, float)
	NETVAR("DT_BaseCombatWeapon", "m_iClip1", clip1_count, int)
	NETVAR("DT_BaseCombatWeapon", "m_iClip2", clip2_count, int)
	NETVAR("DT_BaseCombatWeapon", "m_iPrimaryReserveAmmoCount", primary_reserve_ammo_acount, int)
	NETVAR("DT_WeaponCSBase", "m_flRecoilIndex", recoil_index, float)
	NETVAR("DT_WeaponCSBase", "m_bBurstMode", burst_mode, bool)
	NETVAR("DT_WeaponCSBase", "m_iBurstShotsRemaining", burst_shot_remaining, int)
	NETVAR("DT_WeaponCSBase", "m_flPostponeFireReadyTime", ready_fire_time, float)
	NETVAR("DT_WeaponCSBaseGun", "m_zoomLevel", zoom_level, float)
	NETVAR("DT_BaseAttributableItem", "m_iItemDefinitionIndex", item_definition_index, short)
	NETVAR("DT_BaseCombatWeapon", "m_iEntityQuality", entity_quality, int)
	NETVAR("DT_BaseCombatWeapon", "m_hWeaponWorldModel", weapon_world_model, int)
	NETVAR("DT_BaseCombatWeapon", "m_flCycle", cycle, float)
	NETVAR("DT_BaseCombatWeapon", "m_iItemIDHigh", item_id_high, int)
	NETVAR("DT_BaseCSGrenadeProjectile", "m_hThrower", thrower, uintptr_t)
	NETVAR("DT_BaseCSGrenade", "m_flThrowStrength", throwstrength, float)
	NETVAR("DT_BaseCSGrenade", "m_fThrowTime", throwtime, float)
	NETVAR("DT_BaseCSGrenade", "m_bPinPulled", pin_pulled, bool)
	NETVAR("DT_BaseCSGrenadeProjectile", "m_nExplodeEffectTickBegin", explodeeffecttickbegin, int)
	NETVAR("DT_WeaponCSBase", "m_fAccuracyPenalty", accuracy_penalty, float)

	float inaccuracy() {
		using original_fn = float(__thiscall*)(void*);
		return (*(original_fn * *)this)[483](this);
	}

	float get_spread() {
		using original_fn = float(__thiscall*)(void*);
		return (*(original_fn * *)this)[453](this);
	}

	void update_accuracy_penalty() {
		using original_fn = void(__thiscall*)(void*);
		(*(original_fn * *)this)[483](this);
	}

	weapon_info_t* get_weapon_data() {
		return interfaces::weapon_system->get_weapon_data(this->item_definition_index());
	}
};
class VarMapEntry_t
{
public:
	unsigned short type;
	unsigned short m_bNeedsToInterpolate;
	void* data;
	void* watcher;
};

struct VarMapping_t
{
	utl_vector<VarMapEntry_t> m_Entries;
	int m_nInterpolatedEntries;
	float m_lastInterpolationTime;
};
class player_t : public entity_t {
private:
	template <typename T>
	T& read(uintptr_t offset) {
		return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	template <typename T>
	void write(uintptr_t offset, T data) {
		*reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + offset) = data;
	}

public:
	NETVAR("DT_BasePlayer", "m_hViewModel[0]", view_model, uintptr_t)
	NETVAR("DT_CSPlayer", "m_bHasDefuser", has_defuser, bool)
	NETVAR("DT_CSPlayer", "m_bGunGameImmunity", has_gun_game_immunity, bool)
	NETVAR("DT_CSPlayer", "m_iShotsFired", shots_fired, int)
	NETVAR("DT_CSPlayer", "m_angEyeAngles", eye_angles, vec3_t)
	NETVAR("DT_CSPlayer", "m_ArmorValue", armor, int)
	NETVAR("DT_CSPlayer", "m_bHasHelmet", has_helmet, bool)
	NETVAR("DT_CSPlayer", "m_bIsScoped", is_scoped, bool)
	NETVAR("DT_CSPlayer", "m_bIsDefusing", is_defusing, bool)
	NETVAR("DT_CSPlayer", "m_iAccount", money, int)
	NETVAR("DT_CSPlayer", "m_flLowerBodyYawTarget", lower_body_yaw, float)
	NETVAR("DT_CSPlayer", "m_flNextAttack", next_attack, float)
	NETVAR("DT_CSPlayer", "m_flFlashDuration", flash_duration, float)
	NETVAR("DT_CSPlayer", "m_flFlashMaxAlpha", flash_alpha, float)
	NETVAR("DT_CSPlayer", "m_bHasNightVision", has_night_vision, bool)
	NETVAR("DT_CSPlayer", "m_bNightVisionOn", night_vision_on, bool)
	NETVAR("DT_CSPlayer", "m_iHealth", health, int)
	NETVAR("DT_CSPlayer", "m_lifeState", life_state, int)
	NETVAR("DT_CSPlayer", "m_fFlags", flags, int)
	NETVAR_PTR("DT_CSPlayer", "m_szArmsModel", arm_model, char)
	NETVAR("DT_BasePlayer", "m_viewPunchAngle", punch_angle, vec3_t)
	NETVAR("DT_BasePlayer", "m_aimPunchAngle", aim_punch_angle, vec3_t)
	NETVAR("DT_BasePlayer", "m_aimPunchAngleVel", aim_punch_angle_vel, vec3_t)
	NETVAR("DT_BasePlayer", "m_vecVelocity[0]", velocity, vec3_t)
	NETVAR("DT_BasePlayer", "m_vecMins", vecMins, vec3_t)
	NETVAR("DT_BasePlayer", "m_vecMaxs", vecMaxs, vec3_t)
	NETVAR("DT_BasePlayer", "m_flMaxspeed", max_speed, float)
	NETVAR("DT_BasePlayer", "m_flFallVelocity", fall_velocity, float)
	NETVAR("DT_BaseEntity", "m_flShadowCastDistance", fov_time, float)
	NETVAR("DT_BasePlayer", "m_hObserverTarget", observer_target, unsigned long)
	NETVAR_PTR("DT_CSPlayer", "m_iObserverMode", observer_mode, int);
	NETVAR("DT_BasePlayer", "m_nHitboxSet", hitbox_set, int)
	NETVAR("DT_CSPlayer", "m_flDuckAmount", duck_amount, float)
	NETVAR("DT_CSPlayer", "m_flDuckSpeed", duck_speed, float)
	NETVAR("DT_CSPlayer", "m_bHasHeavyArmor", has_heavy_armor, bool)
	NETVAR("DT_SmokeGrenadeProjectile", "m_nSmokeEffectTickBegin", smoke_grenade_tick_begin, int)
	NETVAR("DT_CSPlayer", "m_nTickBase", get_tick_base, int)
	NETVAR("DT_BasePlayer", "deadflag", view_angles, vec3_t, 0x4)
	OFFSET(bool, is_jiggle_bones_enabled, 0x2930)
	NETVAR("DT_BasePlayer", "m_fEffects", get_effects, int)
	NETVAR("DT_BasePlayer", "m_flVelocityModifier", velocitymodifier, float)
	NETVAR_PTR("DT_CSPlayer", "m_flPoseParameter", get_pose_parameter, float)
	OFFSET(bool, use_new_animstate, 0x9B14)
	NETVAR_OFFSET("DT_CSPlayer", "m_bIsScoped", get_animation_state, CCSGOPlayerAnimState*, -0x14)

	vec3_t& base_velocity()
	{
		static unsigned int m_surfaceFrictionFrame = netvar_manager::find_in_datamap(GetPredDescMap(), "m_vecBaseVelocity");
		return *(vec3_t*)((uintptr_t)this + m_surfaceFrictionFrame);
	}

	bool IsArmored(const int iHitGroup)
	{
		// @ida isarmored: server.dll @ 55 8B EC 32 D2

		bool bIsArmored = false;

		if (this->armor() > 0)
		{
			switch (iHitGroup)
			{
			case hitgroup_generic:
			case hitgroup_chest:
			case hitgroup_stomach:
			case hitgroup_leftarm:
			case hitgroup_rightarm:
			case hitgroup_neck:
				bIsArmored = true;
				break;
			case hitgroup_head:
				if (this->has_helmet())
					bIsArmored = true;
				[[fallthrough]];
			case hitgroup_leftleg:
			case hitgroup_rightleg:
				if (this->has_heavy_armor())
					bIsArmored = true;
				break;
			default:
				break;
			}
		}

		return bIsArmored;
	}
	//bool& use_new_animstate() {BAD
	//	static const std::uintptr_t uUseNewAnimationOffset = *reinterpret_cast<std::uintptr_t*>(utilities::pattern_scan("client.dll", "88 87 ? ? ? ? 75") + 0x2);
	//	return *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + uUseNewAnimationOffset);
	//}
	weapon_t* active_weapon() {
		auto active_weapon = read<uintptr_t>(netvar_manager::get_net_var(fnv::hash("DT_CSPlayer"), fnv::hash("m_hActiveWeapon"))) & 0xFFF;
		return reinterpret_cast<weapon_t*>(interfaces::entity_list->get_client_entity(active_weapon));
	}

	UINT* get_wearables() {
		return (UINT*)((uintptr_t)this + (netvar_manager::get_net_var(fnv::hash("DT_CSPlayer"), fnv::hash("m_hMyWearables"))));
	}

	UINT* get_weapons() {
		return (UINT*)((uintptr_t)this + (netvar_manager::get_net_var(fnv::hash("DT_CSPlayer"), fnv::hash("m_hMyWeapons"))));
	}

	vec3_t get_eye_pos() {
		return origin() + view_offset();
	}

	anim_state* get_anim_state() {
		return *reinterpret_cast<anim_state * *>(this + 0x3914);
	}

	animation_layer* get_animation_overlay()
	{
		return *(animation_layer**)((DWORD)this + 0x2990);
	}

	bool can_see_player_pos(player_t * player, const vec3_t & pos) {
		trace_t tr;
		ray_t ray;
		trace_filter filter;
		filter.skip = this;

		auto start = get_eye_pos();
		auto dir = (pos - start).normalized();

		ray.initialize(start, pos);
		interfaces::trace_ray->trace_ray(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

		return tr.entity == player || tr.flFraction > 0.97f;
	}

	vec3_t get_bone_position(int bone) {
		matrix_t bone_matrices[128];
		if (setup_bones(bone_matrices, 128, 256, 0.0f))
			return vec3_t{ bone_matrices[bone][0][3], bone_matrices[bone][1][3], bone_matrices[bone][2][3] };
		else
			return vec3_t{ };
	}

	vec3_t get_hitbox_position(int hitbox_id) {
		matrix_t bone_matrix[MAXSTUDIOBONES];

		if (setup_bones(bone_matrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
			auto studio_model = interfaces::model_info->get_studio_model(model());

			if (studio_model) {
				auto hitbox = studio_model->hitbox_set(0)->hitbox(hitbox_id);

				if (hitbox) {
					auto min = vec3_t{}, max = vec3_t{};

					math::transform_vector(hitbox->mins, bone_matrix[hitbox->bone], min);
					math::transform_vector(hitbox->maxs, bone_matrix[hitbox->bone], max);

					return vec3_t((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f);
				}
			}
		}
		return vec3_t{};
	}

	bool is_alive() {
		if ( !this ) return false;
		return this->health() > 0;
	}

	bool is_moving() {
		if ( !this ) return false;
		return this->velocity().length() > 0.1f;
	}

	bool is_in_air() {
		if ( !this ) return false;
		return !( this->flags() & fl_onground );
	}

	bool is_flashed() {
		if ( !this ) return false;
		return this->flash_duration() > 0.0f;
	}

	void update_client_side_animations() {
		using original_fn = void(__thiscall*)(void*);
		(*(original_fn * *)this)[224](this);
	}

	void set_model_index(int index) {
		using original_fn = void(__thiscall*)(void*, int);
		(*(original_fn**)this)[75](this, index);
	}

	c_baseplayeranimationstate* get_animation_stateb()
	{
		return *reinterpret_cast<c_baseplayeranimationstate**>(reinterpret_cast<void*>(uintptr_t(this) + 0x9960));
	}

	void set_abs_origin(const vec3_t& vecOrigin)
	{
		using SetAbsOriginFn = void(__thiscall*)(void*, const vec3_t&);
		static auto oSetAbsOrigin = reinterpret_cast<SetAbsOriginFn>(utilities::pattern_scan("client.dll", "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8"));
		oSetAbsOrigin(this, vecOrigin);
	}
	void set_abs_angles(vec3_t& ang) {
		using SetAbsAngleFn = void(__thiscall*)(void*, const vec3_t&); 
		static auto oSetAbsAngles = reinterpret_cast<SetAbsAngleFn>(utilities::pattern_scan("client.dll", "55 8B EC 83 E4 F8 83 EC 64 53"));
		oSetAbsAngles(this, ang);
	}

	vec3_t& abs_origin() {
		using original_fn = vec3_t & (__thiscall*)(void*);
		return (*(original_fn * *)this)[10](this);;
	}
	vec3_t& abs_angles() {
		using original_fn = vec3_t & (__thiscall*)(void*);
		return (*(original_fn * *)this)[11](this);;
	}

	int move_type() {
		static int type = netvar_manager::get_net_var(fnv::hash("DT_BaseEntity"), fnv::hash("m_nRenderMode")) + 1;
		return read<int>(type);
	}

	bool can_shoot()
	{
		weapon_t* weapon = this->active_weapon();
		if (weapon == nullptr) return false;
		const float flServerTime = (this->get_tick_base() * interfaces::globals->interval_per_tick);
		
		// check is have ammo
		if (weapon->clip1_count() <= 0)
			return false;

		// is player ready to shoot
		if (this->next_attack() > flServerTime)
			return false;

		const short nDefinitionIndex = weapon->item_definition_index();

		// check is weapon with burst mode
		if ((nDefinitionIndex == WEAPON_FAMAS || nDefinitionIndex == WEAPON_GLOCK) &&
			// check is burst mode
			weapon->burst_mode() && weapon->burst_shot_remaining() > 0)
			return true;

		// is weapon ready to shoot
		if (weapon->next_primary_attack() > flServerTime)
			return false;

		// check for revolver cocking ready
		if (nDefinitionIndex == WEAPON_REVOLVER && weapon->ready_fire_time() > flServerTime)
			return false;

		return true;
	}

	void get_animation_layers(animation_layer* layers) {

		std::memcpy(layers, get_animation_overlay(), sizeof(animation_layer) * 13);

	}

	void set_animation_layers(animation_layer* layers) {

		std::memcpy(get_animation_overlay(), layers, sizeof(animation_layer) * 13);

	}

	void get_pose_parameters(float* poses) {

		std::memcpy(poses, get_pose_parameter(), sizeof(float) * 24);

	}

	void set_pose_parameters(float* poses) {

		std::memcpy(get_pose_parameter(), poses, sizeof(float) * 24);

	}
	float max_desync_delta() {
		c_baseplayeranimationstate* animstate = this->get_animation_stateb();

		float flDuckAmount = animstate->m_flAnimDuckAmount;
		float flRunningSpeed = std::clamp(animstate->m_flSpeedAsPortionOfWalkTopSpeed, 0.0f, 1.0f);
		float flDuckingSpeed = std::clamp(animstate->m_flSpeedAsPortionOfCrouchTopSpeed, 0.0f, 1.0f);
		float flYawModifier = (((animstate->m_flWalkToRunTransition * -0.3f) - 0.2f) * flRunningSpeed) + 1.0f;

		if (flDuckAmount > 0.0f)
			flYawModifier += ((flDuckAmount * flDuckingSpeed) * (0.5f - flYawModifier));

		float flMaxYawModifier = flYawModifier * animstate->m_flAimYawMax;
		return flMaxYawModifier;
	}
	
	utl_vector <matrix_t>& CachedBoneData() {
		//return *(utl_vector <matrix3x4_t>*)(((uintptr_t)this) + (*(DWORD*)(utilities::pattern_scan("client.dll", "FF B7 ?? ?? ?? ?? 52")) + 0x2) + 0x4);
		return *(utl_vector<matrix_t>*)(uintptr_t(this) + 0x2914);
	}

	void disable_interpolation() {
		VarMapping_t* map = (VarMapping_t*)((uintptr_t)this + 36);

		for (int i = 0; i < map->m_nInterpolatedEntries; i++)
		{
			VarMapEntry_t* e = &map->m_Entries[i];
			e->m_bNeedsToInterpolate = false;
		}
	}
};

class plantedc4_t : public entity_t {
public:
	NETVAR("DT_PlantedC4", "m_flC4Blow", blow_time, float)
	NETVAR("DT_PlantedC4", "m_flTimerLength", timer_length, float)
	NETVAR("DT_PlantedC4", "m_flDefuseLength", defuse_length, float)
	NETVAR("DT_PlantedC4", "m_flDefuseCountDown", defuse_count_down, float)
	NETVAR("DT_PlantedC4", "m_bBombTicking", ticking, bool)
	NETVAR("DT_PlantedC4", "m_hBombDefuser", defuser, uintptr_t)
	NETVAR("DT_PlantedC4", "m_bBombDefused", defused, bool)

	float timer(const float server_time) {
		return std::clamp(blow_time() - server_time, 0.0f, timer_length());
	}

	float defuse_timer(const float server_time) {
		return std::clamp(defuse_count_down() - server_time, 0.0f, defuse_length());
	}
};

class smoke_grenade_t : public entity_t {
public: 
	NETVAR("DT_SmokeGrenadeProjectile", "m_nSmokeEffectTickBegin", effect_tick_begin, int)
		float max_time()
	{
		return 18.f;
	}
};

class inferno_t : public entity_t {
public:
	NETVAR("DT_Inferno", "m_nFireEffectTickBegin", effect_tick_begin, int)
		float max_time()
	{
		return 7.f;
	}
};