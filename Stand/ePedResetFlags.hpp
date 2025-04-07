#pragma once

enum ePedResetFlags
{
	CPED_RESET_FLAG_FallenDown, // decides whether to ik rotate body to match ground angle
	CPED_RESET_FLAG_DontRenderThisFrame, // force ped to stop rendering this frame (for example - drive task can stop peds rendering inside buses)
	CPED_RESET_FLAG_IsDrowning,
	CPED_RESET_FLAG_PedHitWallLastFrame,
	CPED_RESET_FLAG_UsingMobilePhone,
	CPED_RESET_FLAG_BlockMovementAnims, // completely disable processing of on-foot movement anim blending for this frame
	CPED_RESET_FLAG_ZeroDesiredMoveBlendRatios, // zero out all inputs to movement system this frame, causing ped to stop moving
	CPED_RESET_FLAG_DontChangeMbrInSimpleMoveDoNothing, // If this is set, then CTaskSimpleMoveDoNothing will not reset desired moveblendratio to zero this frame.
	CPED_RESET_FLAG_FollowingRoute, // whether this ped is following a route of some sort - used to let simplest goto task pull the ped back onto their current route segment
	CPED_RESET_FLAG_TakingRouteSplineCorner, // whether the ped is cornering via a spline curve, which will take them off their route linesegment
	CPED_RESET_FLAG_Wandering,
	CPED_RESET_FLAG_ProcessPhysicsTasks, // do we need to call process physics for (main) tasks this frame
	CPED_RESET_FLAG_ProcessPreRender2, // do we need to call ProcessPreRender2 for tasks
	CPED_RESET_FLAG_SetLastMatrixDone,
	CPED_RESET_FLAG_FiringWeapon, // set when ped fires any weapon, so script can check the flag
	CPED_RESET_FLAG_SearchingForCover, // Set if the ped is likely to be searching for cover, used by the navmesh to load coverpoints around interested peds
	CPED_RESET_FLAG_KeepCoverPoint, // Set if the ped wants to keep their current cover point this frame, otherwise it gets released
	CPED_RESET_FLAG_IsClimbing, // If the ped is climbing, shimmying or on a ladder.  Stops ProcessProbes() from happening.
	CPED_RESET_FLAG_IsJumping, // is the ped jumping.
	CPED_RESET_FLAG_IsLanding, // is landing after being in the air
	CPED_RESET_FLAG_CullExtraFarAway, // flag Ai can set to make ped get culled further away (used for roadblock cops, crimials...)
	CPED_RESET_FLAG_DontActivateRagdollFromAnyPedImpactReset,
	CPED_RESET_FLAG_ForceScriptControlledRagdoll,
	CPED_RESET_FLAG_TaskUseKinematicPhysics, // For use by tasks - puts the ped into Kinematic physics mode. In this mode the ped will push other objects our of the way, but not be physically affected by them
	CPED_RESET_FLAG_TemporarilyBlockWeaponSwitching, // Stop weapon switch processing
	CPED_RESET_FLAG_DoNotClampFootIk, // Stop clamping the foot IK. Maybe the clamps should be parameters of the IK manager, but apparently that is to be refactored in future
	CPED_RESET_FLAG_MoveBlend_bFleeTaskRunning, // TASK_SMART_FLEE, or TASK_COMPLEX_LEAVE_CAR_AND_FLEE
	CPED_RESET_FLAG_IsAiming, // TASK_GUN or TASK_USE_COVER
	CPED_RESET_FLAG_MoveBlend_bTaskComplexGunRunning, // TASK_COMPLEX_GUN (only)
	CPED_RESET_FLAG_MoveBlend_bMeleeTaskRunning, // TASK_COMPLEX_MELEE
	CPED_RESET_FLAG_MoveBlend_bCopSearchTaskRunning, // TASK_COMPLEX_SEARCH_FOR_PED_ON_FOOT
	CPED_RESET_FLAG_PatrollingInVehicle, // ped is patrolling in a vehicle, likely meaning they are swat or police
	CPED_RESET_FLAG_RaiseVelocityChangeLimit,
	CPED_RESET_FLAG_DimTargetReticule,
	CPED_RESET_FLAG_IsWalkingRoundPlayer, // Whether this ped is walking around another ped (one frame latency)
	CPED_RESET_FLAG_GestureAnimsAllowed,
	CPED_RESET_FLAG_VisemeAnimsBlocked, // Blocks viseme anims from playing
	CPED_RESET_FLAG_AmbientAnimsBlocked, // Blocks new ambient idles from starting.
	CPED_RESET_FLAG_KnockedToTheFloorByPlayer,
	CPED_RESET_FLAG_RandomisePointsDuringNavigation,
	CPED_RESET_FLAG_Prevent180SkidTurns,
	CPED_RESET_FLAG_IsOnAssistedMovementRoute,
	CPED_RESET_FLAG_ApplyVelocityDirectly, // Should we apply the velocity directly to the physics collider or go through the force solver?
	CPED_RESET_FLAG_DisablePlayerLockon,
	CPED_RESET_FLAG_ResetMoveGroupAfterRagdoll, // If true, will reset the temp anim group when not ragdolling
	CPED_RESET_FLAG_DisablePedConstraints, // Allow the ped to rotate around freely
	CPED_RESET_FLAG_DisablePlayerJumping, // Disables player jumping if true. Reset in ResetPostPhysics, as it will be set via script
	CPED_RESET_FLAG_DisablePlayerVaulting, // Disables player vaulting/climbing if true.
	CPED_RESET_FLAG_DisableAsleepImpulse, // Disable the code that pushes peds which fall asleep in the air
	CPED_RESET_FLAG_ForcePostCameraAIUpdate,
	CPED_RESET_FLAG_ForcePostCameraAnimUpdate,
	CPED_RESET_FLAG_ePostCameraAnimUpdateUseZeroTimestep,
	CPED_RESET_FLAG_CollideWithGlassRagdoll,
	CPED_RESET_FLAG_CollideWithGlassWeapon,
	CPED_RESET_FLAG_SyncDesiredHeadingToCurrentHeading,
	CPED_RESET_FLAG_AllowUpdateIfNoCollisionLoaded,
	CPED_RESET_FLAG_InternalWalkingRndPlayer,
	CPED_RESET_FLAG_PlacingCharge, // Setting Bomb (firing weapon is set also)
	CPED_RESET_FLAG_ScriptDisableSecondaryAnimationTasks, // Disable upper body animtion tasks such as shove ped and open door anims
	CPED_RESET_FLAG_SearchingForClimb,
	CPED_RESET_FLAG_SearchingForDoors,
	CPED_RESET_FLAG_WanderingStoppedForOtherPed,
	CPED_RESET_FLAG_SupressGunfireEvents,
	CPED_RESET_FLAG_InfiniteStamina, // Currently just for mounts, but could be expanded to anything with stamina
	CPED_RESET_FLAG_BlockWeaponReactionsUnlessDead, //  Stops ragdoll and nm behaviors triggering from weapon damage unless the ped has died.
	CPED_RESET_FLAG_ForcePlayerFiring,
	CPED_RESET_FLAG_InCoverFacingLeft, // set when exiting the cover state saying if we are facing left
	CPED_RESET_FLAG_ForcePeekFromCover, // if set the ped will go into peeking if they are already in cover
	CPED_RESET_FLAG_NotAllowedToChangeCrouchState, // if set the ped will not be allowed to change their crouch state
	CPED_RESET_FLAG_ForcePedToStrafe, // forces a ped to strafe
	CPED_RESET_FLAG_ForceMeleeStrafingAnims, // forces a ped to use the melee strafing anims when strafing
	CPED_RESET_FLAG_HitPedWithWeapon, // set when damage is inflicted by this ped on another ped
	CPED_RESET_FLAG_ForcePedToUseScripCamHeading, // forces a ped to the scripted camera heading instead of gameplay
	CPED_RESET_FLAG_ProcessProbesWhenExtractingZ, // makes the capsule physics push the ped out of the ground even when extracting z
	CPED_RESET_FLAG_KeepDesiredCoverPoint, // should the ped keep their desired cover point this frame
	CPED_RESET_FLAG_HasProcessedCornering, // whether the ped has already processed slowing down for this corner
	CPED_RESET_FLAG_StandingOnForkliftForks, // Set when the ped standing capsule hits the forklift forks.
	CPED_RESET_FLAG_AimWeaponReactionRunning, // ped is running the reaction task this frame
	CPED_RESET_FLAG_InContactWithFoliage, // ped is in contact with GTA_FOLIAGE_TYPE bounds
	CPED_RESET_FLAG_ForceExplosionCollisions, // ped will always collide with explosions, even when collision is off
	CPED_RESET_FLAG_IgnoreTargetsCoverForLOS, // when checking LOS against targets this ped will ignore their cover (if the cover exists)
	CPED_RESET_FLAG_BlockAnimatedWeaponReactions, // Ped should not play animated damager reactions while this flag is set
	CPED_RESET_FLAG_DisablePedCapsule, // Removes the ped capsule from the physics simulation
	CPED_RESET_FLAG_DisableCrouchWhileInCover, // Force the crouch flag to return true while in cover.
	CPED_RESET_FLAG_IncreasedAvoidanceRadius, // Adds extra 2m onto the radius other peds use to avoid this ped during local steering.
	CPED_RESET_FLAG_UNUSED_REPLACE_ME,
	CPED_RESET_FLAG_ForceRunningSpeedForFragSmashing, // Can be set by designers to force the ped to smash more easily through frags.
	CPED_RESET_FLAG_EnableMoverAnimationWhileAttached, // While flagged, any mover animation will be applied to the offset of the attachment.
	CPED_RESET_FLAG_NoTimeDelayBeforeShot, // While flagged, The time delay before a player can fire after aiming is 0
	CPED_RESET_FLAG_SearchingForAutoVaultClimb, // inform climb code we are doing an autovault
	CPED_RESET_FLAG_ExtraLongWeaponRange, // Extends the range of a peds weapons to 250m
	CPED_RESET_FLAG_ForcePlayerToEnterVehicleThroughDirectDoorOnly, // Forces the player to only use direct access when entering vehicles
	CPED_RESET_FLAG_TaskCullExtraFarAway, // Can be set by ai tasks on the main task tree to disable a ped getting cull from a vehicle.
	CPED_RESET_FLAG_IsVaulting, // Set the entire time CTaskVault is running.
	CPED_RESET_FLAG_IsParachuting, // Set the entire time CTaskParachute is running.
	CPED_RESET_FLAG_SuppressSlowingForCorners, // If set this will prevent the ped from slowing down to take corners this frame
	CPED_RESET_FLAG_DisableProcessProbes, // Disables processing of probes.
	CPED_RESET_FLAG_DisablePlayerAutoVaulting,
	CPED_RESET_FLAG_DisableGaitReduction,
	CPED_RESET_FLAG_ExitVehicleTaskFinishedThisFrame,
	CPED_RESET_FLAG_RequiresLegIk,
	CPED_RESET_FLAG_JayWalking, // If set then the ped is jay walking and a vehicle is allowed to run him over.
	CPED_RESET_FLAG_UseBulletPenetration, // Ped will use bullet penetration code.
	CPED_RESET_FLAG_ForceAimAtHead, // Force all attackers to target the head of this ped
	CPED_RESET_FLAG_IsInStationaryScenario, // In a scenario and not moving
	CPED_RESET_FLAG_TemporarilyBlockWeaponEquipping, // Stop weapon equipping
	CPED_RESET_FLAG_CoverOutroRunning, // TASK_AIM_GUN_FROM_COVER_OUTRO
	CPED_RESET_FLAG_DisableSeeThroughChecksWhenTargeting, // Any targeting LoS checks will fail if any materials with 'see through' materials found.
	CPED_RESET_FLAG_PuttingOnHelmet, //  putting on helmet
	CPED_RESET_FLAG_AllowPullingPedOntoRoute, //  allows goto task to apply heading in order to pull a ped back onto their route
	CPED_RESET_FLAG_ApplyAnimatedVelocityWhilstAttached, // Allows attachment offsets to be updated from animated velocities
	CPED_RESET_FLAG_AICoverEntryRunning, // TASK_ENTER_COVER : State_PlayAIEntryAnim
	CPED_RESET_FLAG_ResponseAfterScenarioPanic, // The ped is entering threat response after panic exiting a scenario.
	CPED_RESET_FLAG_IsNearDoor, // Ped is near a non-vehicle door
	CPED_RESET_FLAG_DisableTorsoSolver,
	CPED_RESET_FLAG_PanicInVehicle,
	CPED_RESET_FLAG_DisableDynamicCapsuleRadius, // Turn off dynamic adjustments to ped capsules
	CPED_RESET_FLAG_IsRappelling, // Is currently in a rappel task
	CPED_RESET_FLAG_SkipReactInReactAndFlee, // When this ped goes to ThreatResponse, play the flee transition but not the reaction clip if fleeing.
	CPED_RESET_FLAG_CannotBeTargeted, // Will prevent this ped from being a part of any other peds target list
	CPED_RESET_FLAG_IsFalling, // Ped is in pure fall state (i.e. no parachuting, landing etc included
	CPED_RESET_FLAG_ForceInjuryAfterStunned, // Forces this ped to the injured state after being stunned
	CPED_RESET_FLAG_HurtThisFrame, // The ped has entered the hurt state this frame
	CPED_RESET_FLAG_BlockWeaponFire, // Prevent the ped from shooting a weapon
	CPED_RESET_FLAG_ExpandPedCapsuleFromSkeleton, // Set the ped capsule radius based on skeleton
	CPED_RESET_FLAG_DisableWeaponLaserSight, // Toggle the weapon laser sight off for this frame
	CPED_RESET_FLAG_PedExitedVehicleThisFrame, // Set when ped gets set out of vehicle
	CPED_RESET_FLAG_SearchingForDropDown, // Ped is seatching for drop down
	CPED_RESET_FLAG_UseTighterTurnSettings, // Ped should use tighter turn settings in human locomotion motion task
	CPED_RESET_FLAG_DisableArmSolver, // Disable the arm solver this frame
	CPED_RESET_FLAG_DisableHeadSolver,
	CPED_RESET_FLAG_DisableLegSolver,
	CPED_RESET_FLAG_DisableTorsoReactSolver,
	CPED_RESET_FLAG_ForcePreCameraAIUpdate,
	CPED_RESET_FLAG_TasksNeedProcessMoveSignalCalls, // Set when peds require calls to ProcessMoveSignals(), for AI timeslicing to work with Move
	CPED_RESET_FLAG_ShootFromGround,
	CPED_RESET_FLAG_NoCollisionMovementMode, // Set when a ped is moving in an area where collisions with fixed geometry are unlikely.  The ped's physics will not be forced to activate.
	CPED_RESET_FLAG_IsNearLaddder, // Ped is near top of a ladder
	CPED_RESET_FLAG_SkipAimingIdleIntro,
	CPED_RESET_FLAG_IgnoredByAutoOpenDoors, // Set this for a ped to be ignored by the auto opened doors when checking to see if the door should be opened.
	CPED_RESET_FLAG_BlockIkWeaponReactions, // Ped should not play Ik damager reactions while this flag is set
	CPED_RESET_FLAG_FirstPhysicsUpdate, // Ped was just created this frame
	CPED_RESET_FLAG_SpawnedThisFrameByAmbientPopulation, // Ped was spawned this frame
	CPED_RESET_FLAG_DisableRootSlopeFixupSolver,
	CPED_RESET_FLAG_SuspendInitiatedMeleeActions, // Temporarily suspend any melee actions this frame (does not include hit reactions). Use PCF_DisableMelee to turn it off completely
	CPED_RESET_FLAG_SuppressInAirEvent, // Prevents ped from getting the in air event the next frame
	CPED_RESET_FLAG_AllowTasksIncompatibleWithMotion, // If set, ped will skip CheckTasksAreCompatibleWithMotion
	CPED_RESET_FLAG_IsEnteringOrExitingVehicle, // TASK_ENTER_VEHICLE or TASK_EXIT_VEHICLE
	CPED_RESET_FLAG_PlayerOnHorse, // TASK_PLAYER_ON_HORSE
	CPED_RESET_FLAG_HasGunTaskWithAimingState, // Ped is running TASK_GUN and task's state is State_Aim
	CPED_RESET_FLAG_SuppressLethalMeleeActions, // This will suppress any melee action that is considered lethal (RA_IS_LETHAL, defined in action_table.meta)
	CPED_RESET_FLAG_InstantBlendToAimFromScript,
	CPED_RESET_FLAG_IsStillOnBicycle,
	CPED_RESET_FLAG_IsSittingAndCycling,
	CPED_RESET_FLAG_IsStandingAndCycling,
	CPED_RESET_FLAG_IsDoingCoverAimOutro,
	CPED_RESET_FLAG_ApplyCoverWeaponBlockingOffsets,
	CPED_RESET_FLAG_IsInLowCover,
	CPED_RESET_FLAG_AmbientIdleAndBaseAnimsBlocked, // Blocks ambient idles and base animations from playing.
	CPED_RESET_FLAG_UseAlternativeWhenBlock, // If set, ped will use alternative aiming/firing anims
	CPED_RESET_FLAG_ForceLowLodWaterCheck, // If set, the ped will always force probe for being in water when in low LOD mode.
	CPED_RESET_FLAG_MakeHeadInvisible, // If set, scale the head of the ped to 0.001
	CPED_RESET_FLAG_NoAutoRunWhenFiring, // Don't auto run when NoAutoRunWhenFiring is set.
	CPED_RESET_FLAG_PermitEventDuringScenarioExit, // Ignore certain aspects (FOV checks, etc) of AffectsPedCore() on some events while the ped is playing a scenario exit.
	CPED_RESET_FLAG_DisableSteeringAroundVehicles, //  Enables/disables the low-level steering behaviour around vehicles
	CPED_RESET_FLAG_DisableSteeringAroundPeds, //  Enables/disables the low-level steering behaviour around peds
	CPED_RESET_FLAG_DisableSteeringAroundObjects, //  Enables/disables the low-level steering behaviour around objects
	CPED_RESET_FLAG_DisableSteeringAroundNavMeshEdges, //  Enables/disables the low-level steering behaviour around nav mesh edges
	CPED_RESET_FLAG_WantsToEnterVehicleFromCover,
	CPED_RESET_FLAG_WantsToEnterCover,
	CPED_RESET_FLAG_WantsToEnterVehicleFromAiming,
	CPED_RESET_FLAG_CapsuleBeingPushedByVehicle,
	CPED_RESET_FLAG_DisableTakeOffParachutePack,
	CPED_RESET_FLAG_IsCallingPolice,
	CPED_RESET_FLAG_ForceCombatTaunt, // Forces a combat taunt for peds using the insult special ability.
	CPED_RESET_FLAG_IgnoreCombatTaunts,
	CPED_RESET_FLAG_SkipAiUpdateProcessControl, // True if we've already run this ped's AI and can skip it from within ProcessControl
	CPED_RESET_FLAG_OverridePhysics, // A reset flag that disables collision and gravity on the ped and drives entity positions and rotations directly, rather than going through the physics update.
	CPED_RESET_FLAG_WasPhysicsOverridden, // True if physics was overriden during the last update.
	CPED_RESET_FLAG_BlockWeaponHoldingAnims, // Block any onfoot weapon holding anims.
	CPED_RESET_FLAG_DisableMoveTaskHeadingAdjustments, // True if the ped's movement task should not adjust the heading of the ped.
	CPED_RESET_FLAG_DisableBodyLookSolver,
	CPED_RESET_FLAG_PreventAllMeleeTakedowns, // Will temporarily prevent any takedown from being performed on this ped
	CPED_RESET_FLAG_PreventFailedMeleeTakedowns, // Will temporarily prevent any failed takedowns from being performed on this ped
	CPED_RESET_FLAG_IsPedalling,
	CPED_RESET_FLAG_UseTighterAvoidanceSettings, // Ped should use tighter avoidance settings in navigation task
	CPED_RESET_FLAG_IsHigherPriorityClipControllingPed, // True if the active task on the main task tree is taking responsibility for the mover track
	CPED_RESET_FLAG_VehicleCrushingRagdoll, // Set to true if a vehicle is pressing downward on the ragdoll
	CPED_RESET_FLAG_OnActivationUpdate, // Ped was just activated this frame
	CPED_RESET_FLAG_ForceMotionStateLeaveDesiredMBR, // Set this to disable setting the desired move blendratio when forcing the motion state. Usefull for forcing a flee start, etc
	CPED_RESET_FLAG_DisableDropDowns, // Disable drop downs for this ped
	CPED_RESET_FLAG_InContactWithBIGFoliage, // ped is in contact with GTA_FOLIAGE_TYPE bounds that are large and the player can be hidden
	CPED_RESET_FLAG_DisableTakeOffScubaGear,
	CPED_RESET_FLAG_DisableCellphoneAnimations, // tells taskmobile phone to Blends out move network and prevents it from blending in
	CPED_RESET_FLAG_IsExitingVehicle, // TASK_EXIT_VEHICLE
	CPED_RESET_FLAG_DisableActionMode, // Disables combat anims for ped
	CPED_RESET_FLAG_EquippedWeaponChanged, // Equipped weapon changed this frame
	CPED_RESET_FLAG_TouchingOverhang, // Some part appears to be contrained downwards
	CPED_RESET_FLAG_TooSteepForPlayer, // We're standing on something flagged too steep for the player to stand on
	CPED_RESET_FLAG_BlockSecondaryAnim, // Block any secondary scripted task animations playing on this ped
	CPED_RESET_FLAG_IsInCombat, // This ped is running the combat task
	CPED_RESET_FLAG_UseHeadOrientationForPerception, // Will use the ped's head orientation for perception tests
	CPED_RESET_FLAG_IsDoingDriveby, // This ped is running a driveby gun or projectile task
	CPED_RESET_FLAG_IsEnteringCover, // This ped is running a cover entry task
	CPED_RESET_FLAG_ForceMovementScannerCheck, // Set to make CStaticMovementScanner::Scan() check for collisions as if we were moving
	CPED_RESET_FLAG_DisableJumpRagdollOnCollision, // If true, the player will no longer auto-ragdoll when colliding with objects, peds, etc. while jumping
	CPED_RESET_FLAG_IsBeingMeleeHomedByPlayer, // Set on the target ped in melee if the player is homing towards them
	CPED_RESET_FLAG_ShouldLaunchBicycleThisFrame, // This ped should launch the bicycle this frame
	CPED_RESET_FLAG_CanDoBicycleWheelie, // This ped can do a bicycle wheelie
	CPED_RESET_FLAG_ForceProcessPhysicsUpdateEachSimStep, // If true, ProcessPhysics() will execute completely for each physics simulation step
	CPED_RESET_FLAG_DisablePedCapsuleMapCollision, // Disables collision between the ped capsule and the map (usefull in cases where the entity position is being tightly controlled outside of physics, e.g. by an animation)
	CPED_RESET_FLAG_DisableSeatShuffleDueToInjuredDriver, // If true, motion in vehicle task won't shuffle to the driver seat just because the driver is injured
	CPED_RESET_FLAG_DisableParachuting,
	CPED_RESET_FLAG_ProcessPostMovement, // do we need to call ProcessPostMovement for tasks
	CPED_RESET_FLAG_ProcessPostCamera, // do we need to call ProcessPostCamera for tasks
	CPED_RESET_FLAG_ProcessPostPreRender, // do we need to call ProcessPostPreRender for tasks
	CPED_RESET_FLAG_PreventBicycleFromLeaningOver,
	CPED_RESET_FLAG_KeepParachutePackOnAfterTeleport,
	CPED_RESET_FLAG_DontRaiseFistsWhenLockedOn,
	CPED_RESET_FLAG_PreferMeleeBodyIkHitReaction, // This will prefer all melee hit reactions to use body ik hit reactions if ragdoll is not selected
	CPED_RESET_FLAG_ProcessPhysicsTasksMotion, // do we need to call process physics for motion tasks this frame
	CPED_RESET_FLAG_ProcessPhysicsTasksMovement, // do we need to call process physics for movement tasks this frame
	CPED_RESET_FLAG_DisableFriendlyGunReactAudio, // If set, disables friendly responses to gunshots/being aimed at.
	CPED_RESET_FLAG_DisableAgitationTriggers,
	CPED_RESET_FLAG_ForceForwardTransitionInReactAndFlee, // If set, force CTaskReactAndFlee to use a forward facing flee transition
	CPED_RESET_FLAG_IsEnteringVehicle, // TASK_ENTER_VEHICLE
	CPED_RESET_FLAG_DoNotSkipNavMeshTrackerUpdate, // If set we will not allow the NavMeshTracker update to be skipped this frame.
	CPED_RESET_FLAG_RagdollOnVehicle, // Set to true when the ragdoll is lying on top of a vehicle (note- hands, feet, forearms and shins are not included in the test)
	CPED_RESET_FLAG_BlockRagdollActivationInVehicle,
	CPED_RESET_FLAG_DisableNMForRiverRapids, // If set, disable NM reactions to fast moving water
	CPED_RESET_FLAG_IsInWrithe, // If set, the ped is on the ground writhing and might start shooting from ground
	CPED_RESET_FLAG_PreventGoingIntoStillInVehicleState, // If set, the ped will not go into the still in vehicle pose
	CPED_RESET_FLAG_UseFastEnterExitVehicleRates, // If set, the ped will get in and out of vehicles faster
	CPED_RESET_FLAG_DisableGroundAttachment, // If set, the ped won't attach to a ground physical when his physics disables (useful for cutscenes, etc)
	CPED_RESET_FLAG_DisableAgitation,
	CPED_RESET_FLAG_DisableTalk,
	CPED_RESET_FLAG_InterruptedToQuickStartEngine,
	CPED_RESET_FLAG_PedEnteredFromLeftEntry,
	CPED_RESET_FLAG_IsDiving,
	CPED_RESET_FLAG_DisableVehicleImpacts,
	CPED_RESET_FLAG_DeepVehicleImpacts,
	CPED_RESET_FLAG_DisablePedCapsuleControl,
	CPED_RESET_FLAG_UseProbeSlopeStairsDetection,
	CPED_RESET_FLAG_DisableVehicleDamageReactions,
	CPED_RESET_FLAG_DisablePotentialBlastReactions,
	CPED_RESET_FLAG_OnlyAllowLeftArmDoorIk,
	CPED_RESET_FLAG_OnlyAllowRightArmDoorIk,
	CPED_RESET_FLAG_ForceProcessPedStandingUpdateEachSimStep, // When set, ProcessPedStanding will get called for each physics step.
	CPED_RESET_FLAG_DisableFlashLight, // When set, the flash light on a Ai weapon will be turned off.
	CPED_RESET_FLAG_DoingCombatRoll, // When set, the ped is doing a combat roll
	CPED_RESET_FLAG_DisableBodyRecoilSolver,
	CPED_RESET_FLAG_CanAbortExitForInAirEvent, // When set, the ped can abort the exit vehicle anim to go into fall
	CPED_RESET_FLAG_DisableSprintDamage,
	CPED_RESET_FLAG_ForceEnableFlashLightForAI, // When set, the ai ped will enable their flash light
	CPED_RESET_FLAG_IsDoingCoverAimIntro,
	CPED_RESET_FLAG_IsAimingFromCover,
	CPED_RESET_FLAG_WaitingForCompletedPathRequest, // This ped is waiting for a path request which is now complete, so their tasks must be updated this frame
	CPED_RESET_FLAG_DisableCombatAudio,
	CPED_RESET_FLAG_DisableCoverAudio,
	CPED_RESET_FLAG_PreventBikeFromLeaning,
	CPED_RESET_FLAG_InCoverTaskActive,
	CPED_RESET_FLAG_EnableSteepSlopePrevention, // Pushes the ped through the same steep slope tolerances in TaskMotionBase that the player encounters
	CPED_RESET_FLAG_InsideEnclosedSearchRegion,
	CPED_RESET_FLAG_JumpingOutOfVehicle,
	CPED_RESET_FLAG_IsTuckedOnBicycleThisFrame,
	CPED_RESET_FLAG_ProcessPostMovementTimeSliced, // Parallel flag to ProcessPostMovement, except this is reset in PreTask(), meaning it stays consistent across timeslicing.
	CPED_RESET_FLAG_EnablePressAndReleaseDives,
	CPED_RESET_FLAG_OnlyExitVehicleOnButtonRelease,
	CPED_RESET_FLAG_IsGoingToStandOnExitedVehicle,
	CPED_RESET_FLAG_BlockRagdollFromVehicleFallOff,
	CPED_RESET_FLAG_DisableTorsoVehicleSolver,
	CPED_RESET_FLAG_IsExitingUpsideDownVehicle,
	CPED_RESET_FLAG_IsExitingOnsideVehicle,
	CPED_RESET_FLAG_IsExactStopping,
	CPED_RESET_FLAG_IsExactStopSettling,
	CPED_RESET_FLAG_IsTrainCrushingRagdoll,
	CPED_RESET_FLAG_OverrideHairScale, // Scales the ped's hair down to the specified value
	CPED_RESET_FLAG_ConsiderAsPlayerCoverThreatWithoutLOS, // Considered as a threat as part of player cover search even if they can't see the player
	CPED_RESET_FLAG_BlockCustomAIEntryAnims,
	CPED_RESET_FLAG_IgnoreVehicleEntryCollisionTests,
	CPED_RESET_FLAG_StreamActionModeAnimsIfDisabled, // Stream action mode anims even if action mode is disabled
	CPED_RESET_FLAG_ForceUpdateRagdollMatrix, // Ped requires ragdoll matrix update next frame.
	CPED_RESET_FLAG_PreventGoingIntoShuntInVehicleState, // If set, the ped will not go into the shunt in vehicle pose
	CPED_RESET_FLAG_DisableIndependentMoverFrame,
	CPED_RESET_FLAG_DoingDrivebyOutro,
	CPED_RESET_FLAG_BeingElectrocuted,
	CPED_RESET_FLAG_DisableUnarmedDrivebys,
	CPED_RESET_FLAG_TalkingToPlayer,
	CPED_RESET_FLAG_DontActivateRagdollFromPlayerPedImpactReset, // Block ragdoll activations from animated player bumps
	CPED_RESET_FLAG_DontActivateRagdollFromAiRagdollImpactReset, // Block ragdoll activations from collisions with an ai ragdoll
	CPED_RESET_FLAG_DontActivateRagdollFromPlayerRagdollImpactReset, // Block ragdoll activations from collisions with a player ragdoll
	CPED_RESET_FLAG_DisableVisemeBodyAdditive, // If set, prevents visemes from playing any additive body animations
	CPED_RESET_FLAG_CapsuleBeingPushedByPlayerCapsule, // Set when the player capsule is pushing up against this peds capsule
	CPED_RESET_FLAG_ForceActionMode,
	CPED_RESET_FLAG_ForceUnarmedActionMode,
	CPED_RESET_FLAG_UsingMoverExtraction, // Set when the players capsule is getting repositioned each frame relative to an anim and origin
	CPED_RESET_FLAG_BeingJacked,
	CPED_RESET_FLAG_EnableVoiceDrivenMouthMovement, // If set, turn on the voice driven mouth movement
	CPED_RESET_FLAG_IsReloading,
	CPED_RESET_FLAG_UseTighterEnterVehicleSettings, // Ped should use tighter (shorter) settings for entering vehicles
	CPED_RESET_FLAG_InRaceMode, // Set when the player is in the race mode.
	CPED_RESET_FLAG_DisableAmbientMeleeMoves, // Disable ambient (initial) melee moves
	CPED_RESET_FLAG_ForceBuoyancyProcessingIfAsleep,
	CPED_RESET_FLAG_AllowSpecialAbilityInVehicle, // Allows the player to trigger the special ability while in a vehicle
	CPED_RESET_FLAG_DisableInVehicleActions, // Prevents ped from doing in vehicle actions such as starting engine, hotwiring, closing door etc.
	CPED_RESET_FLAG_ForceInstantSteeringWheelIkBlendIn, // Forces ped to blend in steering wheel ik instantly rather than over time.
	CPED_RESET_FLAG_IgnoreThreatEngagePlayerCoverBonus, // Ignores the bonus score for selecting cover that the player can engage the enemy at
	CPED_RESET_FLAG_Block180Turns, // Blocks triggering of 180 turns in human locomotion this frame.
	CPED_RESET_FLAG_DontCloseVehicleDoor, // Prevents the ped from closing the vehicle door of the car they're in
	CPED_RESET_FLAG_SkipExplosionOcclusion, // Map collision will not block this ped from being hit by explosions
	CPED_RESET_FLAG_ProcessPhysicsTasksTimeSliced, // Parallel flag to ProcessPhysicsTasks, except this is reset in PreTask(), meaning it stays consistent across timeslicing.
	CPED_RESET_FLAG_MeleeStrikeAgainstNonPed, // Set when the ped has performed a melee strike and hit any non ped material
	CPED_RESET_FLAG_IgnoreNavigationForDoorArmIK, // We will not attempt to walk around doors when using arm IK
	CPED_RESET_FLAG_DisableAimingWhileParachuting, // Disable aiming while parachuting
	CPED_RESET_FLAG_DisablePedCollisionWithPedEvent, // Disable hit reaction due to colliding with a ped
	CPED_RESET_FLAG_IgnoreVelocityWhenClosingVehicleDoor, // Will ignore the vehicle speed threshold and close the door anyway
	CPED_RESET_FLAG_SkipOnFootIdleIntro,
	CPED_RESET_FLAG_DontWalkRoundObjects, // Don't walk round objects that we collide with whilst moving
	CPED_RESET_FLAG_DisablePedEnteredMyVehicleEvents,
	CPED_RESET_FLAG_CancelLeftHandGripIk, // Call ProcessLeftHandGripIk() to cancel left hand grip IK, at the right time of the frame
	CPED_RESET_FLAG_ResetMovementStaticCounter, // Will keep reset the static counter when this is set
	CPED_RESET_FLAG_DisableInVehiclePedVariationBlocking, // Will allow ped variations to be rendered in vehicles, even if marked otherwise
	CPED_RESET_FLAG_ReduceEffectOfVehicleRamControlLoss, // When on a mission this reset flag will slightly reduce the amount of time the player loses control of their vehicle when hit by an AI ped
	CPED_RESET_FLAG_DisablePlayerMeleeFriendlyAttacks, // Another flag to disable friendly attack from the player. Set on the opponent you would like it to be disabled on.
	CPED_RESET_FLAG_MotionPedDoPostMovementIndependentMover,
	CPED_RESET_FLAG_IsMeleeTargetUnreachable, // Set when the melee target has been deemed unreachable (AI only)
	CPED_RESET_FLAG_DisableAutoForceOutWhenBlowingUpCar,
	CPED_RESET_FLAG_ThrowingProjectile,
	CPED_RESET_FLAG_OverrideHairScaleLarger, // Scales the ped's hair up to the specified value
	CPED_RESET_FLAG_DisableDustOffAnims, // Disable amient dust off animations
	CPED_RESET_FLAG_DisableMeleeHitReactions, // This ped will refrain from ever performing a melee hit reaction
	CPED_RESET_FLAG_VisemeAnimsAudioBlocked, // Blocks viseme anims audio from playing
	CPED_RESET_FLAG_AllowHeadPropInVehicle, // This overrides PV_FLAG_NOT_IN_CAR set on any head prop and stops it from being removed when getting into the vehicle
	CPED_RESET_FLAG_IsInVehicleChase,
	CPED_RESET_FLAG_DontQuitMotionAiming,
	CPED_RESET_FLAG_SetLastBoundMatricesDone, // Ensure that the last bound matrices are only updated once per frame
	CPED_RESET_FLAG_PreserveAnimatedAngularVelocity, // Don't allow the locomotion task to adjust angular velocity coming from animation
	CPED_RESET_FLAG_OpenDoorArmIK, // Set if the ped should enable open door arm IK
	CPED_RESET_FLAG_UseTighterTurnSettingsForScript, // Script set flag, to force use of tighter turn settings in locomotion task
	CPED_RESET_FLAG_ForcePreCameraProcessExternallyDrivenDOFs, // Set if the ped should process externally driven dofs before the pre-camera ai update
	CPED_RESET_FLAG_LadderBlockingMovement, // Ped is waiting for ladder and blocking movement to prevent falling off
	CPED_RESET_FLAG_DisableVoiceDrivenMouthMovement, // If set, turn off the voice driven mouth movement (overrides EnableVoiceDrivenMouthMovement)
	CPED_RESET_FLAG_SteerIntoSkids, // If set, steer into skids while driving
	CPED_RESET_FLAG_AllowOpenDoorIkBeforeFullMovement, // When set, code will ignore the logic that requires the ped to be in CTaskHumanLocomotion::State_Moving
	CPED_RESET_FLAG_AllowHomingMissileLockOnInVehicle, // When set, code will bypass rel settings and allow a homing lock on to this ped when they are in a vehicle
	CPED_RESET_FLAG_AllowCloneForcePostCameraAIUpdate,
	CPED_RESET_FLAG_DisableHighHeels, // Force the high heels DOF to be 0"
	CPED_RESET_FLAG_BreakTargetLock, // Force lock on to break for this ped"
	CPED_RESET_FLAG_DontUseSprintEnergy, // Player does not get tired when sprinting"
	CPED_RESET_FLAG_DontChangeHorseMbr, // Similar to CPED_RESET_FLAG_DontChangeMbrInSimpleMoveDoNothing,, but for horses.
	CPED_RESET_FLAG_DisableMaterialCollisionDamage, // Don't be damaged by touching dangerous material (e.g. electic generator)
	CPED_RESET_FLAG_DisableMPFriendlyLockon, // Don't target friendly players in MP
	CPED_RESET_FLAG_DisableMPFriendlyLethalMeleeActions, // Don't melee kill friendly players in MP
	CPED_RESET_FLAG_IfLeaderStopsSeekCover, // If our leader stops, try and seek cover if we can
	CPED_RESET_FLAG_ProcessPostPreRenderAfterAttachments, // do we need to call CPED_RESET_FLAG_ProcessPostPreRenderAfterAttachments, for tasks
	CPED_RESET_FLAG_DoDamageCoughFacial,
	CPED_RESET_FLAG_IsUsingJetpack, // Is ped currently using jetpack. e.g. in air
	CPED_RESET_FLAG_UseInteriorCapsuleSettings, // Use Interior capsule settings
	CPED_RESET_FLAG_IsClosingVehicleDoor, // Ped is closing a vehicle door
	CPED_RESET_FLAG_DisableIdleExtraHeadingChange, // Disable lerping the ped towards the desired heading in the locomotion idle
	CPED_RESET_FLAG_OnlySelectVehicleWeapons, // Only allows vehicle weapons to be selected in CPedWeaponSelector::SelectWeapon
	CPED_RESET_FLAG_IsWarpingIntoVehicleMP, // Set in CTaskEnterVehicle::SetPedInSeat_OnEnter if ped is warping into a vehicle in multiplayer
	CPED_RESET_FLAG_RemoveHelmet, // Forces a ped to remove its helmet.
	CPED_RESET_FLAG_IsRemovingHelmet, // Returns true if ped is removing its helmet.
	CPED_RESET_FLAG_GestureAnimsBlockedFromScript,
	CPED_RESET_FLAG_NeverRagdoll, // Disable all attempts by this ped to ragdoll.
	CPED_RESET_FLAG_DisableWallHitAnimation, // Disable stuck wall hit animation for the ped this frame.
	CPED_RESET_FLAG_PlayAgitatedAnimsInVehicle, // Play agitated anims in vehicle - overrides normal sit idle
	CPED_RESET_FLAG_IsSeatShuffling, // Returns true if ped is shuffling seat.
	CPED_RESET_FLAG_IsThrowingProjectileWhileAiming, // True if we are running TASK_AIM_AND_THROW_PROJECTILE as a subtask of TASK_AIM_GUN_ON_FOOT.
	CPED_RESET_FLAG_DisableProjectileThrowsWhileAimingGun, // Set by script command DISABLE_PLAYER_THROW_GRENADE_WHILE_USING_GUN.
	CPED_RESET_FLAG_AllowControlRadioInAnySeatInMP, // Allows ped in any seat to control radio in multiplayer.
	CPED_RESET_FLAG_DisableSpycarTransformation, // Blocks ped from manually transforming spycar to/from car/sub modes.
	CPED_RESET_FLAG_BlockQuadLocomotionIdleTurns, // Prevent CTaskQuadLocomotion from blending in idle turns, regardless of desired/currenting heading differential.
	CPED_RESET_FLAG_BlockHeadbobbingToRadio, // Blocks ped from headbobbing to radio music in vehicles.
	CPED_RESET_FLAG_PlayFPSIdleFidgets, // Allows us to load and play idle fidgets in TaskMotionAiming
	CPED_RESET_FLAG_ForceExtraLongBlendInForPedSkipIdleCoverTransition, // When putting a ped directly into cover, the ped will blend in the new cover anims slowly to prevent a pose pop
	CPED_RESET_FLAG_BlendingOutFPSIdleFidgets, // True if FPS idle fidgets are blending out
	CPED_RESET_FLAG_DisableMotionBaseVelocityOverride,
	CPED_RESET_FLAG_FPSSwimUseSwimMotionTask, // Set to true when we are pressing forward on the left stick in FPS mode so we switch from Aiming to Swimmimg/Diving motion tasks
	CPED_RESET_FLAG_FPSSwimUseAimingMotionTask, // Set to true when we are strafing in water in FPS mode so we use the motion aiming task
	CPED_RESET_FLAG_FiringWeaponWhenReady, // set when ped has decided to fire weapon when ready, used in FPS mode   
	CPED_RESET_FLAG_IsBlindFiring, // True if the blind fire task is running
	CPED_RESET_FLAG_IsPeekingFromCover, // True if the ped is peeking in cover
	CPED_RESET_FLAG_TaskSkipProcessPreComputeImpacts, // True to bail out of ProcessPreComputeImpacts
	CPED_RESET_FLAG_DisableAssistedAimLockon, // Don't ever try to lock on to this ped with cinematic aim
	CPED_RESET_FLAG_FPSAllowAimIKForThrownProjectile, // To control enabling of FPS aim IK while using a projectile until it is ready
	CPED_RESET_FLAG_TriggerRoadRageAnim,
	CPED_RESET_FLAG_ForcePreCameraAiAnimUpdateIfFirstPerson, // Force a pre camera ai and animation update if the ped is the first person camera target during the pre camera update
	CPED_RESET_FLAG_NoCollisionDamageFromOtherPeds, // Any ped this is set on won't register damage from collisions against other peds."
	CPED_RESET_FLAG_BlockCameraSwitching, // Block camera view mode switching."
	CPED_RESET_FLAG_NeverDieFromCapsuleRagdollSettings, // Negate the capsule's preference for ragdoll triggering death on this ped.
	CPED_RESET_FLAG_InContactWithDeepSurface, // ped is in contact with GTA_DEEP_SURFACE_TYPE bounds
	CPED_RESET_FLAG_DontSuppressUseNavMeshToNavigateToVehicleDoorWhenVehicleInWater,
	CPED_RESET_FLAG_IncludePedReferenceVelocityWhenFiringProjectiles, // Add on the ped's velocity to the projectile's initial velocity.
	CPED_RESET_FLAG_IsDoingCoverOutroToPeek, //  
	CPED_RESET_FLAG_InstantBlendToAimNoSettle,
	CPED_RESET_FLAG_ForcePreCameraAnimUpdate, // Force a pre camera animation update if the ped is the first person camera target during the pre camera update
	CPED_RESET_FLAG_DisableHelmetCullFPS, // Disables PV_FLAG_HIDE_IN_FIRST_PERSON from culling the prop in CPedPropsMgr::RenderPropsInternal
	CPED_RESET_FLAG_ShouldIgnoreCoverAutoHeadingCorrection, // "
	CPED_RESET_FLAG_DisableReticuleInCoverThisFrame,
	CPED_RESET_FLAG_ForceScriptedCameraLowCoverAngleWhenEnteringCover,
	CPED_RESET_FLAG_DisableCameraConstraintFallBackThisFrame,
	CPED_RESET_FLAG_DisableFPSArmIK, // Disables FPS arm IK in CTaskPlayerOnFoot::IsStateValidForFPSIK 
	CPED_RESET_FLAG_DisableRightArmIKInCoverOutroFPS, // Turn off right arm IK during cover outros in FPS mode when set
	CPED_RESET_FLAG_DoFPSSprintBreakOut,
	CPED_RESET_FLAG_DoFPSJumpBreakOut,
	CPED_RESET_FLAG_IsExitingCover,
	CPED_RESET_FLAG_WeaponBlockedInFPSMode, // True if running CTaskWeaponBlocked in FPS mode
	CPED_RESET_FLAG_PoVCameraConstrained,
	CPED_RESET_FLAG_ScriptClearingPedTasks, // Set in CommandClearPedTasksImmediately in commands_task.cpp
	CPED_RESET_FLAG_WasFPSJumpingWithProjectile, // ped was jumping on foot with projectile in hand
	CPED_RESET_FLAG_DisableMeleeWeaponSelection,
	CPED_RESET_FLAG_WaypointPlaybackSlowMoreForCorners, // slow for corners more aggressively for waypoint playback
	CPED_RESET_FLAG_FPSPlacingProjectile, // True while placing a projectile in FPS mode"
	CPED_RESET_FLAG_UseBulletPenetrationForGlass, // Ped will use bullet penetration code when glass material is hit.
	CPED_RESET_FLAG_FPSPlantingBombOnFloor, // Doing a floor plant with a bomb in FPS mode"
	CPED_RESET_FLAG_ForceSkipFPSAimIntro, // don't do FPS Aim intro
	CPED_RESET_FLAG_CanBePinnedByFriendlyBullets, // If set on a ped then they are allowed to be pinned by bullets from friendly peds
	CPED_RESET_FLAG_DisableLeftArmIKInCoverOutroFPS, // Turn off left arm IK during cover outros in FPS mode when set
	CPED_RESET_FLAG_DisableSpikeStripRoadBlocks, // Blocks road blocks with spike strips from spawning
	CPED_RESET_FLAG_SkipFPSUnHolsterTransition, // skip aim unholster transition
	CPED_RESET_FLAG_PutDownHelmetFX, // trigger the put down helmet fx
	CPED_RESET_FLAG_IsLowerPriorityMeleeTarget, // Peds marked with this flag will only be able to be hit by the player if the player explicitly presses the melee button
	CPED_RESET_FLAG_ForceScanForEventsThisFrame, // disable timeslicing of event scanning
	CPED_RESET_FLAG_StartProjectileTaskWithPrimingDisabled, // Set this flag to disable priming when the projectile task starts up until the attack trigger is released and pressed again
	CPED_RESET_FLAG_CheckFPSSwitchInCameraUpdate, // Set if we want to perform a second AI/anim update when switching between first person/third person
	CPED_RESET_FLAG_ForceAutoEquipHelmetsInAicraft, // Force ped to auto-equip a helmet om entering aircraft. Overrides PCF_DisableAutoEquipHelmetsInAicraft (set in the interaction menu)
	CPED_RESET_FLAG_BlockRemotePlayerRecording, // Flag used by replay editor to disable recording specified remote players (for url:bugstar:2218297).
	CPED_RESET_FLAG_InflictedDamageThisFrame, // Inflicted damage this frame
	CPED_RESET_FLAG_UseFirstPersonVehicleAnimsIfFPSCamNotDominant, // allow FPS vehicle anims even if FPS camera isn't dominant
	CPED_RESET_FLAG_ForceIntoStandPoseOnJetski, // puts the ped in a standing pose on the jetski
	CPED_RESET_FLAG_InAirDefenceSphere, // Ped is located inside an air defence sphere.
	CPED_RESET_FLAG_SuppressTakedownMeleeActions, // This will suppress all takedown melee actions (RA_IS_TAKEDOWN or RA_IS_STEALTH_KILL, defined in action_table.meta)
	CPED_RESET_FLAG_InvertLookAroundControls, // Inverts lookaround controls (right stick / mouse) for this player, for this frame.
	CPED_RESET_FLAG_IgnoreCombatManager, // Allows attacking ped to engage another entity without waiting for its turn (if there's multiple attackers).
	CPED_RESET_FLAG_UseBlendedCamerasOnUpdateFpsCameraRelativeMatrix, // Check if there is an active camera blending and use the blended camera frame when compute the FPS camera relative matrix.
	CPED_RESET_FLAG_ForceMeleeCounter, // Forces the ped to perform a dodge and a counter move if it's attacked.
	CPED_RESET_FLAG_WasHitByVehicleMelee, // Indicates that ped was hit by vehicle melee attack.
	CPED_RESET_FLAG_SuppressNavmeshForEnterVehicleTask, // Dont allow ped to use navmesh when navigating in TaskEnterVehicle
	CPED_RESET_FLAG_DisableShallowWaterBikeJumpOutThisFrame,
	CPED_RESET_FLAG_DisablePlayerCombatRoll, // Disables player combat rolling.
	CPED_RESET_FLAG_IgnoreDetachSafePositionCheck, // Will ignore safe position check on detaching the ped
	CPED_RESET_FLAG_DisableEasyLadderConditions, // Prevents the more forgiving MP ladder detection settings from being used, and forces SP settings.
	CPED_RESET_FLAG_PlayerIgnoresScenarioSpawnRestrictions, // Makes local player ignore certain scenario spawn restrictions on scenarios that respect this flag
	CPED_RESET_FLAG_UsingDrone, // Indicates player is using Drone from Battle DLC
	CPED_RESET_FLAG_ForceWantedLevelWhenKilled, // Will force the player that killed this ped to get wanted level, even if he wouldnt otherwise
	CPED_RESET_FLAG_UseScriptedWeaponFirePosition, // Will use scripted firing position on the clones of this ped on other machines
	CPED_RESET_FLAG_EnableCollisionOnNetworkCloneWhenFixed, // Enable collision on player ped network clones when physics is fixed
	CPED_RESET_FLAG_UseExtendedRagdollCollisionCalculator, // Use extended logic for determining damage instigator for ragdoll collisions
	CPED_RESET_FLAG_PreventLockonToFriendlyPlayers, // Prevent the player locking on to friendly players
	CPED_RESET_FLAG_OnlyAbortScriptedAnimOnMovementByInput, // Modifies AF_ABORT_ON_PED_MOVEMENT to only trigger an abort when movement would be caused by player input
	CPED_RESET_FLAG_PreventAllStealthKills, // Prevents stealth take downs from being preformed on a ped
	CPED_RESET_FLAG_BlockFallTaskFromExplosionDamage, // Prevent peds from entering a fall task if affected by explosion damage
	CPED_RESET_FLAG_AllowPedRearEntry, // Mimics the behaviour like with boss peds by holding the button for entering the rear seats
};
