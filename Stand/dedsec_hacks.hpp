#pragma once

#include <cstdint>

#include "fwddecl.hpp"

namespace Stand
{
	struct DedsecHackExplode;
	struct DedsecHackDestroy;
	struct DedsecHackDelete;
	struct DedsecHackDrive;
	struct DedsecHackEnter;
	struct DedsecHackBurn;
	struct DedsecHackRevive;
	struct DedsecHackMenuPlayer;
	struct DedsecHackMenuPlayerVeh;
	struct DedsecHackFreeze;
	struct DedsecHackUnfreeze;
	struct DedsecHackEmpty;
	struct DedsecHackDisarm;
	struct DedsecHackCower;
	struct DedsecHackFlee;
	struct DedsecHackIgnite;
	struct DedsecHackSlingshot;
	struct DedsecHackCage;
	struct DedsecHackKill;

	struct DedsecHack
	{
		bool enabled;
		const int32_t name;
		float x = 0.0f;
		float y = 0.0f;

		explicit constexpr DedsecHack(int32_t name, const bool default_on = true)
			: name(name), enabled(default_on)
		{
		}

		virtual void execute(AbstractEntity& ent) const = 0;

		static DedsecHackExplode explode;
		static DedsecHackDestroy destroy;
		static DedsecHackDelete del;
		static DedsecHackDrive drive;
		static DedsecHackEnter enter;
		static DedsecHackBurn burn;
		static DedsecHackRevive revive;
		static DedsecHackMenuPlayer menu_player;
		static DedsecHackMenuPlayerVeh menu_player_veh;
		static DedsecHackFreeze freeze;
		static DedsecHackUnfreeze unfreeze;
		static DedsecHackEmpty empty;
		static DedsecHackDisarm disarm;
		static DedsecHackCower cower;
		static DedsecHackFlee flee;
		static DedsecHackIgnite ignite;
		static DedsecHackSlingshot slingshot;
		static DedsecHackCage cage;
		static DedsecHackKill kill;
	};

	struct DedsecHackExplode : public DedsecHack
	{
		explicit constexpr DedsecHackExplode();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackDestroy : public DedsecHack
	{
		explicit constexpr DedsecHackDestroy();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackDelete : public DedsecHack
	{
		explicit constexpr DedsecHackDelete();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackDrive : public DedsecHack
	{
		explicit constexpr DedsecHackDrive();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackEnter : public DedsecHack
	{
		explicit constexpr DedsecHackEnter();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackBurn : public DedsecHack
	{
		explicit constexpr DedsecHackBurn();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackRevive : public DedsecHack
	{
		explicit constexpr DedsecHackRevive();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackMenuPlayer : public DedsecHack
	{
		explicit constexpr DedsecHackMenuPlayer();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackMenuPlayerVeh : public DedsecHack
	{
		explicit constexpr DedsecHackMenuPlayerVeh();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackFreeze : public DedsecHack
	{
		explicit constexpr DedsecHackFreeze();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackUnfreeze : public DedsecHack
	{
		explicit constexpr DedsecHackUnfreeze();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackEmpty : public DedsecHack
	{
		explicit constexpr DedsecHackEmpty();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackDisarm : public DedsecHack
	{
		explicit constexpr DedsecHackDisarm();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackCower : public DedsecHack
	{
		explicit constexpr DedsecHackCower();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackFlee : public DedsecHack
	{
		explicit constexpr DedsecHackFlee();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackIgnite : public DedsecHack
	{
		explicit constexpr DedsecHackIgnite();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackSlingshot : public DedsecHack
	{
		explicit constexpr DedsecHackSlingshot();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackCage : public DedsecHack
	{
		explicit constexpr DedsecHackCage();
		void execute(AbstractEntity& ent) const final;
	};

	struct DedsecHackKill : public DedsecHack
	{
		explicit constexpr DedsecHackKill();
		void execute(AbstractEntity& ent) const final;
	};
}
