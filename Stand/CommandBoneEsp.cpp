#include "CommandBoneEsp.hpp"

#include "AbstractPlayer.hpp"
#include "AbstractRenderer.hpp"
#include "PedBoneId.hpp"
#include "vector.hpp"

namespace Stand
{
	void CommandBoneEsp::onTick()
	{
		peds.clear();
		AbstractEntity::getAllPeds([this](AbstractEntity&& ped)
		{
			if (filterPed(ped))
			{
				peds.push_back(std::move(ped));
			}
			return true;
		});
	}

	void CommandBoneEsp::draw(const AbstractRenderer& renderer)
	{
		for (AbstractEntity& ped : peds)
		{
			rage::Vector2 neck = ped.getBonePos(SKEL_Neck_1).getScreenPosIgnoreOrigin();
			rage::Vector2 r_elbow = ped.getBonePos(MH_R_Elbow).getScreenPosIgnoreOrigin();
			rage::Vector2 l_elbow = ped.getBonePos(MH_L_Elbow).getScreenPosIgnoreOrigin();
			rage::Vector2 r_hand = ped.getBonePos(SKEL_R_Hand).getScreenPosIgnoreOrigin();
			rage::Vector2 l_hand = ped.getBonePos(SKEL_L_Hand).getScreenPosIgnoreOrigin();
			rage::Vector2 pelvis = ped.getBonePos(SKEL_Pelvis).getScreenPosIgnoreOrigin();
			rage::Vector2 r_calf = ped.getBonePos(SKEL_R_Calf).getScreenPosIgnoreOrigin();
			rage::Vector2 r_foot = ped.getBonePos(SKEL_R_Foot).getScreenPosIgnoreOrigin();
			rage::Vector2 l_calf = ped.getBonePos(SKEL_L_Calf).getScreenPosIgnoreOrigin();
			rage::Vector2 l_foot = ped.getBonePos(SKEL_L_Foot).getScreenPosIgnoreOrigin();

			auto colour = ped.isAPlayer() ? getColour(ped.getPlayer()) : getColour(ped);

			renderer.drawLine(neck, r_elbow, colour);
			renderer.drawLine(neck, l_elbow, colour);
			renderer.drawLine(r_elbow, r_hand, colour);
			renderer.drawLine(l_elbow, l_hand, colour);
			renderer.drawLine(pelvis, neck, colour);
			renderer.drawLine(pelvis, r_calf, colour);
			renderer.drawLine(r_calf, r_foot, colour);
			renderer.drawLine(pelvis, l_calf, colour);
			renderer.drawLine(l_calf, l_foot, colour);
		}
	}
}
