#include "AntiAntiCheat.hpp"

#include <timeapi.h>

#include <soup/rand.hpp>

#include "conf_antianticheat.hpp"

#include "AcThingData.hpp"
#include "atStringHash.hpp"
#include "CNetworkSession.hpp"
#include "gameSkeleton.hpp"
#include "Hooking.hpp"
#include "main.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "rlRosCredentials.hpp"
#include "ScriptGlobal.hpp"
#include "ScriptLocal.hpp"
#include "script_thread.hpp"

#ifdef STAND_DEBUG
#include "Util.hpp"
#endif

#define ALLOWING_AC_THING_TO_RUN false

namespace Stand
{
	void AntiAntiCheat::onEnteredOnline() // TC_SCRIPT_NOYIELD
	{
		if (NativeCallContext::canInvoke(0x767FBC2AC802EF3D))
		{
			int val = -1;
			STATS::STAT_GET_INT(ATSTRINGHASH("MINORITY_REPORT"), &val, -1);
			if (val != 0)
			{
#ifdef STAND_DEBUG
				Util::toast(fmt::format("[AntiAntiCheat] MR is {}, setting to 0.", val));
#endif
				STATS::STAT_SET_INT(ATSTRINGHASH("MINORITY_REPORT"), 0, TRUE);
			}
		}
	}

	void AntiAntiCheat::onTick() // TC_SCRIPT_NOYIELD
	{
		SOUP_IF_LIKELY (auto t = GtaThread::fromHash(ATSTRINGHASH("shop_controller")))
		{
			// Block shop_controller from issuing the following basket items:
			// - SERVICE_EARN_JBONUS_SE
			// - SERVICE_EARN_JBONUS_NO_FALL
			// - SERVICE_EARN_JBONUS_NOT_SEEN
			// - SERVICE_EARN_JBONUS_NO_DEATH
			// - SERVICE_EARN_JBONUS_MODEL
			*ScriptLocal(t, LOCAL_SHOPCTRL_CHEAT_STATE).as<int*>() = 69;
		}

		// g_bKickPlayerForInvalidTransaction
		ScriptGlobal(GLOBAL_AC_BAIL).set<BOOL>(FALSE);

#if PREVENT_PGSTREAMER_CRASH
		if (pointers::rage_pgStreamer_smTamperCrash != nullptr)
		{
			SOUP_IF_UNLIKELY (pointers::rage_pgStreamer_smTamperCrash->Get() > 0)
			{
#ifdef STAND_DEBUG
				Util::toast("Prevented anti-cheat crash 1", TOAST_ALL);
#endif
				pointers::rage_pgStreamer_smTamperCrash->Set(0);
			}
		}
#endif

#if PREVENT_GBUFFER_CRASH
		if (pointers::GBuffer_m_Attached != nullptr)
		{
			SOUP_IF_UNLIKELY (*pointers::GBuffer_m_Attached != 0)
			{
#ifdef STAND_DEBUG
				Util::toast("Prevented anti-cheat crash 2", TOAST_ALL);
#endif
				*pointers::GBuffer_m_Attached = 0;
			}
		}
#endif

#if PREVENT_GRCDEVICE_CRASH
		if (pointers::rage_grcDevice_sm_KillSwitch != nullptr)
		{
			SOUP_IF_UNLIKELY ((*pointers::rage_grcDevice_sm_KillSwitch & 0x10101010) != 0)
			{
#ifdef STAND_DEBUG
				Util::toast("Prevented anti-cheat crash 3", TOAST_ALL);
#endif
				*pointers::rage_grcDevice_sm_KillSwitch = 0;
			}
		}
#endif

#if !ALLOWING_AC_THING_TO_RUN
		// We don't allow it to run, so keep its timers updated.
		if (const auto ac_thing_data = (*pointers::ac_thing_data))
		{
			const uint32_t time = timeGetTime();

			if (time - ac_thing_data->timer_1.Get() > ac_thing_data->timer_1_interval.Get())
			{
				ac_thing_data->timer_1.Set(time);
			}
			if (time - ac_thing_data->mm_metric_last_check > ac_thing_data->mm_metric_interval)
			{
				ac_thing_data->mm_metric_last_check = time;
			}
		}
#endif
	}

	static void ensureModuleIsMarkedAsReported(uint32_t hash)
	{
		if (!(*pointers::ac_thing_data)->wasModuleReported(hash))
		{
			(*pointers::ac_thing_data)->markModuleAsReported(hash);
		}
	}

	void AntiAntiCheat::onPresent()
	{
#if ALLOWING_AC_THING_TO_RUN
		if (*pointers::ac_thing_data)
		{
#ifdef STAND_DEBUG
			/*if (!(*pointers::ac_thing_data)->wasModuleReported(AcThingData::getModuleHash(StringUtils::utf8_to_utf16("GDI32.dll"))))
			{
				g_renderer.drawCentredTextThisFrame(L"AntiAntiCheat may be misconfigured");
			}*/
#endif

			// Ensure our DLL is marked as reported - we want to do this rather fast but need game TLS for this, so doing it on present.
			const auto our_dll_hash = AcThingData::getModuleHash(StringUtils::utf8_to_utf16(g_stand_dll_file_name));
			if (!(*pointers::ac_thing_data)->wasModuleReported(our_dll_hash))
			{
				(*pointers::ac_thing_data)->markModuleAsReported(our_dll_hash);

				// Same thing with VC runtime DLLs, as we may be causing them to get loaded.
				ensureModuleIsMarkedAsReported(1640017720); // MSVCP140.dll
				ensureModuleIsMarkedAsReported(505062501); // VCRUNTIME140.dll
				ensureModuleIsMarkedAsReported(1376135695); // VCRUNTIME140_1.dll
			}
		}
#endif
	}

	void AntiAntiCheat::onPostTick() // TC_SCRIPT_NOYIELD
	{
		// Needs to be here to bypass the check when going online for the first time
		(*pointers::network_session)->m_dinputCount.Set(0);
	}

	static int64_t spoof_to_acct_ids[] = {
		102776998,
		113528701,
		119231523,
		123598699,
		174154297,
		196184982,
		211456943,
	};

	void AntiAntiCheat::disableScriptHostTracking()
	{
		static int64_t spoof_to_acct_id = ARRAY_RAND(spoof_to_acct_ids);

		if (pointers::ros_credentials->account_id != spoof_to_acct_id)
		{
			pointers::ros_credentials->account_id = spoof_to_acct_id;
			g_hooking.forceUpdateGamerDataNode();
		}
	}
}
