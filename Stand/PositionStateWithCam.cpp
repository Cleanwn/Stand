#include "PositionStateWithCam.hpp"

#include "Camgr.hpp"
#include "natives.hpp"

namespace Stand
{
    PositionStateWithCam::PositionStateWithCam(PositionState&& base, v3&& cam_pos, v3&& cam_rot)
        : PositionState(std::move(base)), cam_pos(std::move(cam_pos)), cam_rot(std::move(cam_rot))
    {
    }

    PositionStateWithCam PositionStateWithCam::save()
    {
        return PositionStateWithCam(
            PositionState::save(),
            CAM::GET_FINAL_RENDERED_CAM_COORD(),
            CAM::GET_FINAL_RENDERED_CAM_ROT(2)
        );
    }

    void PositionStateWithCam::onRestore() const
    {
		Camgr::lock_pos(cam_pos);
		Camgr::lock_rot(cam_rot);
        HUD::LOCK_MINIMAP_ANGLE((int)cam_rot.heading());
    }
}
