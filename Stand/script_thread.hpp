#pragma once

#include <cstdint>
#include <functional>

#include "fwddecl.hpp"

#include "tlsContext.hpp"

namespace rage
{
	constexpr auto MAX_CALLSTACK = 16;

	class scrThreadContext // rage::scrThread::Serialized
	{
	public:
		uint32_t m_thread_id;           // 0x00 - GET_ID_OF_THIS_THREAD
		int32_t m_script_hash;          // 0x04
		uint32_t m_state;               // 0x08
		uint32_t m_instruction_pointer; // 0x0C
		uint32_t m_frame_pointer;       // 0x10
		uint32_t m_stack_pointer;       // 0x14
		float m_timer_a;                // 0x18
		float m_timer_b;                // 0x1C
		float m_timer_c;                // 0x20
		char m_padding1[0x2C];          // 0x24
		/* 0x50 */ int32_t m_stack_size;
		/* 0x54 */ int32_t m_CatchPC;
		/* 0x58 */ int32_t m_CatchFP;
		/* 0x5C */ int32_t m_CatchSP;
		/* 0x60 */ int32_t m_Priority;
		/* 0x64 */ int8_t m_CallDepth;
		/* 0x68 */ int m_CallStack[MAX_CALLSTACK];

		int64_t* getStackPtr(void* stack) noexcept;
		uint8_t* getCodePtr();
	};
	static_assert(sizeof(scrThreadContext) == 0xA8);

	class scrThread
	{
	public:
		enum State : uint32_t
		{
			RUNNING,
			BLOCKED,
			ABORTED,
			HALTED,

			STAND_ERROR,
			STAND_INFLOOP = 69, // hard-coded in script_vm_helper.asm
		};

		virtual ~scrThread() = default;                                                 // 0 (0x00)
		virtual void reset(uint32_t script_hash, void *args, uint32_t arg_count) = 0;   // 1 (0x08)
		virtual State Run() = 0;                                                        // 2 (0x10)
		virtual State tick(uint32_t ops_to_execute) = 0;                                // 3 (0x18)
		virtual void kill() = 0;                                                        // 4 (0x20)

		[[nodiscard]] static scrThread* get()
		{
			return rage::tlsContext::get()->m_script_thread;
		}

		/* 0x08 */ scrThreadContext m_context;
		/* 0xB0 */ void* m_stack;
		PAD(0xB8, 0xC8) const char* m_AbortReason;
		/* 0xD0 */ hash_t name_hash;
		/* 0xD4 */ char m_name[0x40];

		// The rest should be in GtaThread...
		PAD(0x114, 0x118) scriptHandler* m_handler;
		/* 0x120 */ scriptHandlerNetComponent* m_net_component_unreliable;

		[[nodiscard]] int64_t* getStackPtr() noexcept;
		[[nodiscard]] scrValue* getStackPtrAsScrValue() noexcept;
		[[nodiscard]] Stand::Label getLabel() const noexcept;
		[[nodiscard]] static Stand::Label getLabel(const hash_t hash) noexcept;
		
		[[nodiscard]] scriptHandlerNetComponent* getNetComponent() const noexcept;
		[[nodiscard]] ::Stand::AbstractPlayer getHost() const;
		[[nodiscard]] bool hostedByUser() const;

		void spoofAs(const std::function<void()>& func);
		void spoofAsOpt(const std::function<void()>& func);

		bool snatchScriptHost();
	private:
		bool snatchScriptHostInner();
	public:
		bool fiberSnatchScriptHost();
	};
	static_assert(sizeof(scrThread) == 0x120 + 8);
}

class GtaThread : public rage::scrThread
{
public:
	// noted offsets don't match actual offsets, might be outdated
	int32_t m_script_hash;                      // 0x120
	char m_padding3[0x14];                      // 0x124
	int32_t m_instand_id;                       // 0x138
	char m_padding4[0x04];                      // 0x13C
	uint8_t m_flag1;                            // 0x140
	bool m_safe_for_network_game;               // 0x141
	char m_padding5[0x02];                      // 0x142
	bool m_is_minigame_script;                  // 0x144
	char m_padding6[0x02];                      // 0x145
	bool m_can_be_paused;                       // 0x147
	bool m_can_remove_blips_from_other_scripts; // 0x148
	char m_padding7[0x0F];                      // 0x149

	[[nodiscard]] static GtaThread* fromId(const uint32_t id);
	[[nodiscard]] static GtaThread* fromHash(const int32_t hash);
};
//static_assert(sizeof(GtaThread) == 0x158);
