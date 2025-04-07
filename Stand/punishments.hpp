#pragma once

#include <vector>

#include "fwddecl.hpp"
#include "gta_fwddecl.hpp"
#include "typedecl.hpp"

#include "Label.hpp"

#define PUNISHMENT_EXPLODE_ANON		0b1
#define PUNISHMENT_EXPLODE_OWNED	0b10
#define PUNISHMENT_BURN				0b100
#define PUNISHMENT_DIE				0b1000
#define PUNISHMENT_UNARM			0b10000
#define PUNISHMENT_INTERRUPT		0b100000
#define PUNISHMENT_FREEZE			0b1000000
#define PUNISHMENT_COWER			0b10000000
#define PUNISHMENT_FLEE				0b100000000
#define PUNISHMENT_PUSH				0b1000000000
#define PUNISHMENT_DELETE			0b10000000000
#define PUNISHMENT_MARK				0b100000000000
#define PUNISHMENT_SHOWMODELNAME	0b1000000000000
#define PUNISHMENT_DRIVE			0b10000000000000
#define PUNISHMENT_REVIVE			0b100000000000000
#define PUNISHMENT_RAGDOLL			0b1000000000000000
#define PUNISHMENT_REPAIR			0b10000000000000000
#define PUNISHMENT_WEAKEN			0b100000000000000000

#define PUNISHMENTFOR_NPC			0b000001
#define PUNISHMENTFOR_PLAYER		0b000010
#define PUNISHMENTFOR_WORLD			0b000100
#define PUNISHMENTFOR_AIMINGAUSER	0b001000
#define PUNISHMENTFOR_NOTBROAD		0b010000 // Set the punishment as not applicable to existence punishments
#define PUNISHMENTFOR_USERACTION	0b100000
#define PUNISHMENTFOR_NPCHOSTILITY  0b1000000

namespace Stand
{
	enum class PunishmentTrigger : uint8_t
	{
		EXISTENCE,
		PROXMITY,
		HOSTILE,
		AIMED_AT,
		AIMING_AT,
		SHOOTING_AT,
		
		DEDSEC_HACK = SHOOTING_AT
	};

	class Punishment
	{
	public:
		const punishment_t mask;
		const Label name;
		const uint8_t _for;
		const Label help_text;
		const std::vector<CommandName> gunshoot_command_names{};

		explicit Punishment(punishment_t mask, Label&& name, uint8_t _for, Label&& help_text = NOLABEL, std::vector<CommandName>&& gunshoot_command_names = {});

		[[nodiscard]] bool isApplicable(uint8_t _to) const noexcept;

		virtual void execute(AbstractEntity& ent, PunishmentTrigger trigger) const = 0;
	};

	class WorldPunishment : public Punishment
	{
	public:
		explicit WorldPunishment(punishment_t bitflag, Label&& name, uint8_t _for, Label&& help_text = NOLABEL);

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const override;
		virtual void execute(const v3& pos, AbstractEntity& ent, PunishmentTrigger trigger) const = 0;
	};

	class PunishmentExplodeAnon : public WorldPunishment
	{
	public:
		explicit PunishmentExplodeAnon();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
		void execute(const v3& pos, AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentExplodeOwned : public WorldPunishment
	{
	public:
		explicit PunishmentExplodeOwned();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
		void execute(const v3& pos, AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentBurn : public WorldPunishment
	{
	public:
		explicit PunishmentBurn();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
		void execute(const v3& pos, AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentDie : public Punishment
	{
	public:
		explicit PunishmentDie();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentUnarm : public Punishment
	{
	public:
		explicit PunishmentUnarm();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentInterrupt : public Punishment
	{
	public:
		explicit PunishmentInterrupt();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentFreeze : public Punishment
	{
	public:
		explicit PunishmentFreeze();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentCower : public Punishment
	{
	public:
		explicit PunishmentCower();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentFlee : public Punishment
	{
	public:
		explicit PunishmentFlee();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentPush : public Punishment
	{
	public:
		explicit PunishmentPush();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentDelete : public Punishment
	{
	public:
		explicit PunishmentDelete();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentMark : public Punishment
	{
	public:
		explicit PunishmentMark();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentShowModelName : public WorldPunishment
	{
	public:
		explicit PunishmentShowModelName();

		void execute(const v3& pos, AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentDrive : public Punishment
	{
	public:
		explicit PunishmentDrive();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentRevive : public Punishment
	{
	public:
		explicit PunishmentRevive();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentRagdoll : public Punishment
	{
	public:
		explicit PunishmentRagdoll();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentRepair : public Punishment
	{
	public:
		explicit PunishmentRepair();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	class PunishmentWeaken : public Punishment
	{
	public:
		explicit PunishmentWeaken();

		void execute(AbstractEntity& ent, PunishmentTrigger trigger) const final;
	};

	struct Punishments
	{
		inline static PunishmentExplodeAnon explode_anon{};
		inline static PunishmentExplodeOwned explode_owned{};
		inline static PunishmentBurn burn{};
		inline static PunishmentDie die{};
		inline static PunishmentUnarm unarm{};
		inline static PunishmentInterrupt interrupt{};
		inline static PunishmentFreeze freeze{};
		inline static PunishmentCower cower{};
		inline static PunishmentFlee flee{};
		inline static PunishmentPush push{};
		inline static PunishmentDelete del{};
		inline static PunishmentMark mark{};
		inline static PunishmentShowModelName showmodelname{};
		inline static PunishmentDrive drive{};
		inline static PunishmentRevive revive{};
		inline static PunishmentRagdoll ragdoll{};
		inline static PunishmentRepair repair{};
		inline static PunishmentWeaken weaken{};

		inline static const Punishment* all[] = {
			&explode_anon,
			&explode_owned,
			&burn,
			&die,
			&unarm,
			&interrupt,
			&freeze,
			&cower,
			&flee,
			&push,
			&drive,
			&revive,
			&ragdoll,
			&repair,
			&weaken,
			&del,
			&mark,
			&showmodelname,
		};

		static void apply(punishment_t punishments, AbstractEntity& ent, PunishmentTrigger trigger);
		static void apply(punishment_t punishments, const v3& pos, AbstractEntity& ent, PunishmentTrigger trigger);
	};
}
