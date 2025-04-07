#pragma once

namespace Stand
{
	enum eDamageFlags
	{
		None = 0,
		IsAccurate = 1 << 0,
		MeleeDamage = 1 << 1,
		SelfDamage = 1 << 2,
		ForceMeleeDamage = 1 << 3,
		IgnorePedFlags = 1 << 4,
		ForceInstantKill = 1 << 5,
		IgnoreArmor = 1 << 6,
		IgnoreStatModifiers = 1 << 7,
		FatalMeleeDamage = 1 << 8,
		AllowHeadShot = 1 << 9,
		AllowDriverKill = 1 << 10,
		KillPriorToClearedWantedLevel = 1 << 11,
		SuppressImpactAudio = 1 << 12,
		ExpectedPlayerKill = 1 << 13,
		DontReportCrimes = 1 << 14,
		PtFxOnly = 1 << 15,
		UsePlayerPendingDamage = 1 << 16,
		AllowCloneMeleeDamage = 1 << 17,
		NoAnimatedMeleeReaction = 1 << 18,
		IgnoreRemoteDistCheck = 1 << 19,
		VehicleMeleeHit = 1 << 20,
		EnduranceDamageOnly = 1 << 21,
		HealthDamageOnly = 1 << 22,
		DamageFromBentBullet = 1 << 23
	};
}