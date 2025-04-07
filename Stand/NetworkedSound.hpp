#pragma once

namespace Stand
{
	class NetworkedSound
	{
	private:
		hash_t m_soundNameHash;
		hash_t m_setNameHash;

	public:
		uint8_t m_SoundID;

		NetworkedSound(hash_t m_soundNameHash, hash_t m_setNameHash, uint8_t m_SoundID)
			: m_soundNameHash(m_soundNameHash), m_setNameHash(m_setNameHash), m_SoundID(m_SoundID)
		{
		}

		[[nodiscard]] bool isSameSoundData(hash_t name, hash_t dict) const noexcept
		{
			return m_soundNameHash == name && m_setNameHash == dict;
		}
	};
}