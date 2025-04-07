#pragma once

namespace rage
{
	struct ptxEffectRule
	{
		PAD(0, 0x20) const char* m_name;
		PAD(0x28, 0xA0) float m_viewportCullingSphereRadius;
		PAD(0xA4, 0xB4) float m_colnRange;
	};
	static_assert(offsetof(ptxEffectRule, m_viewportCullingSphereRadius) == 0xA0);
	static_assert(offsetof(ptxEffectRule, m_colnRange) == 0xB4);

	enum ptxEffectInstPlayState
	{
		PTXEFFECTINSTPLAYSTATE_WAITING_TO_ACTIVATE = 0,
		PTXEFFECTINSTPLAYSTATE_ACTIVATED,
		PTXEFFECTINSTPLAYSTATE_PLAYING,
		PTXEFFECTINSTPLAYSTATE_STOPPED_MANUALLY,
		PTXEFFECTINSTPLAYSTATE_STOPPED_NATURALLY,
		PTXEFFECTINSTPLAYSTATE_FINISHED,
		PTXEFFECTINSTPLAYSTATE_DEACTIVATED,

		PTXEFFECTINSTPLAYSTATE_NUM
	};

	struct ptxEffectInst
	{
		/* 0x00 */ ptxEffectRule* m_pEffectRule;
		PAD(0x08, 0xA0) soup::Matrix m_vMtxFinal;
		PAD(0xE0, 0x110) ptxEffectInstPlayState m_playState;

		[[nodiscard]] bool isActive() const noexcept
		{
			return m_playState != PTXEFFECTINSTPLAYSTATE_WAITING_TO_ACTIVATE
				&& m_playState != PTXEFFECTINSTPLAYSTATE_DEACTIVATED
				;
		}
	};
	static_assert(offsetof(ptxEffectInst, m_playState) == 0x110);
}
