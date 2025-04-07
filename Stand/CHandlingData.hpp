#pragma once

#include "struct_base.hpp"

#include "atArray.hpp"
#include "vector.hpp"

namespace rage
{
	class parStructure;
}

class CHandlingObject
{
public:
	virtual ~CHandlingObject() = default;
	virtual rage::parStructure* parser_GetStructure() { return nullptr; }
};

enum eHandlingType
{
	HANDLING_TYPE_INVALID = -1,
	HANDLING_TYPE_BIKE = 0,
	HANDLING_TYPE_FLYING, // CFlyingHandlingData
	HANDLING_TYPE_VERTICAL_FLYING, // CFlyingHandlingData
	HANDLING_TYPE_BOAT,
	HANDLING_TYPE_SEAPLANE,
	HANDLING_TYPE_SUBMARINE,
	HANDLING_TYPE_TRAIN,
	HANDLING_TYPE_TRAILER,
	HANDLING_TYPE_CAR,
	HANDLING_TYPE_WEAPON,
	HANDLING_TYPE_SPECIAL_FLIGHT,
	HANDLING_TYPE_MAX_TYPES
};

class CBaseSubHandlingData : public CHandlingObject
{
public:
	virtual eHandlingType GetHandlingType() = 0;
	virtual void ConvertToGameUnits() {}
};

class CAIHandlingInfo;

#pragma pack(push, 1)
class CHandlingData : public CHandlingObject
{
public:
	/* 0x008 */ uint32_t m_handlingName;
	/* 0x00C */ float m_fMass;
	/* 0x010 */ float m_fInitialDragCoeff; // (used to be calculated from dimensions), This can be modified after creation, so check CVehicle m_fDragCoeff for actual dragCoeff
	/* 0x014 */ float m_fDownforceModifier;
	/* 0x018 */ float m_fPopUpLightRotation;
	/* 0x01C */ char pad_001C[4];
	/* 0x020 */ rage::Vector3 m_centre_of_mass;
	/* 0x030 */ rage::Vector3 m_inertia_mult;
	/* 0x040 */ float m_buoyancy;
	/* 0x044 */ float m_drive_bias_rear;
	/* 0x048 */ float m_drive_bias_front;
	/* 0x04C */ float m_acceleration;
	/* 0x050 */ uint8_t m_initial_drive_gears;
	/* 0x051 */ char pad_0051[3];
	/* 0x054 */ float m_drive_inertia;
	/* 0x058 */ float m_upshift;
	/* 0x05C */ float m_downshift;
	/* 0x060 */ float m_initial_drive_force;
	/* 0x064 */ float m_drive_max_flat_velocity;
	/* 0x068 */ float m_initial_drive_max_flat_vel;
	/* 0x06C */ float m_brake_force;
	/* 0x070 */ char pad_0070[4];
	/* 0x074 */ float m_brake_bias_front;
	/* 0x078 */ float m_brake_bias_rear;
	/* 0x07C */ float m_handbrake_force;
	/* 0x080 */ float m_steering_lock;
	/* 0x084 */ float m_steering_lock_ratio;
	/* 0x088 */ float m_traction_curve_max;
	/* 0x08C */ float m_traction_curve_lateral;
	/* 0x090 */ float m_traction_curve_min;
	/* 0x094 */ float m_traction_curve_ratio;
	/* 0x098 */ float m_curve_lateral;
	/* 0x09C */ float m_curve_lateral_ratio;
	/* 0x0A0 */ float m_traction_spring_delta_max;
	/* 0x0A4 */ float m_traction_spring_delta_max_ratio;
	/* 0x0A8 */ float m_low_speed_traction_loss_mult;
	/* 0x0AC */ float m_camber_stiffness;
	/* 0x0B0 */ float m_traction_bias_front;
	/* 0x0B4 */ float m_traction_bias_rear;
	/* 0x0B8 */ float m_traction_loss_mult;
	/* 0x0BC */ float m_suspension_force;
	/* 0x0C0 */ float m_suspension_comp_damp;
	/* 0x0C4 */ float m_suspension_rebound_damp;
	/* 0x0C8 */ float m_suspension_upper_limit;
	/* 0x0CC */ float m_suspension_lower_limit;
	/* 0x0D0 */ float m_suspension_raise;
	/* 0x0D4 */ float m_suspension_bias_front;
	/* 0x0D8 */ float m_suspension_bias_rear;
	/* 0x0DC */ float m_anti_rollbar_force;
	/* 0x0E0 */ float m_anti_rollbar_bias_front;
	/* 0x0E4 */ float m_anti_rollbar_bias_rear;
	/* 0x0E8 */ float m_roll_centre_height_front;
	/* 0x0EC */ float m_roll_centre_height_rear;
	/* 0x0F0 */ float m_collision_damage_mult;
	/* 0x0F4 */ float m_weapon_damamge_mult;
	/* 0x0F8 */ float m_deformation_mult;
	/* 0x0FC */ float m_engine_damage_mult;
	/* 0x100 */ float m_petrol_tank_volume;
	/* 0x104 */ float m_oil_volume;
	/* 0x108 */ char pad_0108[4];
	/* 0x10C */ Stand::v3 m_seat_offset_dist;
	/* 0x118 */ uint32_t m_monetary_value;
	/* 0x11C */ char pad_011C[8];
	/* 0x124 */ uint32_t m_model_flags;
	/* 0x128 */ uint32_t m_handling_flags;
	/* 0x12C */ uint32_t m_damage_flags;
	/* 0x130 */ char pad_0130[12];
	/* 0x13C */ uint32_t m_ai_handling_hash;
	PAD(0x013C + 4, 0x150) CAIHandlingInfo* ai_handling_info;
	/* 0x158 */ rage::atArray<CBaseSubHandlingData*> sub_handling_arr;

	[[nodiscard]] bool hasSubhandlingData(eHandlingType type) const noexcept
	{
		return getSubhandlingData(type) != nullptr;
	}

	[[nodiscard]] CBaseSubHandlingData* getSubhandlingData(eHandlingType type) const noexcept
	{
		for (const auto& sub_handling : sub_handling_arr)
		{
			if (sub_handling)
			{
				if (sub_handling->GetHandlingType() == type)
				{
					return sub_handling;
				}
			}
		}
		return nullptr;
	}
};
static_assert(sizeof(CHandlingData) == 0x158 + sizeof(rage::atArray<CBaseSubHandlingData*>));

#define MAX_NUM_VEHICLE_WEAPONS 6

class CVehicleWeaponHandlingData : public CBaseSubHandlingData
{
public:
	/* 0x08 */ uint32_t uWeaponHash[MAX_NUM_VEHICLE_WEAPONS];
	/* 0x20 */ int32_t WeaponSeats[MAX_NUM_VEHICLE_WEAPONS]; // value is (seat + 1) as for which seat has access to it
	PAD(0x020 + (4 * MAX_NUM_VEHICLE_WEAPONS), 0x050) float fTurretSpeed[MAX_NUM_VEHICLE_WEAPONS];
	PAD(0x050 + (4 * MAX_NUM_VEHICLE_WEAPONS), 0x080) float fTurretPitchMin[MAX_NUM_VEHICLE_WEAPONS];
	PAD(0x080 + (4 * MAX_NUM_VEHICLE_WEAPONS), 0x0B0) float fTurretPitchMax[MAX_NUM_VEHICLE_WEAPONS];
	PAD(0x0B0 + (4 * MAX_NUM_VEHICLE_WEAPONS), 0x0E0) float fTurretCamPitchMin[MAX_NUM_VEHICLE_WEAPONS];
	PAD(0x0E0 + (4 * MAX_NUM_VEHICLE_WEAPONS), 0x110) float fTurretCamPitchMax[MAX_NUM_VEHICLE_WEAPONS];
	PAD(0x110 + (4 * MAX_NUM_VEHICLE_WEAPONS), 0x140) float fBulletVelocityForGravity[MAX_NUM_VEHICLE_WEAPONS];
	PAD(0x140 + (4 * MAX_NUM_VEHICLE_WEAPONS), 0x170) float fTurretPitchForwardMin[MAX_NUM_VEHICLE_WEAPONS];
	PAD(0x170 + (4 * MAX_NUM_VEHICLE_WEAPONS), 0x320) float fUvAnimationMult;
	/* 0x324 */ float fMiscGadgetVar;
	/* 0x328 */ float fWheelImpactOffset;
};
static_assert(sizeof(CVehicleWeaponHandlingData) == 0x328 + 4);
#pragma pack(pop)

class CSubmarineHandlingData : public CBaseSubHandlingData
{
public:
	INIT_PAD(CBaseSubHandlingData, 0x10);
	rage::Vec3V m_TurnRes;     // 0x0010
	float m_MoveResXY;         // 0x0020
	float m_MoveResZ;          // 0x0024
	float m_PitchMult;         // 0x0028
	float m_PitchAngle;        // 0x002C
	float m_YawMult;           // 0x0030
	float m_DiveSpeed;         // 0x0034
	float m_RollMult;          // 0x0038
	float m_RollStab;          // 0x003C
};

class CTrailerHandlingData : public CBaseSubHandlingData
{
public:
	float m_AttachLimitPitch;        // 0x0008
	float m_AttachLimitRoll;         // 0x000C
	float m_AttachLimitYaw;          // 0x0010
	float m_UprightSpringConstant;   // 0x0014
	float m_UprightDampingConstant;  // 0x0018
	float m_AttachedMaxDistance;     // 0x001C
	float m_AttachedMaxPenetration;  // 0x0020
	float m_AttachRaiseZ;            // 0x0024
	float m_PosConstraintMassRatio;  // 0x0028
};

class CSeaPlaneHandlingData : public CBaseSubHandlingData
{
public:
	int   m_LeftPontoonComponentId;                 // 0x0008
	int   m_RightPontoonComponentId;                // 0x000C
	float m_PontoonBuoyConst;                       // 0x0010
	float m_PontoonSampleSizeFront;                 // 0x0014
	float m_PontoonSampleSizeMiddle;                // 0x0018
	float m_PontoonSampleSizeRear;                  // 0x001C
	float m_PontoonLengthFractionForSamples;        // 0x0020
	float m_PontoonDragCoefficient;                 // 0x0024
	float m_PontoonVerticalDampingCoefficientUp;    // 0x0028
	float m_PontoonVerticalDampingCoefficientDown;  // 0x002C
	float m_KeelSphereSize;                         // 0x0030
};

class CSpecialFlightHandlingData : public CBaseSubHandlingData
{
public:
	eHandlingType GetHandlingType() final { return HANDLING_TYPE_SPECIAL_FLIGHT; }

	INIT_PAD(CBaseSubHandlingData, 0x10);
	rage::Vec3V    m_VecAngularDamping;              // 0x0010
	rage::Vec3V    m_VecAngularDampingMin;           // 0x0020
	rage::Vec3V    m_VecLinearDamping;               // 0x0030
	rage::Vec3V    m_VecLinearDampingMin;            // 0x0040
	float m_LiftCoefficient;                         // 0x0050
	float m_CriticalLiftAngle;                       // 0x0054
	float m_InitialLiftAngle;                        // 0x0058
	float m_MaxLiftAngle;                            // 0x005C
	float m_DragCoefficient;                         // 0x0060
	float m_BrakingDrag;                             // 0x0064
	float m_MaxLiftVelocity;                         // 0x0068
	float m_MinLiftVelocity;                         // 0x006C
	float m_RollTorqueScale;                         // 0x0070
	float m_MaxTorqueVelocity;                       // 0x0074
	float m_MinTorqueVelocity;                       // 0x0078
	float m_YawTorqueScale;                          // 0x007C
	float m_SelfLevelingPitchTorqueScale;            // 0x0080
	float m_SelfLevelingRollTorqueScale;             // 0x0084
	float m_MaxPitchTorque;                          // 0x0088
	float m_MaxSteeringRollTorque;                   // 0x008C
	float m_PitchTorqueScale;                        // 0x0090
	float m_SteeringTorqueScale;                     // 0x0094
	float m_MaxThrust;                               // 0x0098
	float m_TransitionDuration;                      // 0x009C
	float m_HoverVelocityScale;                      // 0x00A0
	float m_StabilityAssist;                         // 0x00A4
	float m_MinSpeedForThrustFalloff;                // 0x00A8
	float m_BrakingThrustScale;                      // 0x00AC
	int    m_Mode;                                   // 0x00B0
	const char* m_StrFlags;                          // 0x00B4 (rage::atFinalHashString)
};

class CBoatHandlingData : public CBaseSubHandlingData
{
public:
	float m_BoxFrontMult;                   // 0x0008
	float m_BoxRearMult;                    // 0x000C
	float m_BoxSideMult;                    // 0x0010
	float m_SampleTop;                      // 0x0014
	float m_SampleBottom;                   // 0x0018
	float m_SampleBottomTestCorrection;     // 0x001C
	float m_AquaplaneForce;                 // 0x0020
	float m_AquaplanePushWaterMult;         // 0x0024
	float m_AquaplanePushWaterCap;          // 0x0028
	float m_AquaplanePushWaterApply;        // 0x002C
	float m_RudderForce;                    // 0x0030
	float m_RudderOffsetSubmerge;           // 0x0034
	float m_RudderOffsetForce;              // 0x0038
	float m_RudderOffsetForceZMult;         // 0x003C
	float m_WaveAudioMult;                  // 0x0040
	rage::Vec3V m_VecMoveResistance;		// 0x0050
	rage::Vec3V m_VecTurnResistance;		// 0x0060
	float m_Look_L_R_CamHeight;             // 0x0070
	float m_DragCoefficient;                // 0x0074
	float m_KeelSphereSize;                 // 0x0078
	float m_PropRadius;                     // 0x007C
	float m_LowLodAngOffset;                // 0x0080
	float m_LowLodDraughtOffset;            // 0x0084
	float m_ImpellerOffset;                 // 0x0088
	float m_ImpellerForceMult;              // 0x008C
	float m_DinghySphereBuoyConst;          // 0x0090
	float m_ProwRaiseMult;                  // 0x0094
	float m_DeepWaterSampleBuoyancyMult;    // 0x0098
	float m_TransmissionMultiplier;         // 0x009C
	float m_TractionMultiplier;             // 0x00A0
};

class CCarHandlingData : public CBaseSubHandlingData
{
public:
	/* 0x8 */ float m_BackEndPopUpCarImpulseMult;
	/* 0xc */ float m_BackEndPopUpBuildingImpulseMult;
	/* 0x10 */ float m_BackEndPopUpMaxDeltaSpeed;
	/* 0x14 */ float m_ToeFront;
	/* 0x18 */ float m_ToeRear;
	/* 0x1c */ float m_CamberFront;
	/* 0x20 */ float m_CamberRear;
	/* 0x24 */ float m_Castor;
	/* 0x28 */ float m_EngineResistance;
	/* 0x2c */ float m_MaxDriveBiasTransfer;
	/* 0x30 */ float m_JumpForceScale;
	/* 0x34 */ float m_IncreasedRammingForceScale;
	/* 0x38 */ uint32_t m_strAdvancedFlags;
	/* 0x3C */ uint32_t aFlags; // This is actually where "strAdvancedFlags" from online guides should go.
	// /* 0x40 */ rage::atArray<CAdvancedData, 0, unsigned short> m_AdvancedData has CAN_WHEELIE, only thing we'd want.
};
static_assert(sizeof(CCarHandlingData) == 0x3C + 4);

class CBikeHandlingData : public CBaseSubHandlingData
{
public:
	float m_LeanFwdCOMMult;            	      // 0x0008
	float m_LeanFwdForceMult;          	      // 0x000C
	float m_LeanBakCOMMult;            	      // 0x0010
	float m_LeanBakForceMult;          	      // 0x0014
	float m_MaxBankAngle;              	      // 0x0018
	float m_FullAnimAngle;             	      // 0x001C
	float m_FullAnimAngleInv;                 // 0x0020
	float m_DesLeanReturnFrac;         	      // 0x0024
	float m_StickLeanMult;             	      // 0x0028
	float m_BrakingStabilityMult;      	      // 0x002C
	float m_InAirSteerMult;            	      // 0x0030
	float m_WheelieBalancePoint;       	      // 0x0034
	float m_StoppieBalancePoint;       	      // 0x0038
	float m_WheelieSteerMult;          	      // 0x003C
	float m_RearBalanceMult;           	      // 0x0040
	float m_FrontBalanceMult;          	      // 0x0044
	float m_BikeGroundSideFrictionMult;	      // 0x0048
	float m_BikeWheelGroundSideFrictionMult;  // 0x004C
	float m_BikeOnStandLeanAngle; 		      // 0x0050
	float m_BikeOnStandSteerAngle;		      // 0x0054
	float m_JumpForce;            		      // 0x0058
};

class CFlyingHandlingData : public CBaseSubHandlingData
{
public:
	float m_Thrust;									// 0x0008
	float m_ThrustFallOff;							// 0x000C
	float m_ThrustVectoring;						// 0x0010
	float m_InitialThrust;							// 0x0014
	float m_InitialThrustFallOff;					// 0x0018
	float m_YawMult;								// 0x001C
	float m_YawStabilise;							// 0x0020
	float m_SideSlipMult;							// 0x0024
	float m_InitialYawMult;							// 0x0028
	float m_RollMult;								// 0x002C
	float m_RollStabilise;							// 0x0030
	float m_InitialRollMult;						// 0x0034
	float m_PitchMult;								// 0x0038
	float m_PitchStabilise;							// 0x003C
	float m_InitialPitchMult;						// 0x0040
	float m_FormLiftMult;							// 0x0044
	float m_AttackLiftMult;							// 0x0048
	float m_AttackDiveMult;							// 0x004C
	float m_GearDownDragV;							// 0x0050
	float m_GearDownLiftMult;						// 0x0054
	float m_WindMult;								// 0x0058
	float m_MoveRes;								// 0x005C
	rage::Vec3V m_VecTurnRes;						// 0x0060
	rage::Vec3V m_VecSpeedRes;						// 0x0070
	float m_GearDoorFrontOpen;						// 0x0080
	float m_GearDoorRearOpen;						// 0x0084
	float m_GearDoorRearOpen2;						// 0x0088
	float m_GearDoorRearMOpen;						// 0x008C
	float m_TurublenceMagnitudeMax;					// 0x0090
	float m_TurublenceForceMulti;					// 0x0094
	float m_TurublenceRollTorqueMulti;				// 0x0098
	float m_TurublencePitchTorqueMulti;				// 0x009C
	float m_BodyDamageControlEffectMult;			// 0x00A0
	float m_InputSensitivityForDifficulty;			// 0x00A4
	float m_OnGroundYawBoostSpeedPeak;				// 0x00A8
	float m_OnGroundYawBoostSpeedCap;				// 0x00AC
	float m_EngineOffGlideMulti;					// 0x00B0
	float m_AfterburnerEffectRadius;				// 0x00B4
	float m_AfterburnerEffectDistance;				// 0x00B8
	float m_AfterburnerEffectForceMulti;			// 0x00BC
	float m_SubmergeLevelToPullHeliUnderwater;		// 0x00C0
	float m_ExtraLiftWithRoll;						// 0x00C4
	eHandlingType m_HandlingType;					// 0x00C8
};
