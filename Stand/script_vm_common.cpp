#include "script_vm_common.hpp"

#include "PointerNames.hpp"
#include "ScriptMgr.hpp"
#include "Util.hpp"

namespace Stand
{
	const char* opcode_names[] = {
		"Nop",
		"iAdd",
		"iSub",
		"iMult",
		"iDiv",
		"iMod",
		"iNot",
		"iNeg",
		"iCmpEq",
		"iCmpNe",
		"iCmpGt",
		"iCmpGe",
		"iCmpLt",
		"iCmpLe",
		"fAdd",
		"fSub",
		"fMult",
		"fDiv",
		"fMod",
		"fNeg",
		"fCmpEq",
		"fCmpNe",
		"fCmpGt",
		"fCmpGe",
		"fCmpLt",
		"fCmpLe",
		"vAdd",
		"vSub",
		"vMult",
		"vDiv",
		"vNeg",
		"And",
		"Or",
		"Xor",
		"ItoF",
		"FtoI",
		"FtoV",
		"iPushByte1",
		"iPushByte2",
		"iPushByte3",
		"iPushInt",
		"fPush",
		"dup",
		"pop",
		"Native",
		"Enter",
		"Return",
		"pGet",
		"pSet",
		"pPeekSet",
		"ToStack",
		"FromStack",
		"pArray1",
		"ArrayGet1",
		"ArraySet1",
		"pFrame1",
		"GetFrame1",
		"SetFrame1",
		"pStatic1",
		"StaticGet1",
		"StaticSet1",
		"Add1",
		"Mult1",
		"pStructStack",
		"pStruct1",
		"GetStruct1",
		"SetStruct1",
		"iPushShort",
		"Add2",
		"Mult2",
		"pStruct2",
		"GetStruct2",
		"SetStruct2",
		"pArray2",
		"ArrayGet2",
		"ArraySet2",
		"pFrame2",
		"GetFrame2",
		"SetFrame2",
		"pStatic2",
		"StaticGet2",
		"StaticSet2",
		"pGlobal2",
		"GlobalGet2",
		"GlobalSet2",
		"Jump",
		"JumpFalse",
		"JumpNe",
		"JumpEq",
		"JumpLe",
		"JumpLt",
		"JumpGe",
		"JumpGt",
		"Call",
		"pStatic3",
		"StaticGet3",
		"StaticSet3",
		"pGlobal3",
		"GlobalGet3",
		"GlobalSet3",
		"iPushI24",
		"Switch",
		"PushString",
		"GetHash",
		"StrCopy",
		"ItoS",
		"StrConCat",
		"StrConCatInt",
		"MemCopy",
		"Catch",
		"Throw",
		"pCall",
		"iPush_n1",
		"iPush_0",
		"iPush_1",
		"iPush_2",
		"iPush_3",
		"iPush_4",
		"iPush_5",
		"iPush_6",
		"iPush_7",
		"fPush_n1",
		"fPush_0",
		"fPush_1",
		"fPush_2",
		"fPush_3",
		"fPush_4",
		"fPush_5",
		"fPush_6",
		"fPush_7",
		"Bittest",
	};

	uint32_t getOpcodeArgLength(const uint8_t* op_ptr)
	{
		switch(*op_ptr)
		{
		case 37:
			return 1;
		case 38:
			return 2;
		case 39:
			return 3;
		case 40:
		case 41:
			return 4;
		case 44:
			return 3;
		case 45:
			return 4 + (*(op_ptr + 4));
		case 46:
			return 2;
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
		case 58:
		case 59:
		case 60:
		case 61:
		case 62:
		case 64:
		case 65:
		case 66:
			return 1;
		case 67:
		case 68:
		case 69:
		case 70:
		case 71:
		case 72:
		case 73:
		case 74:
		case 75:
		case 76:
		case 77:
		case 78:
		case 79:
		case 80:
		case 81:
		case 82:
		case 83:
		case 84:
		case 85:
		case 86:
		case 87:
		case 88:
		case 89:
		case 90:
		case 91:
		case 92:
			return 2;
		case Call:
		case pStatic3:
		case StaticGet3:
		case StaticSet3:
		case pGlobal3:
		case GlobalGet3:
		case GlobalSet3:
		case iPushI24:
			return 3;
		case Switch:
			return 1 + ((*(op_ptr + 1)) * 6);
		case StrCopy:
		case ItoS:
		case StrConCat:
		case StrConCatInt:
			return 1;
		}
		return 0;
	}

	std::string native_entrypoint_to_string(const rage::scrNativeHandler entrypoint)
	{
		auto entry = g_script_mgr.reverse_entrypoint_map.find(entrypoint);
		if (entry == g_script_mgr.reverse_entrypoint_map.end())
		{
			return std::move(std::string("@ ").append(PointerNames::format(entrypoint)));
		}
		std::string str{};
		for (auto i = entry->second.begin(); i != entry->second.end(); ++i)
		{
			if (i != entry->second.begin())
			{
				str.append(" / ");
			}
			str.append("0x").append(Util::to_padded_hex_string(*i));
		}
		return str;
	}

	std::vector<rage::scrNativeHash> native_entrypoint_to_hashes(const rage::scrNativeHandler entrypoint)
	{
		auto entry = g_script_mgr.reverse_entrypoint_map.find(entrypoint);
		if (entry != g_script_mgr.reverse_entrypoint_map.end())
		{
			return entry->second;
		}
		return {};
	}
}
