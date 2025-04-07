#include "CommandListHandling.hpp"

#include "CommandDivider.hpp"
#include "CommandHandlingEditorSave.hpp"
#include "CommandListCommandBoxPresets.hpp"
#include "CommandListViewport.hpp"
#include "CommandRedirectToList.hpp"
#include "CommandScripts.hpp"
#include "CommandToggle.hpp"
#include "CommandVehicleHandling.hpp"

namespace Stand
{
	class CommandListSubhandling : public CommandListViewport<CommandList>
	{
	public:
		const eHandlingType handling_type;

		explicit CommandListSubhandling(CommandList* const parent, Label&& menu_name, const eHandlingType handling_type)
			: CommandListViewport(parent, std::move(menu_name)), handling_type(handling_type)
		{
		}

		void populateFlying()
		{
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_Thrust), LIT("Thrust"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_ThrustFallOff), LIT("ThrustFallOff"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_ThrustVectoring), LIT("ThrustVectoring"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_InitialThrust), LIT("InitialThrust"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_InitialThrustFallOff), LIT("InitialThrustFallOff"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_YawMult), LIT("YawMult"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_YawStabilise), LIT("YawStabilise"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_SideSlipMult), LIT("SideSlipMult"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_InitialYawMult), LIT("InitialYawMult"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_RollMult), LIT("RollMult"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_RollStabilise), LIT("RollStabilise"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_InitialRollMult), LIT("InitialRollMult"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_PitchMult), LIT("PitchMult"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_PitchStabilise), LIT("PitchStabilise"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_InitialPitchMult), LIT("InitialPitchMult"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_FormLiftMult), LIT("FormLiftMult"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_AttackLiftMult), LIT("AttackLiftMult"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_AttackDiveMult), LIT("AttackDiveMult"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_GearDownDragV), LIT("GearDownDragV"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_GearDownLiftMult), LIT("GearDownLiftMult"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_WindMult), LIT("WindMult"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_MoveRes), LIT("MoveRes"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_GearDoorFrontOpen), LIT("GearDoorFrontOpen"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_GearDoorRearOpen), LIT("GearDoorRearOpen"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_GearDoorRearOpen2), LIT("GearDoorRearOpen2"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_GearDoorRearMOpen), LIT("GearDoorRearMOpen"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_TurublenceMagnitudeMax), LIT("TurublenceMagnitudeMax"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_TurublenceForceMulti), LIT("TurublenceForceMulti"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_TurublenceRollTorqueMulti), LIT("TurublenceRollTorqueMulti"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_TurublencePitchTorqueMulti), LIT("TurublencePitchTorqueMulti"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_BodyDamageControlEffectMult), LIT("BodyDamageControlEffectMult"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_InputSensitivityForDifficulty), LIT("InputSensitivityForDifficulty"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_OnGroundYawBoostSpeedPeak), LIT("OnGroundYawBoostSpeedPeak"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_OnGroundYawBoostSpeedCap), LIT("OnGroundYawBoostSpeedCap"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_EngineOffGlideMulti), LIT("EngineOffGlideMulti"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_AfterburnerEffectRadius), LIT("AfterburnerEffectRadius"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_AfterburnerEffectDistance), LIT("AfterburnerEffectDistance"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_AfterburnerEffectForceMulti), LIT("AfterburnerEffectForceMulti"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_SubmergeLevelToPullHeliUnderwater), LIT("SubmergeLevelToPullHeliUnderwater"));
			createChild<CommandVehicleHandlingFloat>(offsetof(CFlyingHandlingData, m_ExtraLiftWithRoll), LIT("ExtraLiftWithRoll"));
		}
	};

	CommandListHandling::CommandListHandling(CommandList* const parent)
		: CommandListWithOnTickAsActiveList(parent, LOC("HNEDIT"))
	{
		CommandIssuable::collapse_command_names.emplace(L"vh");

		{
			auto base = this->createChild<CommandListSubhandling>(LOC("HNE_BASE"), HANDLING_TYPE_INVALID);

			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_fMass), LIT("Mass"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_acceleration), LIT("Acceleration"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_suspension_force), LIT("SuspensionForce"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_suspension_raise), LIT("SuspensionRaise"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_suspension_comp_damp), LIT("SuspensionCompDamp"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_traction_loss_mult), LIT("TractionLossMult"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_upshift), LIT("Upshift"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_downshift), LIT("Downshift"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_deformation_mult), LIT("DeformationMult"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_traction_curve_min), LIT("TractionCurveMin"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_traction_curve_max), LIT("TractionCurveMax"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_fDownforceModifier), LIT("DownforceModifier"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_fInitialDragCoeff), LIT("InitialDragCoeff"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_fPopUpLightRotation), LIT("PopupLightRotation"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_buoyancy), LIT("Buoyancy"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_drive_bias_rear), LIT("DriveBiasRear"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_drive_bias_front), LIT("DriveBiasFront"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_drive_inertia), LIT("DriveInertia"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_initial_drive_force), LIT("InitialDriveForce"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_drive_max_flat_velocity), LIT("DriveMaxFlatVelocity"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_initial_drive_max_flat_vel), LIT("InitialDriveMaxFlatVel"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_brake_force), LIT("BrakeForce"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_brake_bias_front), LIT("BrakeBiasFront"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_brake_bias_rear), LIT("BrakeBiasRear"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_handbrake_force), LIT("HandbrakeForce"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_steering_lock), LIT("SteeringLock"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_steering_lock_ratio), LIT("SteeringLockRatio"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_traction_curve_lateral), LIT("TractionCurveLateral"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_curve_lateral), LIT("CurveLateral"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_curve_lateral_ratio), LIT("CurveLateralRatio"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_traction_spring_delta_max), LIT("TractionSpringDeltaMax"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_traction_spring_delta_max_ratio), LIT("TractionSpringDeltaMaxRatio"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_low_speed_traction_loss_mult), LIT("LowSpeedTractionLossMult"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_camber_stiffness), LIT("CamberStiffness"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_traction_bias_front), LIT("TractionBiasFront"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_traction_bias_rear), LIT("TractionBiasRear"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_suspension_rebound_damp), LIT("SuspensionReboundDamp"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_suspension_upper_limit), LIT("SuspensionUpperLimit"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_suspension_lower_limit), LIT("SuspensionLowerLimit"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_suspension_bias_front), LIT("SuspensionBiasFront"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_suspension_bias_rear), LIT("SuspensionBiasRear"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_anti_rollbar_force), LIT("AntiRollbarForce"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_anti_rollbar_bias_front), LIT("AntiRollbarBiasFront"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_anti_rollbar_bias_rear), LIT("AntiRollbarBiasRear"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_roll_centre_height_front), LIT("RollCentreHeightFront"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_roll_centre_height_rear), LIT("RollCentreHeightRear"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_collision_damage_mult), LIT("CollisionDamageMult"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_weapon_damamge_mult), LIT("WeaponDamamgeMult"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_engine_damage_mult), LIT("EngineDamageMult"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_petrol_tank_volume), LIT("PetrolTankVolume"));
			base->createChild<CommandVehicleHandlingFloat>(offsetof(CHandlingData, m_oil_volume), LIT("OilVolume"));
		}

		// CBikeHandlingData
		{
			auto bike = this->createChild<CommandListSubhandling>(LOC("HNE_BIKES"), HANDLING_TYPE_BIKE);

			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_LeanFwdCOMMult), LIT("LeanFwdCOMMult"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_LeanFwdForceMult), LIT("LeanFwdForceMult"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_LeanBakCOMMult), LIT("LeanBakCOMMult"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_LeanBakForceMult), LIT("LeanBakForceMult"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_MaxBankAngle), LIT("MaxBankAngle"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_FullAnimAngle), LIT("FullAnimAngle"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_FullAnimAngleInv), LIT("FullAnimAngleInv"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_DesLeanReturnFrac), LIT("DesLeanReturnFrac"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_StickLeanMult), LIT("StickLeanMult"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_BrakingStabilityMult), LIT("BrakingStabilityMult"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_InAirSteerMult), LIT("InAirSteerMult"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_WheelieBalancePoint), LIT("WheelieBalancePoint"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_StoppieBalancePoint), LIT("StoppieBalancePoint"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_WheelieSteerMult), LIT("WheelieSteerMult"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_RearBalanceMult), LIT("RearBalanceMult"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_FrontBalanceMult), LIT("FrontBalanceMult"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_BikeWheelGroundSideFrictionMult), LIT("BikeWheelGroundSideFrictionMult"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_BikeGroundSideFrictionMult), LIT("BikeGroundSideFrictionMult"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_BikeOnStandLeanAngle), LIT("BikeOnStandLeanAngle"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_BikeOnStandSteerAngle), LIT("BikeOnStandSteerAngle"));
			bike->createChild<CommandVehicleHandlingFloat>(offsetof(CBikeHandlingData, m_JumpForce), LIT("JumpForce"));
		}

		// CBoatHandlingData
		{
			auto boat = this->createChild<CommandListSubhandling>(LOC("boat"), HANDLING_TYPE_BOAT);

			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_BoxFrontMult), LIT("BoxFrontMult"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_BoxRearMult), LIT("BoxRearMult"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_BoxSideMult), LIT("BoxSideMult"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_SampleTop), LIT("SampleTop"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_SampleBottom), LIT("SampleBottom"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_SampleBottomTestCorrection), LIT("SampleBottomTestCorrection"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_AquaplaneForce), LIT("AquaplaneForce"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_AquaplanePushWaterMult), LIT("AquaplanePushWaterMult"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_AquaplanePushWaterCap), LIT("AquaplanePushWaterCap"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_AquaplanePushWaterApply), LIT("AquaplanePushWaterApply"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_RudderForce), LIT("RudderForce"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_RudderOffsetSubmerge), LIT("RudderOffsetSubmerge"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_RudderOffsetForce), LIT("RudderOffsetForce"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_RudderOffsetForceZMult), LIT("RudderOffsetForceZMult"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_WaveAudioMult), LIT("WaveAudioMult"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_Look_L_R_CamHeight), LIT("Look_L_R_CamHeight"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_DragCoefficient), LIT("DragCoefficient"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_KeelSphereSize), LIT("KeelSphereSize"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_PropRadius), LIT("PropRadius"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_LowLodAngOffset), LIT("LowLodAngOffset"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_LowLodDraughtOffset), LIT("LowLodDraughtOffset"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_ImpellerOffset), LIT("ImpellerOffset"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_ImpellerForceMult), LIT("ImpellerForceMult"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_DinghySphereBuoyConst), LIT("DinghySphereBuoyConst"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_ProwRaiseMult), LIT("ProwRaiseMult"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_DeepWaterSampleBuoyancyMult), LIT("DeepWaterSampleBuoyancyMult"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_TransmissionMultiplier), LIT("TransmissionMultiplier"));
			boat->createChild<CommandVehicleHandlingFloat>(offsetof(CBoatHandlingData, m_TractionMultiplier), LIT("TractionMultiplier"));
		}

		// CCarHandlingData
		{
			auto car = this->createChild<CommandListSubhandling>(LOC("HNE_CAR"), HANDLING_TYPE_CAR);

			car->createChild<CommandVehicleHandlingFloat>(offsetof(CCarHandlingData, m_BackEndPopUpCarImpulseMult), LIT("BackEndPopUpCarImpulseMult"));
			car->createChild<CommandVehicleHandlingFloat>(offsetof(CCarHandlingData, m_BackEndPopUpBuildingImpulseMult), LIT("BackEndPopUpBuildingImpulseMult"));
			car->createChild<CommandVehicleHandlingFloat>(offsetof(CCarHandlingData, m_BackEndPopUpMaxDeltaSpeed), LIT("BackEndPopUpMaxDeltaSpeed"));
			car->createChild<CommandVehicleHandlingFloat>(offsetof(CCarHandlingData, m_ToeFront), LIT("ToeFront"));
			car->createChild<CommandVehicleHandlingFloat>(offsetof(CCarHandlingData, m_ToeRear), LIT("ToeRear"));
			car->createChild<CommandVehicleHandlingFloat>(offsetof(CCarHandlingData, m_CamberFront), LIT("CamberFront"));
			car->createChild<CommandVehicleHandlingFloat>(offsetof(CCarHandlingData, m_CamberRear), LIT("CamberRear"));
			car->createChild<CommandVehicleHandlingFloat>(offsetof(CCarHandlingData, m_Castor), LIT("Castor"));
			car->createChild<CommandVehicleHandlingFloat>(offsetof(CCarHandlingData, m_EngineResistance), LIT("EngineResistance"));
			car->createChild<CommandVehicleHandlingFloat>(offsetof(CCarHandlingData, m_MaxDriveBiasTransfer), LIT("MaxDriveBiasTransfer"));
			car->createChild<CommandVehicleHandlingFloat>(offsetof(CCarHandlingData, m_JumpForceScale), LIT("JumpForceScale"));
			car->createChild<CommandVehicleHandlingFloat>(offsetof(CCarHandlingData, m_IncreasedRammingForceScale), LIT("IncreasedRammingForceScale"));
		}

		// CTrailerHandlingData
		{
			auto trl = this->createChild<CommandListSubhandling>(LOC("HNE_TRAILER"), HANDLING_TYPE_TRAILER);

			trl->createChild<CommandVehicleHandlingFloat>(offsetof(CTrailerHandlingData, m_AttachLimitPitch), LIT("AttachLimitPitch"));
			trl->createChild<CommandVehicleHandlingFloat>(offsetof(CTrailerHandlingData, m_AttachLimitRoll), LIT("AttachLimitRoll"));
			trl->createChild<CommandVehicleHandlingFloat>(offsetof(CTrailerHandlingData, m_AttachLimitYaw), LIT("AttachLimitYaw"));
			trl->createChild<CommandVehicleHandlingFloat>(offsetof(CTrailerHandlingData, m_UprightSpringConstant), LIT("UprightSpringConstant"));
			trl->createChild<CommandVehicleHandlingFloat>(offsetof(CTrailerHandlingData, m_UprightDampingConstant), LIT("UprightDampingConstant"));
			trl->createChild<CommandVehicleHandlingFloat>(offsetof(CTrailerHandlingData, m_AttachedMaxDistance), LIT("AttachedMaxDistance"));
			trl->createChild<CommandVehicleHandlingFloat>(offsetof(CTrailerHandlingData, m_AttachedMaxPenetration), LIT("AttachedMaxPenetration"));
			trl->createChild<CommandVehicleHandlingFloat>(offsetof(CTrailerHandlingData, m_AttachRaiseZ), LIT("AttachRaiseZ"));
			trl->createChild<CommandVehicleHandlingFloat>(offsetof(CTrailerHandlingData, m_PosConstraintMassRatio), LIT("PosConstraintMassRatio"));
		}

		this->createChild<CommandListSubhandling>(LOC("HNE_FLYING"), HANDLING_TYPE_FLYING)->populateFlying();
		this->createChild<CommandListSubhandling>(LOC("HNE_VERTFLY"), HANDLING_TYPE_VERTICAL_FLYING)->populateFlying();

		// CSpecialFlightHandlingData
		{
			auto specfly = this->createChild<CommandListSubhandling>(LOC("HNE_SPECFLY"), HANDLING_TYPE_SPECIAL_FLIGHT);

			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_LiftCoefficient), LIT("LiftCoefficient"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_CriticalLiftAngle), LIT("CriticalLiftAngle"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_InitialLiftAngle), LIT("InitialLiftAngle"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_MaxLiftAngle), LIT("MaxLiftAngle"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_DragCoefficient), LIT("DragCoefficient"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_BrakingDrag), LIT("BrakingDrag"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_MaxLiftVelocity), LIT("MaxLiftVelocity"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_MinLiftVelocity), LIT("MinLiftVelocity"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_RollTorqueScale), LIT("RollTorqueScale"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_MaxTorqueVelocity), LIT("MaxTorqueVelocity"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_MinTorqueVelocity), LIT("MinTorqueVelocity"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_YawTorqueScale), LIT("YawTorqueScale"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_SelfLevelingPitchTorqueScale), LIT("SelfLevelingPitchTorqueScale"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_SelfLevelingRollTorqueScale), LIT("SelfLevelingRollTorqueScale"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_MaxPitchTorque), LIT("MaxPitchTorque"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_MaxSteeringRollTorque), LIT("MaxSteeringRollTorque"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_PitchTorqueScale), LIT("PitchTorqueScale"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_SteeringTorqueScale), LIT("SteeringTorqueScale"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_MaxThrust), LIT("MaxThrust"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_TransitionDuration), LIT("TransitionDuration"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_HoverVelocityScale), LIT("HoverVelocityScale"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_StabilityAssist), LIT("StabilityAssist"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_MinSpeedForThrustFalloff), LIT("MinSpeedForThrustFalloff"));
			specfly->createChild<CommandVehicleHandlingFloat>(offsetof(CSpecialFlightHandlingData, m_BrakingThrustScale), LIT("BrakingThrustScale"));
			specfly->createChild<CommandVehicleHandlingInt>(offsetof(CSpecialFlightHandlingData, m_Mode), LIT("Mode"));
		}

		// CSeaPlaneHandlingData
		{
			auto sea = this->createChild<CommandListSubhandling>(LOC("HNE_SEAPLANE"), HANDLING_TYPE_SEAPLANE);

			sea->createChild<CommandVehicleHandlingInt>(offsetof(CSeaPlaneHandlingData, m_LeftPontoonComponentId), LIT("LeftPontoonComponentId"));
			sea->createChild<CommandVehicleHandlingInt>(offsetof(CSeaPlaneHandlingData, m_RightPontoonComponentId), LIT("RightPontoonComponentId"));
			sea->createChild<CommandVehicleHandlingFloat>(offsetof(CSeaPlaneHandlingData, m_PontoonBuoyConst), LIT("PontoonBuoyConst"));
			sea->createChild<CommandVehicleHandlingFloat>(offsetof(CSeaPlaneHandlingData, m_PontoonSampleSizeFront), LIT("PontoonSampleSizeFront"));
			sea->createChild<CommandVehicleHandlingFloat>(offsetof(CSeaPlaneHandlingData, m_PontoonSampleSizeMiddle), LIT("PontoonSampleSizeMiddle"));
			sea->createChild<CommandVehicleHandlingFloat>(offsetof(CSeaPlaneHandlingData, m_PontoonSampleSizeRear), LIT("PontoonSampleSizeRear"));
			sea->createChild<CommandVehicleHandlingFloat>(offsetof(CSeaPlaneHandlingData, m_PontoonLengthFractionForSamples), LIT("PontoonLengthFractionForSamples"));
			sea->createChild<CommandVehicleHandlingFloat>(offsetof(CSeaPlaneHandlingData, m_PontoonDragCoefficient), LIT("PontoonDragCoefficient"));
			sea->createChild<CommandVehicleHandlingFloat>(offsetof(CSeaPlaneHandlingData, m_PontoonVerticalDampingCoefficientUp), LIT("PontoonVerticalDampingCoefficientUp"));
			sea->createChild<CommandVehicleHandlingFloat>(offsetof(CSeaPlaneHandlingData, m_PontoonVerticalDampingCoefficientDown), LIT("PontoonVerticalDampingCoefficientDown"));
			sea->createChild<CommandVehicleHandlingFloat>(offsetof(CSeaPlaneHandlingData, m_KeelSphereSize), LIT("KeelSphereSize"));
		}

		// CSubmarineHandlingData
		{
			auto sub = this->createChild<CommandListSubhandling>(LOC("HNE_SUBMARINE"), HANDLING_TYPE_SUBMARINE);

			sub->createChild<CommandVehicleHandlingFloat>(offsetof(CSubmarineHandlingData, m_MoveResXY), LIT("MoveResXY"));
			sub->createChild<CommandVehicleHandlingFloat>(offsetof(CSubmarineHandlingData, m_MoveResZ), LIT("MoveResZ"));
			sub->createChild<CommandVehicleHandlingFloat>(offsetof(CSubmarineHandlingData, m_PitchMult), LIT("PitchMult"));
			sub->createChild<CommandVehicleHandlingFloat>(offsetof(CSubmarineHandlingData, m_PitchAngle), LIT("PitchAngle"));
			sub->createChild<CommandVehicleHandlingFloat>(offsetof(CSubmarineHandlingData, m_YawMult), LIT("YawMult"));
			sub->createChild<CommandVehicleHandlingFloat>(offsetof(CSubmarineHandlingData, m_DiveSpeed), LIT("DiveSpeed"));
			sub->createChild<CommandVehicleHandlingFloat>(offsetof(CSubmarineHandlingData, m_RollMult), LIT("RollMult"));
			sub->createChild<CommandVehicleHandlingFloat>(offsetof(CSubmarineHandlingData, m_RollStab), LIT("RollStab"));
		}

		show_na = this->createChild<CommandToggle>(LOC("SHWNA"));

		this->createChild<CommandDivider>(LOC("PRSTS"));
		this->createChild<CommandListCommandBoxPresets<CommandHandlingEditorSave>>(CMDNAMES_OBF("handlingpreset"), "Handling");
	}

	void CommandListHandling::onTickAsActiveList()
	{
		CHandlingData* h = nullptr;
		if (auto v = g_player_veh.getCVehicle())
		{
			h = v->handling_data;
		}

		Command* prev_focus;
		bool any_change = false;
		bool iterator_invalidated;
		do
		{
			iterator_invalidated = false;
			for (auto i = this->children.cbegin() + 1; i != this->children.cend(); ++i)
			{
				if (!i->get()->isList() || i->get()->getMenuName() == ATSTRINGHASH("PRSTS"))
				{
					continue;
				}
				auto cmd = (*i)->as<CommandListSubhandling>();
				if (h && h->hasSubhandlingData(cmd->handling_type))
				{
					cmd->setHelpText(NOLABEL);
				}
				else
				{
					if (!show_na->m_on)
					{
						cmd->setHelpText(NOLABEL);
						if (isVisible(cmd))
						{
							iterator_invalidated = true;
							if (!any_change)
							{
								any_change = true;
								prev_focus = getFocus();
							}
							setVisible(cmd, false);
							break;
						}
						continue;
					}
					cmd->setHelpText(LOC("HNE_NA"));
				}
				if (!isVisible(cmd))
				{
					iterator_invalidated = true;
					if (!any_change)
					{
						any_change = true;
						prev_focus = getFocus();
					}
					setVisible(cmd, true);
					break;
				}
			}
		} while (iterator_invalidated);
		if (any_change)
		{
			processChildrenUpdateWithPossibleCursorDisplacement(prev_focus, TC_SCRIPT_NOYIELD);
		}
	}
}
