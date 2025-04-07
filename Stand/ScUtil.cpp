#include "ScUtil.hpp"

#include "players_interface.hpp"
#include "pointers.hpp"
#include "rlPc.hpp"

namespace rgsc
{
	template <typename T>
	struct WeakString
	{
		/* 0x00 */ const T* mData;
		/* 0x08 */ size_t mLength;
		/* 0x10 */ bool mAllocatedOnHeap;
	};

	using WideString = WeakString<wchar_t>;

	struct Variant
	{
		enum Type
		{
			JSSTRING,
			JSDOUBLE,
			JSINTEGER,
			JSBOOLEAN,
			JSNULL,
			JSEMPTYOBJECT,
			JSEMPTYARRAY,
			JSBINDFUNC,
			JSBINDSYNCFUNC
		};

		union
		{
			WideString mStrPointer;
			double mDoubleValue;
		};
		Type mType = JSNULL;
	};
	static_assert(offsetof(Variant, mType) == 0x18);

	struct Script
	{
		struct Info
		{
			WeakString<wchar_t>* m_FuncName;
			Variant* m_Args;
			Variant m_Result;
		};
	};
}

namespace Stand
{
	bool ScUtil::isBlocked(int64_t rid)
	{
#if false // same thing, just more convoluted
		if (pointers::rgsc_Script_CallFunction)
		{
			rgsc::WeakString<wchar_t> name;
			name.mData = L"RGSC_FRIEND_IS_BLOCKED";
			name.mLength = 22;
			name.mAllocatedOnHeap = false;

			auto json = fmt::format(LR"({{"RockstarId":{}}})", rid);

			rgsc::Variant arg;
			arg.mStrPointer.mData = json.data();
			arg.mStrPointer.mLength = json.size();
			arg.mStrPointer.mAllocatedOnHeap = false;
			arg.mType = rgsc::Variant::JSSTRING;

			rgsc::Script::Info info;
			info.m_FuncName = &name;
			info.m_Args = &arg;

			pointers::rgsc_Script_CallFunction(&info);

			return info.m_Result.mDoubleValue != 0; // Yeah, this is how they do bools.
		}
#endif
		return pointers::g_rlPc->player_manager->IsBlocked(rid);
	}

	void ScUtil::block(int64_t rid)
	{
		if (pointers::rgsc_Script_CallFunction)
		{
			rgsc::WeakString<wchar_t> name;
			name.mData = L"RGSC_FRIEND_BLOCK_PLAYER";
			name.mLength = 24;
			name.mAllocatedOnHeap = false;

			auto json = fmt::format(LR"({{"RockstarId":{}}})", rid);

			rgsc::Variant arg;
			arg.mStrPointer.mData = json.data();
			arg.mStrPointer.mLength = json.size();
			arg.mStrPointer.mAllocatedOnHeap = false;
			arg.mType = rgsc::Variant::JSSTRING;

			rgsc::Script::Info info;
			info.m_FuncName = &name;
			info.m_Args = &arg;

			pointers::rgsc_Script_CallFunction(&info);
		}
	}

	void ScUtil::unblock(int64_t rid)
	{
		if (pointers::rgsc_Script_CallFunction)
		{
			rgsc::WeakString<wchar_t> name;
			name.mData = L"RGSC_FRIEND_UNBLOCK_PLAYER";
			name.mLength = 26;
			name.mAllocatedOnHeap = false;

			auto json = fmt::format(LR"({{"RockstarId":{}}})", rid);

			rgsc::Variant arg;
			arg.mStrPointer.mData = json.data();
			arg.mStrPointer.mLength = json.size();
			arg.mStrPointer.mAllocatedOnHeap = false;
			arg.mType = rgsc::Variant::JSSTRING;

			rgsc::Script::Info info;
			info.m_FuncName = &name;
			info.m_Args = &arg;

			pointers::rgsc_Script_CallFunction(&info);
		}
	}
}
