#pragma once

#include "rgsc_common.hpp"

namespace rgsc
{
	struct IProfile : public IRgscUnknown
	{
	};

	inline const RGSC_IID IID_IProfileV1 = { 0x5c0c8f48, 0x7f01, 0x4680, {0x99, 0xa1, 0x5f, 0x8d, 0x52, 0xc1, 0x62, 0xb8} };

	struct IProfileV1 : public IProfile
	{
		virtual void RGSC_CALL SetProfileId(int64_t profileId) = 0;
		virtual void RGSC_CALL SetNickname(const char* nickname) = 0;
		virtual void RGSC_CALL SetProfileUniqueKey(const uint8_t* key) = 0;
		virtual void RGSC_CALL SetIsOfflineProfile(bool isOfflineProfile) = 0;
		virtual void RGSC_CALL SetRosTicket(const char* ticket) = 0;
	};

	inline const RGSC_IID IID_IProfileV2 = { 0x43412d5, 0xaedd, 0x486b, {0xbc, 0xde, 0x71, 0xfd, 0xfb, 0x83, 0x2, 0x1f} };

	struct IProfileV2 : public IProfileV1
	{
		virtual void RGSC_CALL SetSocialClubEmail(const char* email) = 0;
		virtual void RGSC_CALL SetSocialClubPassword(const char* password) = 0;
	};

	struct IProfileV3 : public IProfileV2
	{
	};

	struct IProfileManager : public IRgscUnknown
	{
	};

	struct IProfileManagerV1 : public IProfileManager
	{
		virtual bool RGSC_CALL IsSignedIn() = 0;
		virtual bool RGSC_CALL IsOnline() = 0;
		virtual RGSC_HRESULT RGSC_CALL GetSignInInfo(IProfile* profile);
	};

#pragma pack(push, 1)
	struct rlPcProfile
	{
		PAD(0x00, 0x10) int64_t profileId; // this is constantly checked against rlPresence_GamerPresences_0_GamerInfo->handle.rockstar_id, they must match!
		PAD(0x18, 0x38) char nickname[17];
	};
	static_assert(sizeof(rlPcProfile) == 0x38 + 17);

	struct rlPcProfileManager : public IProfileManagerV1
	{
		INIT_PAD(IProfileManagerV1, 0x10) rlPcProfile profile;
	};
#pragma pack(pop)
}

namespace Stand
{
	struct RgscProfile : public rgsc::IProfileV2
	{
		int64_t m_profileId;
		const char* m_nickname;
		const char* m_email;

		RGSC_HRESULT RGSC_CALL QueryInterface(RGSC_REFIID riid, void** ppvObject) final
		{
			rgsc::IRgscUnknown* pUnknown = NULL;

			if (ppvObject == NULL)
			{
				return RGSC_INVALIDARG;
			}

			if (riid == rgsc::IID_IRgscUnknown)
			{
				pUnknown = static_cast<rgsc::IProfile*>(this);
			}
			else if (riid == rgsc::IID_IProfileV1)
			{
				pUnknown = static_cast<rgsc::IProfileV1*>(this);
			}
			else if (riid == rgsc::IID_IProfileV2)
			{
				pUnknown = static_cast<rgsc::IProfileV2*>(this);
			}

			*ppvObject = pUnknown;
			if (pUnknown == NULL)
			{
				return RGSC_NOINTERFACE;
			}

			return RGSC_OK;
		}

		void RGSC_CALL SetProfileId(int64_t profileId) final
		{
			m_profileId = profileId;
		}

		void RGSC_CALL SetNickname(const char* nickname) final
		{
			m_nickname = nickname;
		}

		void RGSC_CALL SetProfileUniqueKey(const uint8_t* key) final {}
		void RGSC_CALL SetIsOfflineProfile(bool isOfflineProfile) final {}
		void RGSC_CALL SetRosTicket(const char* ticket) final {}

		void RGSC_CALL SetSocialClubEmail(const char* email) final
		{
			m_email = email;
		}

		void RGSC_CALL SetSocialClubPassword(const char* password) final {} // just provided with nullptr
	};
}
