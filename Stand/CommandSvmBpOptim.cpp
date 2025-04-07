#if false
#include "CommandSvmBpOptim.hpp"

#include "Gui.hpp"
#include "Hooking.hpp"
#include "FileLogger.hpp"
#include "pointers.hpp"

namespace Stand
{
	static constexpr uint16_t continue_offsets[] = {
		0x00007FF60996B3F2 - 0x00007FF60996B2D4,
		0x00007FF60996B500 - 0x00007FF60996B2D4,
		0x00007FF60996B509 - 0x00007FF60996B2D4,
		0x00007FF60996B666 - 0x00007FF60996B2D4,
		0x00007FF60996B673 - 0x00007FF60996B2D4,
		0x00007FF60996B680 - 0x00007FF60996B2D4,
		0x00007FF60996B690 - 0x00007FF60996B2D4,
		0x00007FF60996B6A9 - 0x00007FF60996B2D4,
		0x00007FF60996B700 - 0x00007FF60996B2D4,
		0x00007FF60996B8F7 - 0x00007FF60996B2D4,
		0x00007FF60996BA51 - 0x00007FF60996B2D4,
		0x00007FF60996BA64 - 0x00007FF60996B2D4,
		0x00007FF60996BA78 - 0x00007FF60996B2D4,
		0x00007FF60996BB46 - 0x00007FF60996B2D4,
		0x00007FF60996BB7D - 0x00007FF60996B2D4,
		0x00007FF60996BBA9 - 0x00007FF60996B2D4,
		0x00007FF60996BBC3 - 0x00007FF60996B2D4,
		0x00007FF60996BC1F - 0x00007FF60996B2D4,
		0x00007FF60996BC50 - 0x00007FF60996B2D4,
		0x00007FF60996BC63 - 0x00007FF60996B2D4,
		0x00007FF60996BE02 - 0x00007FF60996B2D4,
		0x00007FF60996BE3B - 0x00007FF60996B2D4,
		0x00007FF60996BECA - 0x00007FF60996B2D4,
		0x00007FF60996BF99 - 0x00007FF60996B2D4,
		0x00007FF60996BFE9 - 0x00007FF60996B2D4,
		0x00007FF60996C00D - 0x00007FF60996B2D4,
		0x00007FF60996C01B - 0x00007FF60996B2D4,
		0x00007FF60996C026 - 0x00007FF60996B2D4,
		0x00007FF60996C034 - 0x00007FF60996B2D4,
		0x00007FF60996C042 - 0x00007FF60996B2D4,
		0x00007FF60996C050 - 0x00007FF60996B2D4,
		0x00007FF60996C05E - 0x00007FF60996B2D4,
		0x00007FF60996C06C - 0x00007FF60996B2D4,
		0x00007FF60996C07A - 0x00007FF60996B2D4,
		0x00007FF60996C085 - 0x00007FF60996B2D4,
		0x00007FF60996C093 - 0x00007FF60996B2D4,
		0x00007FF60996C0A1 - 0x00007FF60996B2D4,
		0x00007FF60996C0AF - 0x00007FF60996B2D4,
		0x00007FF60996C0BD - 0x00007FF60996B2D4,
		0x00007FF60996C0CB - 0x00007FF60996B2D4,
		0x00007FF60996C0D9 - 0x00007FF60996B2D4,
	};

	extern "C"
	{
		// script_vm_helper.asm
		void script_vm_bpoptim_op_0();
		void script_vm_bpoptim_op_2();
		void script_vm_continue_1();
		void script_vm_continue_2();
		void script_vm_continue_3();
		void script_vm_continue_4();
		void script_vm_continue_5();
		void script_vm_continue_6();
		void script_vm_continue_7();
		void script_vm_continue_8();
		void script_vm_continue_9();
		void script_vm_continue_10();
		void script_vm_continue_11();
		void script_vm_continue_12();
		void script_vm_continue_13();
		void script_vm_continue_14();
		void script_vm_continue_15();
		void script_vm_continue_16();
		void script_vm_continue_17();
		void script_vm_continue_18();
		void script_vm_continue_19();
		void script_vm_continue_20();
		void script_vm_continue_21();
		void script_vm_continue_22();
		void script_vm_continue_23();
		void script_vm_continue_24();
		void script_vm_continue_25();
		void script_vm_continue_26();
		void script_vm_continue_27();
		void script_vm_continue_28();
		void script_vm_continue_29();
		void script_vm_continue_30();
		void script_vm_continue_31();
		void script_vm_continue_32();
		void script_vm_continue_33();
		void script_vm_continue_34();
		void script_vm_continue_35();
		void script_vm_continue_36();
		void script_vm_continue_37();
		void script_vm_continue_38();
		void script_vm_continue_39();
		void script_vm_continue_40();
		void script_vm_continue_41();
	}

	CommandSvmBpOptim::CommandSvmBpOptim(CommandList* parent)
		: CommandToggle(parent, LIT("just get more fps lol"), {}, LIT("...or not? Just give your CPU a few seconds to get used to it."))
	{
	}

	static void check_init_hook(DetourHook& hook, const char* const name, void detour(), const uint16_t svm_offset)
	{
		auto* const target = reinterpret_cast<uint8_t*>(reinterpret_cast<uint64_t>(pointers::script_vm) + svm_offset);
		if (target[0] == 0xE9
			&& target[3] == 0xFF
			&& target[4] == 0xFF
			)
		{
			hook.init(name, detour, target);
		}
		else
		{
			g_logger.log(fmt::format("{} mismatch", name));
		}
	}

	void CommandSvmBpOptim::onEnable(Click& click)
	{
		if (!g_hooking.script_vm_continue_hooks[0].isValid())
		{
#define REGISTER_5BYTE_CONTINUE_HOOK(i) check_init_hook(g_hooking.script_vm_continue_hooks[i - 1], "C-"#i, &script_vm_continue_##i, continue_offsets[i - 1]);
			REGISTER_5BYTE_CONTINUE_HOOK(1);
			REGISTER_5BYTE_CONTINUE_HOOK(2);
			REGISTER_5BYTE_CONTINUE_HOOK(3);
			REGISTER_5BYTE_CONTINUE_HOOK(4);
			REGISTER_5BYTE_CONTINUE_HOOK(5);
			REGISTER_5BYTE_CONTINUE_HOOK(6);
			REGISTER_5BYTE_CONTINUE_HOOK(7);
			REGISTER_5BYTE_CONTINUE_HOOK(8);
			REGISTER_5BYTE_CONTINUE_HOOK(9);
			REGISTER_5BYTE_CONTINUE_HOOK(10);
			REGISTER_5BYTE_CONTINUE_HOOK(11);
			REGISTER_5BYTE_CONTINUE_HOOK(12);
			REGISTER_5BYTE_CONTINUE_HOOK(13);
			REGISTER_5BYTE_CONTINUE_HOOK(14);
			REGISTER_5BYTE_CONTINUE_HOOK(15);
			REGISTER_5BYTE_CONTINUE_HOOK(16);
			REGISTER_5BYTE_CONTINUE_HOOK(17);
			REGISTER_5BYTE_CONTINUE_HOOK(18);
			REGISTER_5BYTE_CONTINUE_HOOK(19);
			REGISTER_5BYTE_CONTINUE_HOOK(20);
			REGISTER_5BYTE_CONTINUE_HOOK(21);
			REGISTER_5BYTE_CONTINUE_HOOK(22);
			REGISTER_5BYTE_CONTINUE_HOOK(23);
			REGISTER_5BYTE_CONTINUE_HOOK(24);
			REGISTER_5BYTE_CONTINUE_HOOK(25);
			REGISTER_5BYTE_CONTINUE_HOOK(26);
			REGISTER_5BYTE_CONTINUE_HOOK(27);
			REGISTER_5BYTE_CONTINUE_HOOK(28);
			REGISTER_5BYTE_CONTINUE_HOOK(29);
			REGISTER_5BYTE_CONTINUE_HOOK(30);
			REGISTER_5BYTE_CONTINUE_HOOK(31);
			REGISTER_5BYTE_CONTINUE_HOOK(32);
			REGISTER_5BYTE_CONTINUE_HOOK(33);
			REGISTER_5BYTE_CONTINUE_HOOK(34);
			REGISTER_5BYTE_CONTINUE_HOOK(35);
			REGISTER_5BYTE_CONTINUE_HOOK(36);
			REGISTER_5BYTE_CONTINUE_HOOK(37);
			REGISTER_5BYTE_CONTINUE_HOOK(38);
			REGISTER_5BYTE_CONTINUE_HOOK(39);
			REGISTER_5BYTE_CONTINUE_HOOK(40);
			REGISTER_5BYTE_CONTINUE_HOOK(41);
		}

		og_op_0 = pointers::script_vm_jump_table[0];
		pointers::script_vm_jump_table[0] = &script_vm_bpoptim_op_0;

		og_op_2 = pointers::script_vm_jump_table[2];
		pointers::script_vm_jump_table[2] = &script_vm_bpoptim_op_2;
		
		for (auto& hook : g_hooking.script_vm_continue_hooks)
		{
			hook.hook(DHERR_LOG);
		}

		g_hooking.enableHooked();
	}

	void CommandSvmBpOptim::onDisable(Click& click)
	{
		pointers::script_vm_jump_table[0] = og_op_0;
		pointers::script_vm_jump_table[2] = og_op_2;

		if (!g_gui.isUnloadPending())
		{
			for (auto& hook : g_hooking.script_vm_continue_hooks)
			{
				hook.disable();
			}
		}
	}
}
#endif
