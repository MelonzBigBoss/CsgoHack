#pragma once

#pragma region renderbeams_definitions
#define	TE_BEAMPOINTS			0	// beam effect between two points
#define TE_SPRITE				1	// additive sprite, plays 1 cycle
#define TE_BEAMDISK				2	// disk that expands to max radius over lifetime
#define TE_BEAMCYLINDER			3	// cylinder that expands to max radius over lifetime
#define TE_BEAMFOLLOW			4	// create a line of decaying beam segments until entity stops moving
#define TE_BEAMRING				5	// connect a beam ring to two entities
#define TE_BEAMSPLINE			6	
#define TE_BEAMRINGPOINT		7	
#define	TE_BEAMLASER			8	// fades according to viewpoint
#define TE_BEAMTESLA			9
#define MAX_BEAM_ENTS			10
#define NOISE_DIVISIONS			128
#pragma endregion
class entity_t;
enum EBeamType : unsigned int
{
	FBEAM_STARTENTITY = 0x00000001,
	FBEAM_ENDENTITY = 0x00000002,
	FBEAM_FADEIN = 0x00000004,
	FBEAM_FADEOUT = 0x00000008,
	FBEAM_SINENOISE = 0x00000010,
	FBEAM_SOLID = 0x00000020,
	FBEAM_SHADEIN = 0x00000040,
	FBEAM_SHADEOUT = 0x00000080,
	FBEAM_ONLYNOISEONCE = 0x00000100,	// only calculate our noise once
	FBEAM_NOTILE = 0x00000200,
	FBEAM_USE_HITBOXES = 0x00000400,	// attachment indices represent hitbox indices instead when this is set.
	FBEAM_STARTVISIBLE = 0x00000800,	// has this client actually seen this beam's start entity yet?
	FBEAM_ENDVISIBLE = 0x00001000,	// has this client actually seen this beam's end entity yet?
	FBEAM_ISACTIVE = 0x00002000,
	FBEAM_FOREVER = 0x00004000,
	FBEAM_HALOBEAM = 0x00008000,	// when drawing a beam with a halo, don't ignore the segments and endwidth
	FBEAM_REVERSED = 0x00010000,
};

struct beam_trail_t
{
	beam_trail_t* pNext;
	float			flDie;
	vec3_t			vecOrigin;
	vec3_t			vecVelocity;
};

struct Beam_t
{
	Beam_t() = default;

	// Methods of IClientRenderable
	virtual const vec3_t& GetRenderOrigin() = 0;
	virtual const vec3_t& GetRenderAngles() = 0;
	virtual const void* RenderableToWorldTransform() = 0;
	virtual void			GetRenderBounds(vec3_t& vecMins, vec3_t& vecMaxs) = 0;
	virtual bool			ShouldDraw() = 0;
	virtual bool			IsTransparent() = 0;
	virtual int				DrawModel(int nFlags) = 0;
	virtual void			ComputeFxBlend() = 0;
	virtual int				GetFxBlend() = 0;

	vec3_t			vecMins;
	vec3_t			vecMaxs;
	int* pQueryHandleHalo;
	float			flHaloProxySize;
	Beam_t* pNext;
	int				nType;
	int				nFlags;

	// Control points for the beam
	int				nAttachments;
	vec3_t			vecAttachment[MAX_BEAM_ENTS];
	vec3_t			vecDelta;

	// 0 .. 1 over lifetime of beam
	float			flTime;
	float			flFrequence;

	// Time when beam should die
	float			flDie;
	float			flWidth;
	float			flEndWidth;
	float			flFadeLength;
	float			flAmplitude;
	float			flLife;

	// Color
	float			r, g, b;
	float			flBrightness;

	// Speed
	float			flSpeed;

	// Animation
	float			flFrameRate;
	float			flFrame;
	int				nSegments;

	// Attachment entities for the beam
	uintptr_t		hEntity[MAX_BEAM_ENTS];
	int				nAttachmentIndex[MAX_BEAM_ENTS];

	// Model info
	int				nModelIndex;
	int				nHaloIndex;
	float			flHaloScale;
	int				iFrameCount;
	float			flRgNoise[NOISE_DIVISIONS + 1];

	// Popcorn trail for beam follows to use
	beam_trail_t* pTrail;

	// for TE_BEAMRINGPOINT
	float			flStartRadius;
	float			flEndRadius;

	// for FBEAM_ONLYNOISEONCE
	bool			bCalculatedNoise;
	float			flHDRColorScale;
};

struct beam_info_t
{
	beam_info_t()
	{
		nType = TE_BEAMPOINTS;
		nSegments = -1;
		pszModelName = nullptr;
		pszHaloName = nullptr;
		nModelIndex = -1;
		nHaloIndex = -1;
		bRenderable = true;
		nFlags = 0;
	}

	int				nType;

	// Entities
	entity_t* pStartEntity;
	int				iStartAttachment;
	entity_t* pEndEntity;
	int				iEndAttachment;

	// Points
	vec3_t			vecStart;
	vec3_t			vecEnd;

	int				nModelIndex;
	const char* pszModelName;
	int				nHaloIndex;
	const char* pszHaloName;
	float			flHaloScale;
	float			flLife;
	float			flWidth;
	float			flEndWidth;
	float			flFadeLength;
	float			flAmplitude;
	float			flBrightness;
	float			flSpeed;
	int				iStartFrame;
	float			flFrameRate;
	float			flRed;
	float			flGreen;
	float			flBlue;
	bool			bRenderable;
	int				nSegments;
	int				nFlags;

	// Rings
	vec3_t			vecCenter;
	float			flStartRadius;
	float			flEndRadius;
};

class CBeam;
class i_view_render_beam
{
public:
	virtual void	InitBeams() = 0;
	virtual void	ShutdownBeams() = 0;
	virtual void	ClearBeams() = 0;
	virtual void	UpdateTempEntBeams() = 0;

	virtual void	DrawBeam(CBeam* pBeam, const void* instance, void* pEntityBeamTraceFilter = nullptr) = 0;
	virtual void	DrawBeam(Beam_t* pBeam) = 0;

	virtual void	KillDeadBeams(entity_t* pEntity) = 0;

	virtual Beam_t* CreateBeamEnts(beam_info_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamEntPoint(beam_info_t& beamInfo) = 0;
	virtual	Beam_t* CreateBeamPoints(beam_info_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamRing(beam_info_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamRingPoint(beam_info_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamCirclePoints(beam_info_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamFollow(beam_info_t& beamInfo) = 0;

	virtual void	FreeBeam(Beam_t* pBeam) = 0;
	virtual void	UpdateBeamInfo(Beam_t* pBeam, beam_info_t& beamInfo) = 0;

	virtual void	CreateBeamEnts(int iStartEntity, int iEndEntity, int nModelIndex, int iHaloIndex, float flHaloScale,
		float flLife, float flWidth, float flEndWidth, float flFadeLength, float flAmplitude,
		float flBrightness, float flSpeed, int flStartFrame,
		float flFrameRate, float r, float g, float b, int iType = -1) = 0;
	virtual void	CreateBeamEntPoint(int iStartEntity, const vec3_t* pStart, int iEndEntity, const vec3_t* pEnd,
		int nModelIndex, int iHaloIndex, float flHaloScale,
		float flLife, float flWidth, float flEndWidth, float flFadeLength, float flAmplitude,
		float flBrightness, float flSpeed, int iStartFrame,
		float flFrameRate, float r, float g, float b) = 0;
	virtual void	CreateBeamPoints(vec3_t& vecStart, vec3_t& vecEnd, int nModelIndex, int iHaloIndex, float flHaloScale,
		float flLife, float flWidth, float flEndWidth, float flFadeLength, float flAmplitude,
		float flBrightness, float flSpeed, int iStartFrame,
		float flFrameRate, float r, float g, float b) = 0;
	virtual void	CreateBeamRing(int iStartEntity, int iEndEntity, int nModelIndex, int iHaloIndex, float flHaloScale,
		float flLife, float flWidth, float flEndWidth, float flFadeLength, float flAmplitude,
		float flBrightness, float flSpeed, int iStartFrame,
		float flFrameRate, float r, float g, float b, int iFlags = 0) = 0;
	virtual void	CreateBeamRingPoint(const vec3_t& vecCenter, float flStartRadius, float flEndRadius, int nModelIndex, int iHaloIndex, float flHaloScale,
		float flLife, float flWidth, float flEndWidth, float flFadeLength, float flAmplitude,
		float flBrightness, float flSpeed, int iStartFrame,
		float flFrameRate, float r, float g, float b, int iFlags = 0) = 0;
	virtual void	CreateBeamCirclePoints(int iType, vec3_t& vecStart, vec3_t& vecEnd,
		int nModelIndex, int iHaloIndex, float flHaloScale, float flLife, float flWidth,
		float flEndWidth, float flFadeLength, float flAmplitude, float flBrightness, float flSpeed,
		int iStartFrame, float flFrameRate, float r, float g, float b) = 0;
	virtual void	CreateBeamFollow(int startEnt, int nModelIndex, int iHaloIndex, float flHaloScale,
		float flLife, float flWidth, float flEndWidth, float flFadeLength, float r, float g, float b,
		float flBrightness) = 0;
};
