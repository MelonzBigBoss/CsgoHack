#pragma once
class anim_state {
public:
	char	u0[ 0x18 ];
	float	anim_update_timer;
	char	u1[ 0xC ];
	float	started_moving_time;
	float	last_move_time;
	char	u2[ 0x10 ];
	float	last_lby_time;
	char	u3[ 0x8 ];
	float	run_amount;
	char	u4[ 0x10 ];
	void	*entity;
	__int32 active_weapon;
	__int32 last_active_weapon;
	float	last_clientside_anim_update_time;
	__int32 last_clientside_anim_update_framecount;
	float	eye_timer;
	float	eye_angles_y;
	float	eye_angles_x;
	float	goal_feet_yaw;
	float	current_feet_yaw;
	float	torso_yaw;
	float	last_move_yaw;
	float	lean_amount;
	char	u5[ 0x4 ];
	float	feet_cycle;
	float	feet_yaw_rate;
	char	u6[ 0x4 ];
	float	duck_amount;
	float	landing_duck_amount;
	char	u7[ 0x4 ];
	vec3_t	current_origin;
	vec3_t	last_origin;
	float	velocity_x;
	float	velocity_y;
	char	u8[ 0x10 ];
	float	move_direction_1;
	float	move_direction_2;
	char	u9[ 0x4 ];
	float	velocity;
	float	jump_fall_velocity; 
	float	clamped_velocity; 
	float	feet_speed_forwards_or_sideways;
	float	feet_speed_unknown_forwards_or_sideways; 
	float	last_time_started_moving;
	float	last_time_stopped_moving;
	bool	on_ground;
	bool	hit_in_ground_anim;
	char	u10[ 0x4 ];
	float	last_origin_z;
	float	head_from_ground_distance_standing;
	float	stop_to_full_running_fraction;
	char	u11[ 0x14 ];
	__int32 is_not_moving;
	char	u12[ 0x20 ];
	float	last_anim_update_time;
	float	moving_direction_x;
	float	moving_direction_y;
	float	moving_direction_z;
	char	u13[ 0x44 ];
	__int32 started_moving;
	char	u14[ 0x8 ];
	float	lean_yaw;
	char	u15[ 0x8 ];
	float	poses_speed;
	char	u16[ 0x8 ];
	float	ladder_speed;
	char	u17[ 0x8 ];
	float	ladder_yaw;
	char	u18[ 0x8 ];
	float	some_pose;
	char	u19[ 0x14 ];
	float	body_yaw;
	char	u20[ 0x8 ];
	float	body_pitch;
	char	u21[ 0x8 ];
	float	death_yaw;
	char	u22[ 0x8 ];
	float	stand;
	char	u23[ 0x8 ];
	float	jump_fall;
	char	u24[ 0x8 ];
	float	aim_blend_stand_idle;
	char	u25[ 0x8 ];
	float	aim_blend_crouch_idle;
	char	u26[ 0x8 ];
	float	strafe_yaw;
	char	u27[ 0x8 ];
	float	aim_blend_stand_walk;
	char	u28[ 0x8 ];
	float	aim_blend_stand_run;
	char	u29[ 0x8 ];
	float	aim_blend_crouch_walk;
	char	u30[ 0x8 ];
	float	move_blend_walk;
	char	u31[ 0x8 ];
	float	move_blend_run;
	char	u32[ 0x8 ];
	float	move_blend_crouch;
	char	u33[ 0x4 ];
	float	speed;
	__int32 moving_in_any_direction;
	float	acceleration;
	char	u34[ 0x74 ];
	float	crouch_height;
	__int32 is_full_crouched;
	char	u35[ 0x4 ];
	float	velocity_subtract_x; 
	float	velocity_subtract_y; 
	float	velocity_subtract_z; 
	float	standing_head_height;
};
class entity_t;
class weapon_t;
class CCSGOPlayerAnimState
{
public:
	void set_layer_sequence(void* animlayer, int activity);
	int select_sequence_from_activity_modifier(int activity);
	void increment_layer_cycle(void* Layer, bool is_loop);
	bool is_layer_sequence_finished(void* layer, float time);
	void set_layer_cycle(void* animlayer, float cycle);
	void set_layer_rate(void* animlayer, float rate);
	void set_layer_weight(void* animlayer, float weight);

	void*					m_pThis; // 0x0
	bool					m_bIsReset; // 0x2
	bool					m_bUnknownClientBoolean; // 0x3B // am i retarded or why is this thing skips so many number.
	char					m_aSomePad[2]; // 0x3F
	int32_t					m_nTick; // 0x45
	float				    m_flFlashedStartTime; // 0x49
	float					m_flFlashedEndTime; // 0x4D
	char    				m_AimLayers[0x3C]; // 0x51
	int32_t					m_iModelIndex; // 0x55
	int32_t					m_iUnknownArray[3]; // 0x59
	player_t*				m_player; // 0x60
	weapon_t*				active_weapon;
	weapon_t*				last_active_weapon;
	float					m_flLastUpdateTime; //0x6C
	int						m_nLastUpdateFrame; //0x70
	float					m_flLastUpdateIncrement; //0x74
	float					m_flEyeYaw; //0x78
	float					m_flEyePitch; //0x7C
	float					m_flFootYaw; //0x80
	float					m_flFootYawLast; //0x84
	float					m_flMoveYaw; //0x88
	float					m_flMoveYawIdeal; //0x8C
	float					m_flMoveYawCurrentToIdeal; //0x90
	float					m_flTimeToAlignLowerBody; // 0x94
	float					m_flPrimaryCycle; // 0x98
	float					m_flMoveWeight; // 0x9C
	float					m_flMoveWeightSmoothed; // 0xA0
	float					m_flAnimDuckAmount; // 0xA4
	float					m_flDuckAdditional; // 0xA8
	float					m_flRecrouchWeight; //0xAC
	vec3_t					m_vecPositionCurrent; //0xB0, 0xB4, 0xB8
	vec3_t					m_vecPositionLast; //0xBC, 0xC0, 0xC4
	vec3_t					m_vecVelocity; // 0xC8, 0xCC, 0xD0
	vec3_t					m_vecVelocityNormalized; // 0xD4, 0xD8, 0xDC
	vec3_t					m_vecVelocityNormalizedNonZero; // 0xE0, 0xE4, 0xE8
	float					m_flVelocityLengthXY; // 0xEC
	float					m_flVelocityLengthZ; // 0xF0
	float					m_flSpeedAsPortionOfRunTopSpeed; // 0xF4
	float					m_flSpeedAsPortionOfWalkTopSpeed; // 0xF8
	float					m_flSpeedAsPortionOfCrouchTopSpeed; // 0xFC
	float					m_flDurationMoving; // 0x100
	float					m_flDurationStill; // 0x104
	bool					m_bOnGround; // 0x108
	bool					m_bLanding; // 0x109
	char					m_pad[2]; // 0x10E
	float					m_flJumpToFall; // 0x10C
	float					m_flDurationInAir; // 0x110
	float					m_flLeftGroundHeight; // 0x114
	float					m_flLandAnimMultiplier; // 0x118 
	float					m_flWalkToRunTransition; // 0x11C
	bool					m_bLandedOnGroundThisFrame; // 0x120
	bool					m_bLeftTheGroundThisFrame; // 0x124
	float					m_flInAirSmoothValue; // 0x128
	bool					m_bOnLadder; // 0x12C
	float					m_flLadderWeight; // 0x128
	float					m_flLadderSpeed;
	bool					m_bWalkToRunTransitionState;
	bool					m_bDefuseStarted;
	bool					m_bPlantAnimStarted;
	bool					m_bTwitchAnimStarted;
	bool					m_bAdjustStarted;
	char					m_ActivityModifiers[20];
	float					m_flNextTwitchTime;
	float					m_flTimeOfLastKnownInjury;
	float					m_flLastVelocityTestTime;
	vec3_t					m_vecVelocityLast;
	vec3_t					m_vecTargetAcceleration;
	vec3_t					m_vecAcceleration;
	float					m_flAccelerationWeight;
	float					m_flAimMatrixTransition;
	float					m_flAimMatrixTransitionDelay;
	bool					m_bFlashed;
	float					m_flStrafeChangeWeight;
	float					m_flStrafeChangeTargetWeight;
	float					m_flStrafeChangeCycle;
	int						m_nStrafeSequence;
	bool					m_bStrafeChanging;
	float					m_flDurationStrafing;
	float					m_flFootLerp;
	bool					m_bFeetCrossed;
	bool					m_bPlayerIsAccelerating;
	char					padb[0x10][20];
	float					m_flDurationMoveWeightIsTooHigh;
	float					m_flStaticApproachSpeed;
	int						m_nPreviousMoveState;
	float					m_flStutterStep;
	float					m_flActionWeightBiasRemainder;
	char                    padg[0x70];
	float					m_flCameraSmoothHeight;
	bool					m_bSmoothHeightValid;
	float					m_flLastTimeVelocityOverTen;
	float					m_flAimYawMin;
	float					m_flAimYawMax;
	float					m_flAimPitchMin;
	float					m_flAimPitchMax;
	int						m_nAnimstateModelVersion;
}; // Size: 0x344

class animation_layer
{
public:
	float	m_flLayerAnimtime;
	float	m_flLayerFadeOuttime;
	int    m_fFlags;
	int		m_iActivity; // m_nDispatchedSrc
	int		m_iPriority; // m_nDispatchedDst
	int     m_nOrder;
	int m_nSequence;
	float m_flPrevCycle;
	float m_flWeight;
	float m_flWeightDeltaRate;
	float m_flPlaybackRate;
	float m_flCycle;
	entity_t* m_pOwner;
	int	m_nInvalidatePhysicsBits;
};

#pragma pack(push, 1)
struct C_AimLayer
{
	float m_flUnknown0;
	float m_flTotalTime;
	float m_flUnknown1;
	float m_flUnknown2;
	float m_flWeight;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct C_AimLayers
{
	C_AimLayer layers[3];
};
#pragma pack(pop)

struct procedural_foot_t
{
	vec3_t m_vecPosAnim;
	vec3_t m_vecPosAnimLast;
	vec3_t m_vecPosPlant;
	vec3_t m_vecPlantVel;
	float m_flLockAmount;
	float m_flLastPlantTime;
};

struct AnimstatePose_t
{
	bool		m_bInitialized;
	int			m_nIndex;
	const char* m_szName;

	AnimstatePose_t()
	{
		m_bInitialized = false;
		m_nIndex = -1;
		m_szName = "";
	}
};
class c_baseplayeranimationstate
{
public:
	void set_layer_sequence(animation_layer* animlayer, int activity);
	int select_sequence_from_activity_modifier(int activity);
	void increment_layer_cycle(animation_layer* Layer, bool is_loop);
	bool is_layer_sequence_finished(animation_layer* layer, float time);
	void set_layer_cycle(animation_layer* animlayer, float cycle);
	void set_layer_rate(animation_layer* animlayer, float rate);
	void set_layer_weight(animation_layer* animlayer, float weight);

	void* m_pThis; // 0x0
	bool					m_bIsReset; // 0x2
	bool					m_bUnknownClientBoolean; // 0x3B // am i retarded or why is this thing skips so many number.
	char					m_aSomePad[2]; // 0x3F
	int32_t					m_nTick; // 0x45
	float_t					m_flFlashedStartTime; // 0x49
	float_t					m_flFlashedEndTime; // 0x4D
	C_AimLayers				m_AimLayers; // 0x51
	int32_t					m_iModelIndex; // 0x55
	int32_t					m_iUnknownArray[3]; // 0x59
	player_t*				m_pBasePlayer; //0x60
	weapon_t*				m_pWeapon; //0x64
	weapon_t*				m_pWeaponLast; //0x68
	float					m_flLastUpdateTime; //0x6C
	int						m_nLastUpdateFrame; //0x70
	float					m_flLastUpdateIncrement; //0x74
	float					m_flEyeYaw; //0x78
	float					m_flEyePitch; //0x7C
	float					m_flFootYaw; //0x80
	float					m_flFootYawLast; //0x84
	float					m_flMoveYaw; //0x88
	float					m_flMoveYawIdeal; //0x8C
	float					m_flMoveYawCurrentToIdeal; //0x90
	float					m_flTimeToAlignLowerBody; // 0x94
	float					m_flPrimaryCycle; // 0x98
	float					m_flMoveWeight; // 0x9C
	float					m_flMoveWeightSmoothed; // 0xA0
	float					m_flAnimDuckAmount; // 0xA4
	float					m_flDuckAdditional; // 0xA8
	float					m_flRecrouchWeight; //0xAC
	vec3_t					m_vecPositionCurrent; //0xB0, 0xB4, 0xB8
	vec3_t					m_vecPositionLast; //0xBC, 0xC0, 0xC4
	vec3_t					m_vecVelocity; // 0xC8, 0xCC, 0xD0
	vec3_t					m_vecVelocityNormalized; // 0xD4, 0xD8, 0xDC
	vec3_t					m_vecVelocityNormalizedNonZero; // 0xE0, 0xE4, 0xE8
	float					m_flVelocityLengthXY; // 0xEC
	float					m_flVelocityLengthZ; // 0xF0
	float					m_flSpeedAsPortionOfRunTopSpeed; // 0xF4
	float					m_flSpeedAsPortionOfWalkTopSpeed; // 0xF8
	float					m_flSpeedAsPortionOfCrouchTopSpeed; // 0xFC
	float					m_flDurationMoving; // 0x100
	float					m_flDurationStill; // 0x104
	bool					m_bOnGround; // 0x108
	bool					m_bLanding; // 0x109
	char					m_pad[2]; // 0x10E
	float					m_flJumpToFall; // 0x10C
	float					m_flDurationInAir; // 0x110
	float					m_flLeftGroundHeight; // 0x114
	float					m_flLandAnimMultiplier; // 0x118 
	float					m_flWalkToRunTransition; // 0x11C
	bool					m_bLandedOnGroundThisFrame; // 0x120
	bool					m_bLeftTheGroundThisFrame; // 0x124
	float					m_flInAirSmoothValue; // 0x128
	bool					m_bOnLadder; // 0x12C
	float					m_flLadderWeight; // 0x128
	float					m_flLadderSpeed;
	bool					m_bWalkToRunTransitionState;
	bool					m_bDefuseStarted;
	bool					m_bPlantAnimStarted;
	bool					m_bTwitchAnimStarted;
	bool					m_bAdjustStarted;
	char					m_ActivityModifiers[20];
	float					m_flNextTwitchTime;
	float					m_flTimeOfLastKnownInjury;
	float					m_flLastVelocityTestTime;
	vec3_t					m_vecVelocityLast;
	vec3_t					m_vecTargetAcceleration;
	vec3_t					m_vecAcceleration;
	float					m_flAccelerationWeight;
	float					m_flAimMatrixTransition;
	float					m_flAimMatrixTransitionDelay;
	bool					m_bFlashed;
	float					m_flStrafeChangeWeight;
	float					m_flStrafeChangeTargetWeight;
	float					m_flStrafeChangeCycle;
	int						m_nStrafeSequence;
	bool					m_bStrafeChanging;
	float					m_flDurationStrafing;
	float					m_flFootLerp;
	bool					m_bFeetCrossed;
	bool					m_bPlayerIsAccelerating;
	AnimstatePose_t			m_tPoseParamMappings[20];
	float					m_flDurationMoveWeightIsTooHigh;
	float					m_flStaticApproachSpeed;
	int						m_nPreviousMoveState;
	float					m_flStutterStep;
	float					m_flActionWeightBiasRemainder;
	procedural_foot_t		m_footLeft;
	procedural_foot_t		m_footRight;
	float					m_flCameraSmoothHeight;
	bool					m_bSmoothHeightValid;
	float					m_flLastTimeVelocityOverTen;
	float					m_flAimYawMin;
	float					m_flAimYawMax;
	float					m_flAimPitchMin;
	float					m_flAimPitchMax;
	int						m_nAnimstateModelVersion;
};