#pragma once

#include "atArray.hpp"

namespace rage
{
	struct characterClothController
	{
		PAD(0x00, 0xB0) atArray<unsigned int> m_BoneIndexMap;
	};

	struct phClothData
	{
		PAD(0x00, 0x30) atArray<Vec3V> m_VertexPositions;

		Vec3V* GetVertexPointer() { return &(m_VertexPositions[0]); }
	};

	struct phVerletCloth
	{
		PAD(0x00, 0x50) phClothData m_ClothData;

		const phClothData& GetClothData() const { return m_ClothData; }
		phClothData& GetClothData() { return m_ClothData; }
	};

	struct clothController
	{
		PAD(0x00, 0x20) phVerletCloth* m_Cloth[/*LOD_COUNT*/ 4];
		PAD(0x40, 0x56) char m_LOD;

		char GetLOD() const { return m_LOD; }

		phVerletCloth* GetCloth(int lodIndex) const { return m_Cloth[lodIndex]; }
	};
	static_assert(offsetof(clothController, m_LOD) == 0x56);

	struct environmentCloth
	{
		PAD(0x00, 0x28) clothController* m_ClothController;

		[[nodiscard]] phVerletCloth* GetCloth()
		{
			const int lodIndex = m_ClothController->GetLOD();
			return m_ClothController->GetCloth(lodIndex);
		}
	};
}
