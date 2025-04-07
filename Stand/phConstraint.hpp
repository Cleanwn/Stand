#pragma once

#include "struct_base.hpp"

namespace rage
{
	struct phManifold;

	struct phConstraintBase
	{
		enum Type
		{
			//Basic types (one phContact internally)
			HALFSPACE,		//Removes 1 relative DOF complementarily
			DISTANCE,		//Removes 1 relative DOF complementarily
			SPHERICAL,		//Removes 3 relative DOFs
			ROTATION,		//Removes 1 relative DOF complementarily
			FIXEDROTATION,	//Removes 3 relative DOFs

			ATTACHMENT,		//Attaches two components, optionally with a permitted separation distance and optionally with their rotations locked
			PRISMATIC,
			HINGE,
			FIXED,			//Removes all relative DOFs (or just the three rotational DOFs)
			CYLINDRICAL		//Removes 2 relative DOFs
		};

		struct Params
		{
			PAD(0, 0x2C) const Type type;
		};
		static_assert(offsetof(Params, type) == 0x2C);

		virtual ~phConstraintBase() = default;
		virtual void _0x08() = 0;
		virtual void _0x10() = 0;
		virtual void _0x18() = 0;
		virtual void _0x20() = 0;
		virtual void _0x28() = 0;
		virtual void _0x30() = 0;
		virtual void _0x38() = 0;
		virtual void _0x40() = 0;
		virtual void _0x48() = 0;
		virtual void DisableManifolds() = 0; // 0x50

		PAD(0x08, 0x30);
	};
	static_assert(sizeof(phConstraintBase) == 0x30);

	struct phConstraintAttachment : public phConstraintBase
	{
		/* 0x30 */ float m_MaxSeparation;
		/* 0x38 */ phConstraintBase* m_TranslationConstraint;
		/* 0x40 */ phConstraintBase* m_RotationConstraint; // phConstraintFixedRotation
	};
	static_assert(sizeof(phConstraintAttachment) == 0x48);

	struct phConstraintFixed : public phConstraintBase
	{
		/* 0x30 */ phManifold* m_TranslationManifold;
		/* 0x38 */ phManifold* m_RotationManifold;
	};
	static_assert(sizeof(phConstraintFixed) == 0x38 + 8);

	struct phConstraintFixedRotation : public phConstraintBase
	{
		/* 0x30 */ phManifold* m_RotationManifold;
	};
	static_assert(sizeof(phConstraintFixedRotation) == 0x30 + 8);

	struct phConstraintPrismatic : public phConstraintBase
	{
		enum ePrismaticConstraintManifolds
		{
			ePCM_SLIDEAXIS_MINLIMIT,
			ePCM_SLIDEAXIS_MAXLIMIT,
			ePCM_PERPAXIS1_MINLIMIT,
			ePCM_PERPAXIS1_MAXLIMIT,
			ePCM_PERPAXIS2_MINLIMIT,
			ePCM_PERPAXIS2_MAXLIMIT,
			ePCM_COUNT,
		};

		/* 0x30 */ phManifold* m_Manifolds[ePCM_COUNT];
	};
}
