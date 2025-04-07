#pragma once

enum eDrivingFlags : uint64_t
{
	// Avoidance
	DF_StopForCars = BIT(0),
	DF_StopForPeds = BIT(1),
	DF_SwerveAroundAllCars = BIT(2),
	DF_SteerAroundStationaryCars = BIT(3),
	DF_SteerAroundPeds = BIT(4),
	DF_SteerAroundObjects = BIT(5),
	DF_DontSteerAroundPlayerPed = BIT(6),
	DF_StopAtLights = BIT(7),
	DF_GoOffRoadWhenAvoiding = BIT(8),

	DF_DriveIntoOncomingTraffic = BIT(9),
	DF_DriveInReverse = BIT(10),
	DF_UseWanderFallbackInsteadOfStraightLine = BIT(11),	// If pathfinding fails, cruise randomly instead of going on a straight line.
	DF_AvoidRestrictedAreas = BIT(12),

	// These only work on MISSION_CRUISE
	DF_PreventBackgroundPathfinding = BIT(13),
	DF_AdjustCruiseSpeedBasedOnRoadSpeed = BIT(14),

	DF_PreventJoinInRoadDirectionWhenMoving = BIT(15),
	DF_DontAvoidTarget = BIT(16),
	DF_TargetPositionOverridesEntity = BIT(17),

	DF_UseShortCutLinks = BIT(18),
	DF_ChangeLanesAroundObstructions = BIT(19),
	DF_AvoidTargetCoors = BIT(20),	//pathfind away from instead of towards m_target
	DF_UseSwitchedOffNodes = BIT(21),
	DF_PreferNavmeshRoute = BIT(22),

	// Only works for planes using MISSION_GOTO, will cause them to drive along the ground instead of fly
	DF_PlaneTaxiMode = BIT(23),

	DF_ForceStraightLine = BIT(24),	// if set, go on a straight line instead of following roads, etc
	DF_UseStringPullingAtJunctions = BIT(25),

	DF_AvoidAdverseConditions = BIT(26),	// Avoids "adverse conditions" (shocking events, etc.) when cruising.
	DF_AvoidTurns = BIT(27),	// Avoids turns when cruising.
	DF_ExtendRouteWithWanderResults = BIT(28),
	DF_AvoidHighways = BIT(29),
	DF_ForceJoinInRoadDirection = BIT(30),

	DF_DontTerminateTaskWhenAchieved = BIT(31),

	DF_LastFlag = DF_DontTerminateTaskWhenAchieved,

	DMode_StopForCars = DF_StopForCars | DF_StopForPeds | DF_SteerAroundObjects | DF_SteerAroundStationaryCars | DF_StopAtLights | DF_UseShortCutLinks | DF_ChangeLanesAroundObstructions,		// Obey lights too
	DMode_StopForCars_Strict = DF_StopForCars | DF_StopForPeds | DF_StopAtLights | DF_UseShortCutLinks,		// Doesn't deviate an inch.
	DMode_AvoidCars = DF_SwerveAroundAllCars | DF_SteerAroundObjects | DF_UseShortCutLinks | DF_ChangeLanesAroundObstructions | DF_StopForCars,
	DMode_AvoidCars_Reckless = DF_SwerveAroundAllCars | DF_SteerAroundObjects | DF_UseShortCutLinks | DF_ChangeLanesAroundObstructions,
	DMode_PloughThrough = DF_UseShortCutLinks,
	DMode_StopForCars_IgnoreLights = DF_StopForCars | DF_SteerAroundStationaryCars | DF_StopForPeds | DF_SteerAroundObjects | DF_UseShortCutLinks | DF_ChangeLanesAroundObstructions,
	DMode_AvoidCars_ObeyLights = DF_SwerveAroundAllCars | DF_StopAtLights | DF_SteerAroundObjects | DF_UseShortCutLinks | DF_ChangeLanesAroundObstructions | DF_StopForCars,
	DMode_AvoidCars_StopForPeds_ObeyLights = DF_SwerveAroundAllCars | DF_StopAtLights | DF_StopForPeds | DF_SteerAroundObjects | DF_UseShortCutLinks | DF_ChangeLanesAroundObstructions | DF_StopForCars,

	CUSTOM_Lawful = DMode_StopForCars,
	CUSTOM_Lawful_RunLights = CUSTOM_Lawful & ~DF_StopAtLights,
	CUSTOM_Aggressive = DF_SwerveAroundAllCars | DF_SteerAroundStationaryCars | DF_SteerAroundObjects | DF_SteerAroundPeds | DF_ChangeLanesAroundObstructions | DF_DriveIntoOncomingTraffic,
	CUSTOM_Backwards = CUSTOM_Lawful | DF_DriveInReverse,
};