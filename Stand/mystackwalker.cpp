#include "mystackwalker.hpp"

#include <fmt/core.h>

#include "Exceptional.hpp"
#include "FileLogger.hpp"
#include "main.hpp"
#include "PeHeader.hpp"
#include "PointerNames.hpp"

namespace Stand
{
	MyStackWalker::MyStackWalker()
		: StackWalker()
	{
		m_options &= ~SymUseSymSrv;
	}

	MyStackWalker::~MyStackWalker()
	{
		// C++ guarantees that the super-destructor is called automatically.
		// However, C++ does not generate this destructor automatically.
	}

	std::shared_ptr<MyStackWalker> MyStackWalker::getInstance()
	{
		std::shared_ptr<MyStackWalker> ret = g_mystackwalker_inst;
		if (!ret)
		{
			ret = std::make_shared<MyStackWalker>();
			g_mystackwalker_inst = ret;
		}
		return ret;
	}

	void MyStackWalker::OnCallstackEntry(CallstackEntryType type, CallstackEntry& entry)
	{
#if !SSW_CALLSTACK_ONLY
		if (entry.offset == 0 && type != StackWalker::firstEntry) // RIP could be nullptr in case of access violation due to jmp to nullptr
		{
			callback_may_be_corrupted = true;
		}
#endif
		callstack_entries.emplace_back((void*)entry.offset);
	}

	HANDLE MyStackWalker::getProcessHandle() const
	{
		return m_hProcess;
	}

	void MyStackWalker::generateStackTrace(const ErrorInfo& err, const std::string& prefix)
	{
		PeHeader::unveil();
#if HIDE_MODULE
		g_module_hider.disable();
#endif
		if (err.is_exp)
		{
			ShowCallstack(GetCurrentThread(), err.exp->ContextRecord);
			diveIntoStack((void**)err.exp->ContextRecord->Rsp, err.exp->ExceptionRecord->ExceptionAddress, prefix);
		}
		else
		{
			ShowCallstack();
			diveIntoStack(err.sp, std::nullopt, prefix);
		}
		enable_module_hider_if_applicable();
		PeHeader::veil();
	}

	void MyStackWalker::diveIntoStack(void** sp, std::optional<void*>&& opt_ip, const std::string& prefix)
	{
#if !SSW_CALLSTACK_ONLY
		if (!callback_may_be_corrupted)
#endif
		{
			for (const auto& addr : callstack_entries)
			{
				std::string address = PointerNames::format(addr);
				address.insert(0, prefix);
				g_logger.log(std::move(address));
			}
		}
#if !SSW_CALLSTACK_ONLY
		else
		{
			if (opt_ip.has_value())
			{
				g_logger.log(std::string(prefix).append("RIP = ").append(PointerNames::format(opt_ip.value())));
			}
			NT_TIB* tib = (NT_TIB*)NtCurrentTeb(); // TIB is first field on TEB: http://undocumented.ntinternals.net/index.html?page=UserMode%2FUndocumented%20Functions%2FNT%20Objects%2FThread%2FTEB.html
			//g_logger.log(fmt::format("Stack region: {} - {}", tib->StackLimit, tib->StackBase));
			int skipped_nulls = 0;
			while (sp != tib->StackBase)
			{
				void* const addr = *(sp++);
				if (addr == nullptr)
				{
					skipped_nulls++;
					continue;
				}
				if (skipped_nulls != 0)
				{
					if (skipped_nulls == 1)
					{
						g_logger.log(std::string(prefix).append("0000000000000000"));
					}
					else
					{
						g_logger.log(std::string(prefix).append(fmt::format("< Skipped {} null values >", skipped_nulls)));
					}
					skipped_nulls = 0;
				}
				std::string message(prefix);
				for (int i = 0; i < callstack_entries.size(); i++)
				{
					if (callstack_entries[i] == addr)
					{
						message.append(fmt::format("[RET+{}] ", i));
						break;
					}
				}
				message.append(PointerNames::format(addr));
				g_logger.log(std::move(message));
			}
			callback_may_be_corrupted = false;
		}
#endif
		callstack_entries.clear();
	}
}
