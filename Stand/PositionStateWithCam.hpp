#pragma once

#include "PositionState.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class PositionStateWithCam : public PositionState
	{
	private:
		v3 cam_pos;
		v3 cam_rot;

		explicit PositionStateWithCam(PositionState&& base, v3&& cam_pos, v3&& cam_rot);

	public:
		[[nodiscard]] static PositionStateWithCam save();

	protected:
		void onRestore() const final;
	};
#pragma pack(pop)
}
