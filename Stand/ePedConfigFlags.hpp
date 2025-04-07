#pragma once

enum ePedConfigFlags
{
	CPED_CONFIG_FLAG_CreatedByFactory,
	CPED_CONFIG_FLAG_CanBeShotInVehicle, //  script can stop peds being shot when driving vehicle (including exposed ones like bikes)
	CPED_CONFIG_FLAG_NoCriticalHits, //  ped cannot be killed by a single bullet
	CPED_CONFIG_FLAG_DrownsInWater, //  does this ped drown or swim?
	CPED_CONFIG_FLAG_DrownsInSinkingVehicle, //  does this ped take damage whilst in a sinking vehicle?
	CPED_CONFIG_FLAG_DiesInstantlyWhenSwimming, //  Ped cannot swim and will die as soon as they are required to swim
	CPED_CONFIG_FLAG_HasBulletProofVest, //  is this ped wearing a bullet proof vest
	CPED_CONFIG_FLAG_UpperBodyDamageAnimsOnly, //  force ped to play only upper body damage anims from weapons
	CPED_CONFIG_FLAG_NeverFallOffSkis, //  Ped will never fall over on skis
	CPED_CONFIG_FLAG_NeverEverTargetThisPed, //  script control over player targeting
	CPED_CONFIG_FLAG_ThisPedIsATargetPriority, //  script control over player targeting
	CPED_CONFIG_FLAG_TargettableWithNoLos, //  script control over player targeting
	CPED_CONFIG_FLAG_DoesntListenToPlayerGroupCommands, //  script can set this so ped will be in players group but not reacting to commands
	CPED_CONFIG_FLAG_NeverLeavesGroup,
	CPED_CONFIG_FLAG_DoesntDropWeaponsWhenDead, //  script command
	CPED_CONFIG_FLAG_SetDelayedWeaponAsCurrent, //  when the peds delayed weapon finally loads, set it as the peds current weapon
	CPED_CONFIG_FLAG_KeepTasksAfterCleanUp, //  If true ped will carry on with task even after script ped cleanup
	CPED_CONFIG_FLAG_BlockNonTemporaryEvents, // ! Set to block any events that might interrupt the currently running tasks.
	CPED_CONFIG_FLAG_HasAScriptBrain,
	CPED_CONFIG_FLAG_WaitingForScriptBrainToLoad,
	CPED_CONFIG_FLAG_AllowMedicsToReviveMe, //  never allow this ped to be revived
	CPED_CONFIG_FLAG_MoneyHasBeenGivenByScript, //  script can give specific amount of money to ped (script peds don't drop any money by default)
	CPED_CONFIG_FLAG_NotAllowedToCrouch, //  Is this ped allowed to crouch at all?
	CPED_CONFIG_FLAG_DeathPickupsPersist, //  script command to control what type of pickups are created when ped dies
	CPED_CONFIG_FLAG_IgnoreSeenMelee, // script command so ped doesn't stop to watch fights
	CPED_CONFIG_FLAG_ForceDieIfInjured, // script command so missions peds die if injured
	CPED_CONFIG_FLAG_DontDragMeOutCar, //  force this ped can't be carjacked
	CPED_CONFIG_FLAG_StayInCarOnJack, //  script sets this to keep peds in car when the player steals it
	CPED_CONFIG_FLAG_ForceDieInCar, //  don't fall out car if killed
	CPED_CONFIG_FLAG_GetOutUndriveableVehicle, //  script can stop peds automatically getting out of vehicle when it's upside down or undrivable(for races and stuff)
	CPED_CONFIG_FLAG_WillRemainOnBoatAfterMissionEnds, //  script can stop peds automatically leaving boats when they become random chars, after a script quits
	CPED_CONFIG_FLAG_DontStoreAsPersistent, //  Some peds (like mission peds) should not be stored as persistent.
	CPED_CONFIG_FLAG_WillFlyThroughWindscreen,
	CPED_CONFIG_FLAG_DieWhenRagdoll,
	CPED_CONFIG_FLAG_HasHelmet, //  currently has helmet equipped?
	CPED_CONFIG_FLAG_UseHelmet, //  will the ped try to put on their helmet?
	CPED_CONFIG_FLAG_DontTakeOffHelmet, //  the ped will not take off their helmet (if equipped) while this is set
	CPED_CONFIG_FLAG_HideInCutscene,
	CPED_CONFIG_FLAG_PedIsEnemyToPlayer,
	CPED_CONFIG_FLAG_DisableEvasiveDives,
	CPED_CONFIG_FLAG_PedGeneratesDeadBodyEvents, //  Generates shocking events as if dead
	CPED_CONFIG_FLAG_DontAttackPlayerWithoutWantedLevel,
	CPED_CONFIG_FLAG_DontInfluenceWantedLevel, //  Can do any crime against this character and the cops turn a blind eye (no crime reported)
	CPED_CONFIG_FLAG_DisablePlayerLockon,
	CPED_CONFIG_FLAG_DisableLockonToRandomPeds,
	CPED_CONFIG_FLAG_AllowLockonToFriendlyPlayers,
	CPED_CONFIG_FLAG_DisableHornAudioWhenDead, // Disable the horn when a ped dies in the car and has his head against the wheel
	CPED_CONFIG_FLAG_PedBeingDeleted,
	CPED_CONFIG_FLAG_BlockWeaponSwitching, //  Disable weapon switching while this is set
	CPED_CONFIG_FLAG_BlockGroupPedAimedAtResponse, //  Disable the behaviour which causes player-group peds to crouch when the player aims at them
	CPED_CONFIG_FLAG_WillFollowLeaderAnyMeans, //  Basically defines whether group peds will use cars etc to follow their leader (default=false)
	CPED_CONFIG_FLAG_BlippedByScript, //  Set to true if the char has ever been blipped, not 100% correct so don't use it on anything sensitive.
	CPED_CONFIG_FLAG_DrawRadarVisualField, //  Draw this peds visual field in the stealth radar
	CPED_CONFIG_FLAG_StopWeaponFiringOnImpact, //  Set to true to stop the peds weapon firing on impact when they drop it
	CPED_CONFIG_FLAG_DissableAutoFallOffTests, //  Set to true to stop ped scanning for things to fall off when shot by the player
	CPED_CONFIG_FLAG_SteerAroundDeadBodies, //  Forces peds to steer around dead bodies, the default is false.
	CPED_CONFIG_FLAG_ConstrainToNavMesh, //  Ped is constrained to navmesh's surface
	CPED_CONFIG_FLAG_SyncingAnimatedProps, //  Set this to true if the ped should attempt to synchronise the animations of an attached prop to its movement anims
	CPED_CONFIG_FLAG_IsFiring,
	CPED_CONFIG_FLAG_WasFiring,
	CPED_CONFIG_FLAG_IsStanding, //  is ped standing on something
	CPED_CONFIG_FLAG_WasStanding, //  was the ped standing last frame
	CPED_CONFIG_FLAG_InVehicle, //  is in a vehicle
	CPED_CONFIG_FLAG_OnMount,
	CPED_CONFIG_FLAG_AttachedToVehicle, //  is attached to a vehicle
	CPED_CONFIG_FLAG_IsSwimming, //  is ped swimming in water
	CPED_CONFIG_FLAG_WasSwimming, //  was the ped swimming in water last frame
	CPED_CONFIG_FLAG_IsSkiing, //  is the ped skiing
	CPED_CONFIG_FLAG_IsSitting, //  is the ped sitting
	CPED_CONFIG_FLAG_KilledByStealth, // Determines if this ped was killed by a stealth action
	CPED_CONFIG_FLAG_KilledByTakedown, // Determines if this ped was killed by a takedown action
	CPED_CONFIG_FLAG_Knockedout, // Determines if this ped was finished with a knockout action
	CPED_CONFIG_FLAG_ClearRadarBlipOnDeath, //  so peds automatically given blips will clear them again when they die (mainly used for peds recruited into players group)
	CPED_CONFIG_FLAG_JustGotOffTrain, //  train code uses this when grabbing random peds to get in trains
	CPED_CONFIG_FLAG_JustGotOnTrain, //  train code uses this when grabbing random peds to get in trains
	CPED_CONFIG_FLAG_UsingCoverPoint, //  set to true when a ped is in process of using a cover point
	CPED_CONFIG_FLAG_IsInTheAir, //  is in the air
	CPED_CONFIG_FLAG_KnockedUpIntoAir, //  has ped been knocked up into the air by a car collision
	CPED_CONFIG_FLAG_IsAimingGun, //  is ped aiming gun
	CPED_CONFIG_FLAG_HasJustLeftCar, //  used by navigation to force scan for cars collisions
	CPED_CONFIG_FLAG_TargetWhenInjuredAllowed,
	CPED_CONFIG_FLAG_CurrLeftFootCollNM, //  footprints
	CPED_CONFIG_FLAG_PrevLeftFootCollNM, //  footprints
	CPED_CONFIG_FLAG_CurrRightFootCollNM, //  footprints
	CPED_CONFIG_FLAG_PrevRightFootCollNM, //  footprints
	CPED_CONFIG_FLAG_HasBeenBumpedInCar, //  Has this ped been bumped by a car while driving?
	CPED_CONFIG_FLAG_InWaterTaskQuitToClimbLadder, //  The in-water task has just quit in response to a ladder-climb request
	CPED_CONFIG_FLAG_NMTwoHandedWeaponBothHandsConstrained, //  when using physical 2-handed weapons, both hands have been latched to the gun model (used when sending ConfigureCharacter to NM)
	CPED_CONFIG_FLAG_CreatedBloodPoolTimer,
	CPED_CONFIG_FLAG_DontActivateRagdollFromAnyPedImpact,
	CPED_CONFIG_FLAG_GroupPedFailedToEnterCover,
	CPED_CONFIG_FLAG_AlreadyChattedOnPhone,
	CPED_CONFIG_FLAG_AlreadyReactedToPedOnRoof,
	CPED_CONFIG_FLAG_ForcePedLoadCover, // Set by the script to force a ped to load cover
	CPED_CONFIG_FLAG_BlockCoweringInCover,
	CPED_CONFIG_FLAG_BlockPeekingInCover,
	CPED_CONFIG_FLAG_JustLeftCarNotCheckedForDoors, //  Set when a ped exits a car.  The first time he he moves he should check for door obstructions.
	CPED_CONFIG_FLAG_VaultFromCover,
	CPED_CONFIG_FLAG_AutoConversationLookAts,
	CPED_CONFIG_FLAG_UsingCrouchedPedCapsule, //  Set to indicate that the ped's bounds are in the crouched configuration
	CPED_CONFIG_FLAG_HasDeadPedBeenReported, //  Whether this ped has been investigated (for dead peds)
	CPED_CONFIG_FLAG_ForcedAim, //  If set, we will always behave like we have the aim trigger pressed
	CPED_CONFIG_FLAG_SteersAroundPeds, //  Enables/disables the low-level steering behaviour around peds and objects
	CPED_CONFIG_FLAG_SteersAroundObjects, //  Enables/disables the low-level steering behaviour around peds and objects
	CPED_CONFIG_FLAG_OpenDoorArmIK, // Set if the ped should enable open door arm IK
	CPED_CONFIG_FLAG_ForceReload, // Force a reload of the current weapon hideFrom=script
	CPED_CONFIG_FLAG_DontActivateRagdollFromVehicleImpact, // Blocks ragdoll activation when hit by a vehicle
	CPED_CONFIG_FLAG_DontActivateRagdollFromBulletImpact, // Blocks ragdoll activation when hit by a bullet
	CPED_CONFIG_FLAG_DontActivateRagdollFromExplosions, // Blocks ragdoll activation when hit by an explosive
	CPED_CONFIG_FLAG_DontActivateRagdollFromFire, // Blocks ragdoll activation when set on fire
	CPED_CONFIG_FLAG_DontActivateRagdollFromElectrocution, // Blocks ragdoll activation when electrocuted
	CPED_CONFIG_FLAG_IsBeingDraggedToSafety, // Whether this ped is being dragged to safety
	CPED_CONFIG_FLAG_HasBeenDraggedToSafety, // Whether this ped has been dragged to safety
	CPED_CONFIG_FLAG_KeepWeaponHolsteredUnlessFired, // Ignores the creation of the weapon object unless the gun is shot.
	CPED_CONFIG_FLAG_ForceScriptControlledKnockout, // Forces a melee knockout state for the victim ped
	CPED_CONFIG_FLAG_FallOutOfVehicleWhenKilled, // Forces a ped to fall out of a vehicle when killed
	CPED_CONFIG_FLAG_GetOutBurningVehicle, // If set, a ped will escape a burning vehicle they are inside
	CPED_CONFIG_FLAG_BumpedByPlayer, // Whether this ped has been bumped by the player.
	CPED_CONFIG_FLAG_RunFromFiresAndExplosions, // If set, a ped will run away from fires or potential explosions
	CPED_CONFIG_FLAG_TreatAsPlayerDuringTargeting, // If set, the ped will be given the same boost a player gets in the targeting scoring system.
	CPED_CONFIG_FLAG_IsHandCuffed, // indicates if the ped is currently hand cuffed
	CPED_CONFIG_FLAG_IsAnkleCuffed, // indicates if the ped is currently ankle cuffed
	CPED_CONFIG_FLAG_DisableMelee, // Disables the melee for a particular ped
	CPED_CONFIG_FLAG_DisableUnarmedDrivebys, // Disables unarmed driveby taunts for ped
	CPED_CONFIG_FLAG_JustGetsPulledOutWhenElectrocuted, // MP only, if ped is electrocuted or rubber bulletted, players jacking the ped will just pull them out
	CPED_CONFIG_FLAG_UNUSED_REPLACE_ME, //
	CPED_CONFIG_FLAG_WillNotHotwireLawEnforcementVehicle, // True if the ped will skip hotwiring a law enforcement vehicle if it needs to be hotwired
	CPED_CONFIG_FLAG_WillCommandeerRatherThanJack, // True if the ped will try to commandeer a vehicle rather than jack if possible
	CPED_CONFIG_FLAG_CanBeAgitated, // True if the ped will respond to agitation events
	CPED_CONFIG_FLAG_ForcePedToFaceLeftInCover, // If set ped will turn to face left in cover
	CPED_CONFIG_FLAG_ForcePedToFaceRightInCover, // If set ped will turn to face right in cover
	CPED_CONFIG_FLAG_BlockPedFromTurningInCover, // If set ped will not turn in cover, unless one of the force flags is set
	CPED_CONFIG_FLAG_KeepRelationshipGroupAfterCleanUp, // Will allow the ped to keep their relationship group after mission cleanup as opposed to going back to default
	CPED_CONFIG_FLAG_ForcePedToBeDragged, // Forces Ped To Loop Try Locked Door Anim In Order To Be Dragged Along When Vehicle Moves
	CPED_CONFIG_FLAG_PreventPedFromReactingToBeingJacked, // Ped doesn't react when being jacked
	CPED_CONFIG_FLAG_IsScuba, // indicates if the ped is currently equipped for scuba
	CPED_CONFIG_FLAG_WillArrestRatherThanJack, // For cops arresting peds in vehicles
	CPED_CONFIG_FLAG_RemoveDeadExtraFarAway, // We must be further away before ped polulation remove this ped when it is dead
	CPED_CONFIG_FLAG_RidingTrain, // True if the ped is riding a train
	CPED_CONFIG_FLAG_ArrestResult, // True if the arrest task succeeded
	CPED_CONFIG_FLAG_CanAttackFriendly, // True allows this ped to attack peds theya re friendly with
	CPED_CONFIG_FLAG_WillJackAnyPlayer, // True if this player ped can jack any other player (MP Only)
	CPED_CONFIG_FLAG_BumpedByPlayerVehicle, // Whether this ped has been bumped by a player vehicle.
	CPED_CONFIG_FLAG_DodgedPlayerVehicle, //  Whether this ped has just dodged a player vehicle.
	CPED_CONFIG_FLAG_WillJackWantedPlayersRatherThanStealCar, // True if this player will jack wanted passengers rather than try to steal a car (cops arresting crims) (MP Only)
	CPED_CONFIG_FLAG_NoCopWantedAggro, // If this flag is set on a cap, skip some of the code that would normally make them extra aggressive and alert.
	CPED_CONFIG_FLAG_DisableLadderClimbing, // If this flag is set on a ped it will not scan for or climb ladders
	CPED_CONFIG_FLAG_StairsDetected, // If this flag is set on a ped it has detected stairs
	CPED_CONFIG_FLAG_SlopeDetected, // If this flag is set on a ped it has detected a slope
	CPED_CONFIG_FLAG_HelmetHasBeenShot, // If this flag is set on a ped it's helmet has been damaged
	CPED_CONFIG_FLAG_CowerInsteadOfFlee, // If set, the ped will cower in place rather than flee, used. Used for scenarios in confined spaces.
	CPED_CONFIG_FLAG_CanActivateRagdollWhenVehicleUpsideDown, // If set the ped will be allowed to ragdoll when the vehicle they are in gets turned upside down if the seat supports it.
	CPED_CONFIG_FLAG_AlwaysRespondToCriesForHelp, // If set, the ped will respond to CEventInjuredCryForHelp regardless if it is allied with the injured ped.
	CPED_CONFIG_FLAG_DisableBloodPoolCreation,
	CPED_CONFIG_FLAG_ShouldFixIfNoCollision, // If set, the ped will be fixed if there is no collision around.
	CPED_CONFIG_FLAG_CanPerformArrest, // If set, the ped can perform arrests on peds that can be arrested
	CPED_CONFIG_FLAG_CanPerformUncuff, // If set, the ped can uncuff peds that are handcuffed
	CPED_CONFIG_FLAG_CanBeArrested, // If set, the ped may be arrested
	CPED_CONFIG_FLAG_MoverConstrictedByOpposingCollisions, // If set, the ped's mover is getting collisions from opposing sides.
	CPED_CONFIG_FLAG_PlayerPreferFrontSeatMP, // When true, Prefer the front seat when getting in a car with buddies.
	CPED_CONFIG_FLAG_DontActivateRagdollFromImpactObject,
	CPED_CONFIG_FLAG_DontActivateRagdollFromMelee,
	CPED_CONFIG_FLAG_DontActivateRagdollFromWaterJet,
	CPED_CONFIG_FLAG_DontActivateRagdollFromDrowning,
	CPED_CONFIG_FLAG_DontActivateRagdollFromFalling,
	CPED_CONFIG_FLAG_DontActivateRagdollFromRubberBullet,
	CPED_CONFIG_FLAG_IsInjured, // When true, the ped will use injured movement anim sets and getup animations.
	CPED_CONFIG_FLAG_DontEnterVehiclesInPlayersGroup, // When true, will follow the player around if in their group but wont enter vehicles.
	CPED_CONFIG_FLAG_SwimmingTasksRunning, // stronger than IsSwimming, persists so long as the tasks are active
	CPED_CONFIG_FLAG_PreventAllMeleeTaunts, // Disable all melee taunts for this particular ped
	CPED_CONFIG_FLAG_ForceDirectEntry, // Will force this ped to use the direct entry point for any vehicle they try to enter, or warp in
	CPED_CONFIG_FLAG_AlwaysSeeApproachingVehicles, // This ped will always see approaching vehicles (even from behind).
	CPED_CONFIG_FLAG_CanDiveAwayFromApproachingVehicles, // This ped can dive away from approaching vehicles.
	CPED_CONFIG_FLAG_AllowPlayerToInterruptVehicleEntryExit, // Will allow player to interrupt a peds scripted entry/exit task as if they had triggered it themselves
	CPED_CONFIG_FLAG_OnlyAttackLawIfPlayerIsWanted, // This ped won't attack cops unless the player is wanted.
	CPED_CONFIG_FLAG_PlayerInContactWithKinematicPed, // Gets set to true if the player ped is colliding against a ped in kinematic mode.
	CPED_CONFIG_FLAG_PlayerInContactWithSomethingOtherThanKinematicPed, // Gets set to true if the player ped is colliding against something which isn't a ped in kinematic mode.
	CPED_CONFIG_FLAG_PedsJackingMeDontGetIn, // If set any ped jacking this ped will not get in as part of the jack.
	CPED_CONFIG_FLAG_AdditionalRappellingPed,
	CPED_CONFIG_FLAG_PedIgnoresAnimInterruptEvents, // AI peds only, will not early out of anims
	CPED_CONFIG_FLAG_IsInCustody, //  Signifies a player is in custody
	CPED_CONFIG_FLAG_ForceStandardBumpReactionThresholds, // By default, armed and friendly peds have increased resistance to being bumped by players and friendly vehicles. Setting this flag will make them use the standard thresholds instead.
	CPED_CONFIG_FLAG_LawWillOnlyAttackIfPlayerIsWanted, // If set, this ped can only be attacked by law if the player is wanted
	CPED_CONFIG_FLAG_IsAgitated, // If set, this ped is agitated.
	CPED_CONFIG_FLAG_PreventAutoShuffleToDriversSeat, // MP only, prevents passenger from auto shuffling over to drivers seat if it becomes free.
	CPED_CONFIG_FLAG_UseKinematicModeWhenStationary, // When enabled, the ped will continually set the kinematic mode reset flag when stationary.
	CPED_CONFIG_FLAG_EnableWeaponBlocking, // When enabled, Non-player peds can use WeaponBlocking behaviors
	CPED_CONFIG_FLAG_HasHurtStarted,
	CPED_CONFIG_FLAG_DisableHurt, // Will prevent the peds go into hurt combat mode
	CPED_CONFIG_FLAG_PlayerIsWeird, // Should this player ped periodically generate shocking events for being weird.
	CPED_CONFIG_FLAG_PedHadPhoneConversation, // Has this ped had a phone conversation before.
	CPED_CONFIG_FLAG_BeganCrossingRoad, // Indicates ped started crossing the road in case of interruption.
	CPED_CONFIG_FLAG_WarpIntoLeadersVehicle, // Warp into leaders vehicle
	CPED_CONFIG_FLAG_DoNothingWhenOnFootByDefault, // Do nothing when on foot by default
	CPED_CONFIG_FLAG_UsingScenario, // Set when the ped is using a scenario. Call CPed::UpdateSpatialArrayTypeFlags() if changing.
	CPED_CONFIG_FLAG_VisibleOnScreen, // Set when the ped is visible on screen, as determined by CPedAILodManager.
	CPED_CONFIG_FLAG_DontCollideWithKinematic, // If true, the ped will not collide with other kinematic peds.
	CPED_CONFIG_FLAG_ActivateOnSwitchFromLowPhysicsLod, // If set, activate physics when switching from low to regular physics LOD.
	CPED_CONFIG_FLAG_DontActivateRagdollOnPedCollisionWhenDead, // Peds with this flag set won't be allowed to reactivate their ragdoll when hit by another ragdoll.
	CPED_CONFIG_FLAG_DontActivateRagdollOnVehicleCollisionWhenDead, // Peds with this flag set won't be allowed to reactivate their ragdoll when hit by a vehicle.
	CPED_CONFIG_FLAG_HasBeenInArmedCombat, // Is set if this ped has ever been in armed combat
	CPED_CONFIG_FLAG_UseDiminishingAmmoRate, // Set for when we want to diminish the ammo at a slower rate. Used specifically in cases where AI do not have infinite ammo.
	CPED_CONFIG_FLAG_Avoidance_Ignore_All, // This ped won't steer around anyone
	CPED_CONFIG_FLAG_Avoidance_Ignored_by_All, // Other peds won't steer around this ped
	CPED_CONFIG_FLAG_Avoidance_Ignore_Group1, // This ped won't steer around peds marked group 1
	CPED_CONFIG_FLAG_Avoidance_Member_of_Group1, // This ped is marked as a member of avoidance group 1
	CPED_CONFIG_FLAG_ForcedToUseSpecificGroupSeatIndex, // Ped is forced to use specific seat index set by SET_PED_GROUP_MEMBER_PASSENGER_INDEX
	CPED_CONFIG_FLAG_LowPhysicsLodMayPlaceOnNavMesh, // If set, peds in low lod physics will be placed so that their feet rest on the navmesh
	CPED_CONFIG_FLAG_DisableExplosionReactions, // If set, peds will disable all explosion reactions
	CPED_CONFIG_FLAG_DodgedPlayer, // Whether this ped has just dodged a player.
	CPED_CONFIG_FLAG_WaitingForPlayerControlInterrupt, // Set when player switches to an ai ped and keeps the scripted task of the ai ped
	CPED_CONFIG_FLAG_ForcedToStayInCover, // Ped won't move out of cover when set (not even to fire).
	CPED_CONFIG_FLAG_GeneratesSoundEvents, // Does this ped generate sound events?
	CPED_CONFIG_FLAG_ListensToSoundEvents, // Does this ped have the ability to respond to sound events?
	CPED_CONFIG_FLAG_AllowToBeTargetedInAVehicle, // Ped can be targeting inside a vehicle
	CPED_CONFIG_FLAG_WaitForDirectEntryPointToBeFreeWhenExiting,
	CPED_CONFIG_FLAG_OnlyRequireOnePressToExitVehicle,
	CPED_CONFIG_FLAG_ForceExitToSkyDive,
	CPED_CONFIG_FLAG_SteersAroundVehicles, //  Enables/disables the low-level steering behaviour around vehicles
	CPED_CONFIG_FLAG_AllowPedInVehiclesOverrideTaskFlags, // If set, allow the ped to be set in vehicles even if the ped's TaskData would otherwise disallow it.
	CPED_CONFIG_FLAG_DontEnterLeadersVehicle, // If set, the ped will not enter the leader's vehicle.
	CPED_CONFIG_FLAG_DisableExitToSkyDive,
	CPED_CONFIG_FLAG_ScriptHasDisabledCollision, // Script disabled collision on this ped via SET_ENTITY_COLLISION, this leaves on collision against explosions and weapons
	CPED_CONFIG_FLAG_UseAmbientModelScaling, // This ped is drawn randomly scaled from [0.5,1.0]
	CPED_CONFIG_FLAG_DontWatchFirstOnNextHurryAway, // Hurry away without watching the next time this ped runs CTaskHurryAway.
	CPED_CONFIG_FLAG_DisablePotentialToBeWalkedIntoResponse, // make EVENT_POTENTIAL_BE_WALKED_INTO not affect this ped.
	CPED_CONFIG_FLAG_DisablePedAvoidance, // This ped will not avoid other peds whilst navigating
	CPED_CONFIG_FLAG_ForceRagdollUponDeath, // When the ped dies, it will ragdoll instead of potentially choosing an animated death.
	CPED_CONFIG_FLAG_CanLosePropsOnDamage, // When ped receives damage any prop glasses could be knocked off
	CPED_CONFIG_FLAG_DisablePanicInVehicle, //
	CPED_CONFIG_FLAG_AllowedToDetachTrailer, // Allow this ped to detach trailers from vehicles
	CPED_CONFIG_FLAG_HasShotBeenReactedToFromFront, //
	CPED_CONFIG_FLAG_HasShotBeenReactedToFromBack, //
	CPED_CONFIG_FLAG_HasShotBeenReactedToFromLeft, //
	CPED_CONFIG_FLAG_HasShotBeenReactedToFromRight, //
	CPED_CONFIG_FLAG_AllowBlockDeadPedRagdollActivation, // If set, the ragdoll activation blocking flags can be used to disable activation of dead peds. Otherwise, by default, dead peds can always activate their ragdolls
	CPED_CONFIG_FLAG_IsHoldingProp, // True if the ped is currently holding a prop.
	CPED_CONFIG_FLAG_BlocksPathingWhenDead, // When this ped dies their body will block all pathfinding modes - not just wandering.
	CPED_CONFIG_FLAG_ForcePlayNormalScenarioExitOnNextScriptCommand, // The next time this ped leaves a scenario to perform some script task they will be forced into their normal scenario exit.
	CPED_CONFIG_FLAG_ForcePlayImmediateScenarioExitOnNextScriptCommand, // The next time this ped leaves a scneario to perform some script task they will be forced into their immediate (blend out) exit.
	CPED_CONFIG_FLAG_ForceSkinCharacterCloth, // Force character cloth to stay skinned immediately after being created. If flag is not set then character cloth is not forced to be skinned when created.
	CPED_CONFIG_FLAG_LeaveEngineOnWhenExitingVehicles, // The player will leave the engine running when leaving vehicles
	CPED_CONFIG_FLAG_PhoneDisableTextingAnimations, // tells taskmobile phone to not texting animations.  Currently don't play these in MP
	CPED_CONFIG_FLAG_PhoneDisableTalkingAnimations, // tells taskmobile phone to not talking animations.  Currently don't play these in MP
	CPED_CONFIG_FLAG_PhoneDisableCameraAnimations, // tells taskmobile phone to not camera animations.  Currently don't play these in SP
	CPED_CONFIG_FLAG_DisableBlindFiringInShotReactions, // Stops the ped from accidentally blind firing it's weapon when doing an nm shot reaction
	CPED_CONFIG_FLAG_AllowNearbyCoverUsage, // This makes it so that OTHER peds are allowed to take cover at points that would otherwise be rejected due to proximity
	CPED_CONFIG_FLAG_InStrafeTransition, // True if in strafe transition.
	CPED_CONFIG_FLAG_CanPlayInCarIdles, // If false, blocks in-car idle animations from playing.
	CPED_CONFIG_FLAG_CanAttackNonWantedPlayerAsLaw, // If this is a law ped then it will ignore the player wanted level clean check in combat and continue attacking
	CPED_CONFIG_FLAG_WillTakeDamageWhenVehicleCrashes, // Ped gets damaged when the vehicle they are in crashes
	CPED_CONFIG_FLAG_AICanDrivePlayerAsRearPassenger, // If this ai ped is driving the vehicle, if the player taps to enter, they will enter as a rear passenger, if they hold, they'll jack the driver
	CPED_CONFIG_FLAG_PlayerCanJackFriendlyPlayers, // If a friendly player is driving the vehicle, if the player taps to enter, they will enter as a passenger, if they hold, they'll jack the driver
	CPED_CONFIG_FLAG_OnStairs, // Are we on stairs?
	CPED_CONFIG_FLAG_SimulatingAiming, // Simulating the aim button for player until on input detection.
	CPED_CONFIG_FLAG_AIDriverAllowFriendlyPassengerSeatEntry, // If this ai ped is driving the vehicle, allow players to get in passenger seats
	CPED_CONFIG_FLAG_ParentCarIsBeingRemoved, // Set on the target ped if the car they are in is being removed to avoid an expensive detach check
	CPED_CONFIG_FLAG_AllowMissionPedToUseInjuredMovement, // Set the target ped to be allowed to use Injured movement clips
	CPED_CONFIG_FLAG_CanLoseHelmetOnDamage, // When ped receives a head shot then a helmet can be knocked off
	CPED_CONFIG_FLAG_NeverDoScenarioExitProbeChecks, // When this ped exits a scenario they ignore probe checks against the environment and just pick an exit clip.
	CPED_CONFIG_FLAG_SuppressLowLODRagdollSwitchWhenCorpseSettles, // This will suppress the automatic switch to a lower ragdoll LOD when switching to the ragdoll frame after dying.
	CPED_CONFIG_FLAG_PreventUsingLowerPrioritySeats, // Don't use certain seats (like hanging on the side of a vehicle)
	CPED_CONFIG_FLAG_JustLeftVehicleNeedsReset, // Set when leaving a vehicle and disabling collision with the vehicle exiting to indicate we need to clear out the disabling
	CPED_CONFIG_FLAG_TeleportIfCantReachPlayer, // If this ped is following the player and stuck in a place where he can't be reached, teleport when possible.
	CPED_CONFIG_FLAG_PedsInVehiclePositionNeedsReset, // Ped was being jacked/killed but isn't anymore, ensure they're in the seat
	CPED_CONFIG_FLAG_PedsFullyInSeat, // Ped is fully in the seat (Set after the position needs reset flag)
	CPED_CONFIG_FLAG_AllowPlayerLockOnIfFriendly, // If this ped is friendly with the player, this will allow the ped to lockon
	CPED_CONFIG_FLAG_UseCameraHeadingForDesiredDirectionLockOnTest, // Force camera direction for heading test if desired direction is also set
	CPED_CONFIG_FLAG_TeleportToLeaderVehicle, // If set, teleport if ped is not in the leader's vehicle before TaskEnterVehicle::m_SecondsBeforeWarpToLeader.
	CPED_CONFIG_FLAG_Avoidance_Ignore_WeirdPedBuffer, // Don't give weird peds extra buffer
	CPED_CONFIG_FLAG_OnStairSlope, // Are we on a stair slope?
	CPED_CONFIG_FLAG_HasPlayedNMGetup, // This ped has gotten up from NM at least once.
	CPED_CONFIG_FLAG_DontBlipCop, // Wanted system shouldnt consider this ped when creating blips.
	CPED_CONFIG_FLAG_SpawnedAtExtendedRangeScenario, // Set if the ped spawned at a scenario with extended range.
	CPED_CONFIG_FLAG_WalkAlongsideLeaderWhenClose, // This ped will walk alongside group leader if they are the first member of the leader's pedgroup, they are close enough to the leader, and the pedgroup's formation is set up to allow this (such as in the default CPedFormationTypes::FORMATION_LOOSE).
	CPED_CONFIG_FLAG_KillWhenTrapped, // This will kill a mission ped that becomes trapped (like under a cow carcass) and cannot getup.
	CPED_CONFIG_FLAG_EdgeDetected, // If this flag is set on a ped it has detected an edge
	CPED_CONFIG_FLAG_AlwaysWakeUpPhysicsOfIntersectedPeds, // This ped will cause physics to activate on any ped this ped's capsule is inside of, even if this ped is being attached.
	CPED_CONFIG_FLAG_EquippedAmbientLoadOutWeapon, // This is set to prevent a ped from holstering a loadout weapon equipped during CPedPopulation::EquipPed as part of the defined CAmbientPedModelVariations' loadout.
	CPED_CONFIG_FLAG_AvoidTearGas, // If set, a ped will avoid tear gas.
	CPED_CONFIG_FLAG_StoppedSpeechUponFreezing, // Marks that we've already dealt with cleaning up speech audio after becoming frozen.
	CPED_CONFIG_FLAG_DisableGoToWritheWhenInjured, // If set, CPed::DAMAGED_GOTOWRITHE will no longer get set.  In particular, tazer hits wil no longer kill this ped in one hit.
	CPED_CONFIG_FLAG_OnlyUseForcedSeatWhenEnteringHeliInGroup, // If set this ped will only use their forced seat index if the vehicle they're entering is a heli as part of a group
	CPED_CONFIG_FLAG_ThrownFromVehicleDueToExhaustion, // Ped got tired and was thrown from bike. Used to scale ragdoll damage for a few seconds after the dismount.
	CPED_CONFIG_FLAG_UpdateEnclosedSearchRegion, // This ped will update their enclosed regions
	CPED_CONFIG_FLAG_DisableWeirdPedEvents,
	CPED_CONFIG_FLAG_ShouldChargeNow, // This ped should charge if in combat right away, for use by scripts, cleared once ped charges
	CPED_CONFIG_FLAG_RagdollingOnBoat, // We don't want ragdolling peds processing buoyancy when in a boat.
	CPED_CONFIG_FLAG_HasBrandishedWeapon,
	CPED_CONFIG_FLAG_AllowMinorReactionsAsMissionPed, // If true, this ped will react to events such as being hit by a vehicle as a mission ped
	CPED_CONFIG_FLAG_BlockDeadBodyShockingEventsWhenDead, // If true, this ped will not generate dead body shocking events when dead.
	CPED_CONFIG_FLAG_PedHasBeenSeen, // True if the ped has be visible to the player
	CPED_CONFIG_FLAG_PedIsInReusePool, // True if the ped is currently in the ped reuse pool
	CPED_CONFIG_FLAG_PedWasReused, // True if the ped was in the reuse pool and then was reused
	CPED_CONFIG_FLAG_DisableShockingEvents,
	CPED_CONFIG_FLAG_MovedUsingLowLodPhysicsSinceLastActive, // Set for peds that have moved using low LOD physics
	CPED_CONFIG_FLAG_NeverReactToPedOnRoof, // If true, this ped will not react to a ped standing on the roof.
	CPED_CONFIG_FLAG_ForcePlayFleeScenarioExitOnNextScriptCommand, // If set this ped will use a flee exit to leave on the next script command.
	CPED_CONFIG_FLAG_JustBumpedIntoVehicle, // Set for peds that just bumped into a vehicle
	CPED_CONFIG_FLAG_DisableShockingDrivingOnPavementEvents,
	CPED_CONFIG_FLAG_ShouldThrowSmokeNow, // This ped should throw a smoke grenade in combat right away, for use by scripts, cleared once ped throws
	CPED_CONFIG_FLAG_DisablePedConstraints, // Flags the ped to ensure it either does or does not have its control constraints
	CPED_CONFIG_FLAG_ForceInitialPeekInCover, // If set, ped will peek once before firing in cover. Cleared upon peeking.
	CPED_CONFIG_FLAG_CreatedByDispatch, // this ped was created by one of the dispatch systems, usually law enforcement
	CPED_CONFIG_FLAG_PointGunLeftHandSupporting, // NM state config flag. Set to true when the characters support hand has broken from the weapon
	CPED_CONFIG_FLAG_DisableJumpingFromVehiclesAfterLeader, // If true, ped will not bail out of a vehicle after his group leader.
	CPED_CONFIG_FLAG_DontActivateRagdollFromPlayerPedImpact, // Blocks ragdoll activation from animated player ped bumps
	CPED_CONFIG_FLAG_DontActivateRagdollFromAiRagdollImpact, // Blocks ragdoll activation from collisions with ai ragdolls
	CPED_CONFIG_FLAG_DontActivateRagdollFromPlayerRagdollImpact, // Blocks ragdoll activation from collisions with a ragdolling player
	CPED_CONFIG_FLAG_DisableQuadrupedSpring, // Use to disable quadruped spring processing when settling from a ragdoll performance
	CPED_CONFIG_FLAG_IsInCluster, // This ped is currently in a cluster.
	CPED_CONFIG_FLAG_ShoutToGroupOnPlayerMelee, // If set, ped will shout target position when melee attacked by a player
	CPED_CONFIG_FLAG_IgnoredByAutoOpenDoors, // Set this for a ped to be ignored by the auto opened doors when checking to see if the door should be opened.
	CPED_CONFIG_FLAG_PreferInjuredGetup, // Set this during nm tasks to trigger an injured geup when the ped gets up.
	CPED_CONFIG_FLAG_ForceIgnoreMeleeActiveCombatant, // Purposely ignore the melee active combatant role and push them into a support or inactive combatant role
	CPED_CONFIG_FLAG_CheckLoSForSoundEvents, // If set, ped will ignore sound events generated by entites it can't see.
	CPED_CONFIG_FLAG_JackedAbandonedCar, // This ped was spawned to steal an ambient car that was left around
	CPED_CONFIG_FLAG_CanSayFollowedByPlayerAudio, // If set, ped can play FRIEND_FOLLOWED_BY_PLAYER lines.
	CPED_CONFIG_FLAG_ActivateRagdollFromMinorPlayerContact, // If set, the ped will activate the nm ragdoll balance as soon as he's touhed by the player (ignoring velocity thresholds).
	CPED_CONFIG_FLAG_HasPortablePickupAttached, // If set, the ped is carrying a portable pickuup.
	CPED_CONFIG_FLAG_ForcePoseCharacterCloth, // If set, default cloth pose will be applied if is available in the character cloth when the cloth is created.
	CPED_CONFIG_FLAG_HasClothCollisionBounds, // If set, ped will use cloth collision bounds.
	CPED_CONFIG_FLAG_HasHighHeels, // Set when the ped has high heels
	CPED_CONFIG_FLAG_TreatAsAmbientPedForDriverLockOn, // If set, this force player ped to treat this ped as an ambient target rather than a mission ped for driver lock on/>
	CPED_CONFIG_FLAG_DontBehaveLikeLaw, // Currently used to prevent security peds from behaving like police (they will not report wanted position, can attack without WL, etc)
	CPED_CONFIG_FLAG_SpawnedAtScenario, // If set, the ped was originally spawned at a scenario point.
	CPED_CONFIG_FLAG_DisablePoliceInvestigatingBody, // If set, police will not perform the CTaskShockingPoliceInvestigate Behavior on the ped
	CPED_CONFIG_FLAG_DisableWritheShootFromGround, // If set, the ped will no longer shoot while writhing.
	CPED_CONFIG_FLAG_LowerPriorityOfWarpSeats, //
	CPED_CONFIG_FLAG_DisableTalkTo, //
	CPED_CONFIG_FLAG_DontBlip, // Stops a ped being blipped by the wanted system
	CPED_CONFIG_FLAG_IsSwitchingWeapon, // ped is running the swap weapon task
	CPED_CONFIG_FLAG_IgnoreLegIkRestrictions, // If set, the ped will ignore leg IK request restrictions for non-player peds.
	CPED_CONFIG_FLAG_ScriptForceNoTimesliceIntelligenceUpdate, // If set, the ped will never have their intelligence update time sliced across frames.
	CPED_CONFIG_FLAG_JackedOutOfMyVehicle, // If set, this ped has been jacked out of its vehicle.
	CPED_CONFIG_FLAG_WentIntoCombatAfterBeingJacked, // If set, this ped went into combat because of being jacked.
	CPED_CONFIG_FLAG_DontActivateRagdollForVehicleGrab, // Blocks ragdoll activation when grabbing vehicle doors
	CPED_CONFIG_FLAG_ForcePackageCharacterCloth, // Set the flag for forcing package on character cloth when cloth is created on the ped
	CPED_CONFIG_FLAG_DontRemoveWithValidOrder, //
	CPED_CONFIG_FLAG_AllowTaskDoNothingTimeslicing, // If set, this ped will timeslice it's DoNothing Task when computing default task.
	CPED_CONFIG_FLAG_ForcedToStayInCoverDueToPlayerSwitch, //
	CPED_CONFIG_FLAG_ForceProneCharacterCloth, // Set the flag to place character cloth in prone state when cloth is created on the ped
	CPED_CONFIG_FLAG_NotAllowedToJackAnyPlayers, //
	CPED_CONFIG_FLAG_InToStrafeTransition,
	CPED_CONFIG_FLAG_KilledByStandardMelee, // Killed by standard melee
	CPED_CONFIG_FLAG_AlwaysLeaveTrainUponArrival, // Does this ped always (not randomly) leave the train when it arrives at a station
	CPED_CONFIG_FLAG_ForcePlayDirectedNormalScenarioExitOnNextScriptCommand, // Set flag to determine that a directed normal exit should be use for new tasks on this scenario ped.
	CPED_CONFIG_FLAG_OnlyWritheFromWeaponDamage, // Only allow ped to writhe from weapon damage, not from other stuff, like small vehicle impacts
	CPED_CONFIG_FLAG_UseSloMoBloodVfx, // Flags the ped to use the slo mo blood vfx instead of the normal ones
	CPED_CONFIG_FLAG_EquipJetpack, // Equip/put on the jetpack if we have one in our inventory
	CPED_CONFIG_FLAG_PreventDraggedOutOfCarThreatResponse, // Don't do threat response when dragged out of a car
	CPED_CONFIG_FLAG_ScriptHasCompletelyDisabledCollision, // Script has completely disabled collision on this ped via SET_ENTITY_COMPLETELY_DISABLE_COLLISION
	CPED_CONFIG_FLAG_NeverDoScenarioNavChecks, // This ped will not check for navmesh when exiting their scenario.
	CPED_CONFIG_FLAG_ForceSynchronousScenarioExitChecking, // This ped will expensively probe for a scenario exit location in one frame.
	CPED_CONFIG_FLAG_ThrowingGrenadeWhileAiming, // Set true in CTaskAimGunOnFoot::Aiming_OnUpdate, false in CTaskAimAndThrowProjectile::CleanUp.
	CPED_CONFIG_FLAG_HeadbobToRadioEnabled, // Set in.
	CPED_CONFIG_FLAG_ForceDeepSurfaceCheck, // Don't do distance from camera culling of the deep surface check, needed for detecting snow, mud, etc.
	CPED_CONFIG_FLAG_DisableDeepSurfaceAnims, // Disable deep surface anims to prevent them slowing ped down
	CPED_CONFIG_FLAG_DontBlipNotSynced, // Don't blip this ped, this is not synced over the network to allow script to individually control a peds blippedness on different machines
	CPED_CONFIG_FLAG_IsDuckingInVehicle, // Ped is ducking inside a vehicle.
	CPED_CONFIG_FLAG_PreventAutoShuffleToTurretSeat,
	CPED_CONFIG_FLAG_DisableEventInteriorStatusCheck, // Disables the ignore events based on interior status check which normally has peds inside ignore events from outside
	CPED_CONFIG_FLAG_HasReserveParachute, // Does ped have a reserve chute that they can deploy
	CPED_CONFIG_FLAG_UseReserveParachute, // Use reserve parachute settings.
	CPED_CONFIG_FLAG_TreatDislikeAsHateWhenInCombat, // If this ped is in combat then any ped they dislike they will consider the relationship hate instead
	CPED_CONFIG_FLAG_OnlyUpdateTargetWantedIfSeen, // If the target is a player we will only set the WL or update the radar if they are seen
	CPED_CONFIG_FLAG_AllowAutoShuffleToDriversSeat, // Allows this ped to auto shuffle to the driver seat of a vehicle if the driver is dead (law and MP peds would do this normally)
	CPED_CONFIG_FLAG_DontActivateRagdollFromSmokeGrenade, // Blocks ragdoll activation when damaged by smoke grenade
	CPED_CONFIG_FLAG_LinkMBRToOwnerOnChain, // This ped will attempt to match the speed of the owner while following its current scenario chain.
	CPED_CONFIG_FLAG_AmbientFriendBumpedByPlayer, // The player has walked into our ambient friend.
	CPED_CONFIG_FLAG_AmbientFriendBumpedByPlayerVehicle, // The player has driven into our ambient friend.
	CPED_CONFIG_FLAG_InFPSUnholsterTransition, // Player is playing the unholster transition in FPS mode
	CPED_CONFIG_FLAG_PreventReactingToSilencedCloneBullets, // Prevents the ped from reacting to silenced bullets fired from network clone peds
	CPED_CONFIG_FLAG_DisableInjuredCryForHelpEvents, // Blocks ped from creating the injured cry for help events(run over, tazed or melee would usually do this)
	CPED_CONFIG_FLAG_NeverLeaveTrain, // Prevents peds riding trains from getting off them
	CPED_CONFIG_FLAG_DontDropJetpackOnDeath, // When dead, don't drop eqquiped jetpack
	CPED_CONFIG_FLAG_UseFPSUnholsterTransitionDuringCombatRoll, // Player is playing the unholster transition in FPS mode
	CPED_CONFIG_FLAG_ExitingFPSCombatRoll, // Player is exiting combat roll in FPS mode
	CPED_CONFIG_FLAG_ScriptHasControlOfPlayer, // True when script is controlling the movement of the player
	CPED_CONFIG_FLAG_PlayFPSIdleFidgetsForProjectile, // True when we should be playing idle fidgets for projectiles
	CPED_CONFIG_FLAG_DisableAutoEquipHelmetsInBikes, // Set from interaction menu
	CPED_CONFIG_FLAG_DisableAutoEquipHelmetsInAircraft, // Set from interaction menu
	CPED_CONFIG_FLAG_WasPlayingFPSGetup, // Was playing getup animations in FPS mode
	CPED_CONFIG_FLAG_WasPlayingFPSMeleeActionResult, // Was playing action result animations in FPS mode
	CPED_CONFIG_FLAG_PreferNoPriorityRemoval, // Unless scenario conditions apply, make this ped go through normal deletion but not priority deletion.
	CPED_CONFIG_FLAG_FPSFidgetsAbortedOnFire, // True when the FPS idle fidgets are aborted because the player fired the gun
	CPED_CONFIG_FLAG_ForceFPSIKWithUpperBodyAnim, // True when upper body anims are used during various tasks.
	CPED_CONFIG_FLAG_SwitchingCharactersInFirstPerson, // True we switch a character in first person mode (in CGameWorld::ChangePlayerPed)
	CPED_CONFIG_FLAG_IsClimbingLadder, // True when the ped is climbing a ladder
	CPED_CONFIG_FLAG_HasBareFeet, // Set when the ped has no shoes
	CPED_CONFIG_FLAG_UNUSED_REPLACE_ME_2, //
	CPED_CONFIG_FLAG_GoOnWithoutVehicleIfItIsUnableToGetBackToRoad, // It will force the ped to abandon its vehicle (when using TaskGoToPointAnyMeans) if it is unable to get back to road
	CPED_CONFIG_FLAG_BlockDroppingHealthSnacksOnDeath, // Set by script to prevent peds from dropping snack health pickups on death (in CPed::CreateDeadPedPickups).
	CPED_CONFIG_FLAG_ResetLastVehicleOnVehicleExit, // Reset the ped's stored MyVehicle pointer when this ped leaves their vehicle.
	CPED_CONFIG_FLAG_ForceThreatResponseToNonFriendToFriendMeleeActions, // Forces threat response to melee actions from non friend to friend peds.
	CPED_CONFIG_FLAG_DontRespondToRandomPedsDamage, // Do not respond to random peds damage
	CPED_CONFIG_FLAG_AllowContinuousThreatResponseWantedLevelUpdates, // Shares logic of OnlyUpdateTargetWantedIfSeen but will continue to check even after the initial WL is set
	CPED_CONFIG_FLAG_KeepTargetLossResponseOnCleanup, // On cleanup the ped will not reset their target loss response
	CPED_CONFIG_FLAG_PlayersDontDragMeOutOfCar, // Similar to DontDragMeOutCar except it only prevents players from dragging the ped out and allows AI to still do so
	CPED_CONFIG_FLAG_BroadcastRepondedToThreatWhenGoingToPointShooting, // Whenever the ped starts shooting while going to a point, it trigger a responded to threat broadcast
	CPED_CONFIG_FLAG_IgnorePedTypeForIsFriendlyWith, // If this is set then IsFriendlyWith will ignore the ped type checks (i.e. two PEDTYPE_COP peds are not automatically friendly)/>
	CPED_CONFIG_FLAG_TreatNonFriendlyAsHateWhenInCombat, // Any non friendly ped will be considered as hated instead
	CPED_CONFIG_FLAG_DontLeaveVehicleIfLeaderNotInVehicle, // Supresses exit vehicle task being created in CEventLeaderExitedCarAsDriver::CreateResponseTask.
	CPED_CONFIG_FLAG_ChangeFromPermanentToAmbientPopTypeOnMigration, // Change ped to ambient pop type on migration.
	CPED_CONFIG_FLAG_AllowMeleeReactionIfMeleeProofIsOn, // Allow melee reaction to come through even if proof is on
	CPED_CONFIG_FLAG_UsingLowriderLeans, // Ped is playing lowrider lean animations due to vehicle suspension modification.
	CPED_CONFIG_FLAG_UsingAlternateLowriderLeans, // Ped is playing alternate lowrider lean animations (ie arm on window) due to vehicle suspension modification.
	CPED_CONFIG_FLAG_UseNormalExplosionDamageWhenBlownUpInVehicle, // If this is set, the ped won't be instantly killed if vehicle is blown up (from CAutomobile::BlowUpCar -> KillPedsInVehicle). Instead, they will take normal explosive damage and be forced to exit the vehicle if they're still alive.
	CPED_CONFIG_FLAG_DisableHomingMissileLockForVehiclePedInside, // Blocks locking on of the vehicle that the ped is inside.
	CPED_CONFIG_FLAG_DisableTakeOffScubaGear, // Same as CPED_RESET_FLAG_DisableTakeOffScubaGear but on a config flag.
	CPED_CONFIG_FLAG_IgnoreMeleeFistWeaponDamageMult, // Melee fist weapons (ie knuckle duster) won't apply relative health damage scaler (m_MeleeRightFistTargetHealthDamageScaler in weapon info).
	CPED_CONFIG_FLAG_LawPedsCanFleeFromNonWantedPlayer, // Allows law ped to flee even if ped is not wanted and CWanted::m_AllRandomsFlee is set.
	CPED_CONFIG_FLAG_ForceBlipSecurityPedsIfPlayerIsWanted, // Forces security peds (not cop peds) to be blipped on the minimap if the player is wanted. Set on the local player.
	CPED_CONFIG_FLAG_IsHolsteringWeapon, // Ped is running the swap weapon task and holstering the previous weapon, but has not started drawing the new one.
	CPED_CONFIG_FLAG_UseGoToPointForScenarioNavigation, // Don't use nav mesh for navigating to scenario points. DLC Hack for yachts
	CPED_CONFIG_FLAG_DontClearLocalPassengersWantedLevel, // Don't clear local ped's wanted level when remote ped in the same car has his wanted level cleared by script.
	CPED_CONFIG_FLAG_BlockAutoSwapOnWeaponPickups, // Block auto weapon swaps for weapon pickups.
	CPED_CONFIG_FLAG_ThisPedIsATargetPriorityForAI, // Increase AI targeting score for peds with this flag.
	CPED_CONFIG_FLAG_IsSwitchingHelmetVisor, // Indicates that ped is playing switch visor up/down anim
	CPED_CONFIG_FLAG_ForceHelmetVisorSwitch, // Forces ped to do a visor helmet switch if able to.
	CPED_CONFIG_FLAG_IsPerformingVehicleMelee, // Indicates that ped is performing vehicle melee action.
	CPED_CONFIG_FLAG_UseOverrideFootstepPtFx, // The ped should use any override footstep effects that are set up.
	CPED_CONFIG_FLAG_DisableVehicleCombat, // Disables vehicle combat.
	CPED_CONFIG_FLAG_TreatAsFriendlyForTargetingAndDamage, // Prevents this ped from being locked on and blocks ability to damage / fire at ped.
	CPED_CONFIG_FLAG_AllowBikeAlternateAnimations, // Allows players on motorcycles to use the alternate animation set when available.
	CPED_CONFIG_FLAG_TreatAsFriendlyForTargetingAndDamageNonSynced, // Prevents this ped from being locked on and blocks ability to damage / fire at ped. This flag is not synced, so will only work on machine that set it
	CPED_CONFIG_FLAG_UseLockpickVehicleEntryAnimations, // When set, will attempt to use lockpick animations designed for Franklin in SP mode when breaking into a car.
	CPED_CONFIG_FLAG_IgnoreInteriorCheckForSprinting, // When set, player will be able to sprint inside interriors even if it is tagged to prevent it.
	CPED_CONFIG_FLAG_SwatHeliSpawnWithinLastSpottedLocation, // When set, swat helicopters will spawn within last spotted location instead of actual ped location (and target is a player).
	CPED_CONFIG_FLAG_DisableStartEngine, // prevents ped from playing start engine anims (and turning engine on)
	CPED_CONFIG_FLAG_IgnoreBeingOnFire, // makes ped ignore being on fire (fleeing, reacting to CEventOnFire event)
	CPED_CONFIG_FLAG_DisableTurretOrRearSeatPreference, // Disables turret seat and activity seat preference for vehicle entry for local player
	CPED_CONFIG_FLAG_DisableWantedHelicopterSpawning, // Will not spawn wanted helicopters to chase after this target
	CPED_CONFIG_FLAG_UseTargetPerceptionForCreatingAimedAtEvents, // Will only create aimed at events if player is within normal perception of the target
	CPED_CONFIG_FLAG_DisableHomingMissileLockon, // Will prevent homing lockon on this ped
	CPED_CONFIG_FLAG_ForceIgnoreMaxMeleeActiveSupportCombatants, // Ignore max number of active support combatants and let ped join them as such
	CPED_CONFIG_FLAG_StayInDefensiveAreaWhenInVehicle, // Will try to stay within set defensive area while driving a vehicle
	CPED_CONFIG_FLAG_DontShoutTargetPosition, // Will prevent the ped from communicating target position to all other friendly peds
	CPED_CONFIG_FLAG_DisableHelmetArmor, // Will apply full headshot damage, regardless if ped has a helmet (or armored one)
	CPED_CONFIG_FLAG_CreatedByConcealedPlayer, // Marks a ped that was created by concealed player from marked up scenarios
	CPED_CONFIG_FLAG_PermanentlyDisablePotentialToBeWalkedIntoResponse, // Synced and permanent version of CPED_CONFIG_FLAG_DisablePotentialToBeWalkedIntoResponse
	CPED_CONFIG_FLAG_PreventVehExitDueToInvalidWeapon, // Will prevent ped from automatically being forced out of vehicle due to weapon being invalid (e.g. turret seats after going into water)
	CPED_CONFIG_FLAG_IgnoreNetSessionFriendlyFireCheckForAllowDamage, // Will ignore the friendly fire setting set by NETWORK_SET_FRIENDLY_FIRE_OPTION when checking if ped can be damaged
	CPED_CONFIG_FLAG_DontLeaveCombatIfTargetPlayerIsAttackedByPolice, // Will make ped stay in combat even if the player hes targeting starts being attacked by cops
	CPED_CONFIG_FLAG_CheckLockedBeforeWarp, // Will check when entering a vehicle if it is locked before warping
	CPED_CONFIG_FLAG_DontShuffleInVehicleToMakeRoom, // Will prevent a player from shuffling across to make room if another player is entering from the same side
	CPED_CONFIG_FLAG_GiveWeaponOnGetup, // Will give the ped a weapon to use once their weapon is removed for getups
	CPED_CONFIG_FLAG_DontHitVehicleWithProjectiles, // Ped fired projectiles will ignore the vehicle they are in
	CPED_CONFIG_FLAG_DisableForcedEntryForOpenVehiclesFromTryLockedDoor, // Will prevent ped from forcing entry into cars that are open from TryLockedDoor state
	CPED_CONFIG_FLAG_FiresDummyRockets, // This ped will fire rockets that explode when close to its target, and won't affect it
	CPED_CONFIG_FLAG_PedIsArresting, // Is the ped currently preforming an arrest
	CPED_CONFIG_FLAG_IsDecoyPed, // Will make this ped a decoy ped that will focus targeting
	CPED_CONFIG_FLAG_HasEstablishedDecoy, // This ped has created a decoy
	CPED_CONFIG_FLAG_BlockDispatchedHelicoptersFromLanding, // Will prevent dispatched helicopters from landing and dropping off peds
	CPED_CONFIG_FLAG_DontCryForHelpOnStun, // Will prevent peds from crying for help when shot with the stun gun
	CPED_CONFIG_FLAG_HitByTranqWeapon, // Tranq weapons are handled differently in terms of damage. This triggers that logic
	CPED_CONFIG_FLAG_CanBeIncapacitated, // If set, the ped may be incapacitated
	CPED_CONFIG_FLAG_ForcedAimFromArrest, // If set, we will always behave like we have the aim trigger pressed
	CPED_CONFIG_FLAG_DontChangeTargetFromMelee, // If set, we wont set a new target after a melee attack
	CPED_CONFIG_FLAG_DisableHealthRegenerationWhenStunned, // Used to disable health regeneration when damaged with the stun gun in MP
	CPED_CONFIG_FLAG_RagdollFloatsIndefinitely, // Prevents a dead ped from sinking
	CPED_CONFIG_FLAG_BlockElectricWeaponDamage, // Blocks electric weapon damage
};
