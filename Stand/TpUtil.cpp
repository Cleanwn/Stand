#include "TpUtil.hpp"

#include "AbstractEntity.hpp"
#include "CommandTeleportParticle.hpp"
#include "get_current_time_millis.hpp"
#include "get_ground_z.hpp"
#include "FiberPool.hpp"
#include "Gui.hpp"
#include "natives.hpp"
#include "PositionState.hpp"
#include "Renderer.hpp"
#include "Script.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"

namespace Stand
{
	Vector2Plus TpUtil::followRedirects(const Vector2Plus& coords, bool in_vehicle)
	{
		using namespace std;
		using V3 = v3;

		constexpr pair<pair<float, float>, pair<V3, V3>> redirect_pairs[] = {
			pair(pair(1697.979126f, 3753.200439f), pair(V3(1701.130859f, 3750.561523f, 34.367294f), V3(1697.979126f, 3753.200439f, 34.701405f))),		// Ammu-Nation 1
			pair(pair(245.271103f, -45.812595f), pair(V3(239.232605f, -43.767391f, 69.180779f), V3(245.271103f, -45.812595f, 69.929283f))),			// Ammu-Nation 2
			pair(pair(844.124817f, -1025.570679f), pair(V3(843.955627f, -1018.552734f, 27.022779f), V3(844.124817f, -1025.570679f, 28.183146f))),		// Ammu-Nation 3
			pair(pair(-325.886658f, 6077.020020f), pair(V3(-321.587433f, 6073.997070f, 30.747419f), V3(-325.886658f, 6077.020020f, 31.442123f))),		// Ammu-Nation 4
			pair(pair(-664.217773f, -943.364624f), pair(V3(-663.850891f, -949.329773f, 21.019037f), V3(-664.217773f, -943.364624f, 21.816385f))),		// Ammu-Nation 5
			pair(pair(-1313.927856f, -390.968933f), pair(V3(-1318.658447f, -389.834839f, 35.908642f), V3(-1313.927856f, -390.968933f, 36.684586f))),	// Ammu-Nation 6
			pair(pair(-3165.230713f, 1082.855103f), pair(V3(-3160.738281f, 1081.890137f, 20.234903f), V3(-3165.230713f, 1082.855103f, 20.825485f))),	// Ammu-Nation 7
			pair(pair(2569.611572f, 302.575989f), pair(V3(2569.598145f, 310.108673f, 107.942314f), V3(2569.611572f, 302.575989f, 108.724243f))),		// Ammu-Nation 8
			pair(pair(-1111.251831f, 2688.476318f), pair(V3(-1111.186401f, 2688.437500f, 18.085384f), V3(-1113.466187f, 2692.016113f, 18.554150f))),	// Ammu-Nation 9
			pair(pair(811.869873f, -2149.101563f), pair(V3(812.627136f, -2143.387207f, 28.766768f), V3(811.869873f, -2149.101563f, 29.619356f))),		// Ammu-Nation with Range 1
			pair(pair(17.680408f, -1114.287964f), pair(V3(14.320112f, -1123.469849f, 28.262930f), V3(17.680408f, -1114.287964f, 29.798706f))),		// Ammu-Nation with Range 2
			pair(pair(321.6098f, 179.4165f), pair(V3(320.24875f, 174.75006f, 103.742386f), V3(322.58475f, 180.0087f, 103.58657f))),					// Tattoo Parlor 1
			pair(pair(1861.6853f, 3750.0798f), pair(V3(1855.9877f, 3747.6792f, 32.997875f), V3(1861.5864f, 3750.2366f, 33.031868f))),					// Tattoo Parlor 2
			pair(pair(-290.1603f, 6199.0947f), pair(V3(-287.07327f, 6201.5464f, 31.473038f), V3(-290.53964f, 6198.3516f, 31.487114f))),				// Tattoo Parlor 3
			pair(pair(-1153.9481f, -1425.0186f), pair(V3(-1156.1406f, -1419.0974f, 4.823194f), V3(-1153.4972f, -1425.688f, 4.954457f))),				// Tattoo Parlor 4
			pair(pair(1322.4547f, -1651.1252f), pair(V3(1319.1198f, -1646.931f, 52.145573f), V3(1322.8029f, -1651.6593f, 52.275066f))),				// Tattoo Parlor 5
			pair(pair(-3169.4204f, 1074.7272f), pair(V3(-3164.598f, 1072.463f, 20.683409f), V3(-3169.6594f, 1075.0051f, 20.829184f))),				// Tattoo Parlor 6
			pair(pair(-821.9946f, -187.1776f), pair(V3(-827.5958f, -189.18474f, 37.61457f), V3(-820.1551f, -186.87096f, 37.5689f))),					// Barber Shop 1
			pair(pair(133.5702f, -1710.918f), pair(V3(130.37733f, -1714.8396f, 29.220602f), V3(135.4413f, -1709.0634f, 29.29162f))),					// Barber Shop 2
			pair(pair(-1287.0822f, -1116.5576f), pair(V3(-1292.8444f, -1116.5463f, 6.6531425f), V3(-1285.7589f, -1116.6903f, 6.9901085f))),			// Barber Shop 3
			pair(pair(1933.1191f, 3726.079f), pair(V3(1935.5698f, 3721.771f, 32.871326f), V3(1932.2449f, 3727.5989f, 32.84443f))),					// Barber Shop 4
			pair(pair(1208.3335f, -470.917f), pair(V3(1202.8934f, -470.4119f, 66.244255f), V3(1210.424f, -471.9904f, 66.20722f))),					// Barber Shop 5
			pair(pair(-30.7448f, -148.4921f), pair(V3(-29.34236f, -143.49902f, 57.024364f), V3(-32.043205f, -150.1631f, 57.075897f))),				// Barber Shop 6
			pair(pair(-280.8165f, 6231.7705f), pair(V3(-284.91342f, 6235.1514f, 31.483158f), V3(-278.9422f, 6229.9194f, 31.694422f))),				// Barber Shop 7
			pair(pair(419.531f, -807.5787f), pair(V3(414.8278f, -808.60046f, 29.341763f), V3(421.97498f, -807.82825f, 29.491158f))),					// Binco Clothing 1
			pair(pair(-818.6218f, -1077.533f), pair(V3(-816.42365f, -1081.9867f, 11.1324625f), V3(-818.6504f, -1077.2657f, 11.339294f))),				// Binco Clothing 2
			pair(pair(80.665f, -1391.6694f), pair(V3(86.65787f, -1391.2084f, 29.214544f), V3(79.77582f, -1391.3315f, 29.378551f))),					// Discount Store 1
			pair(pair(1687.8812f, 4820.55f), pair(V3(1681.2683f, 4821.0103f, 42.058624f), V3(1690.5027f, 4821.0264f, 42.06316f))),					// Discount Store 2
			pair(pair(-1094.0487f, 2704.1707f), pair(V3(-1092.6343f, 2702.3652f, 19.270864f), V3(-1097.3839f, 2708.6177f, 19.107866f))),				// Discount Store 3
			pair(pair(1197.9722f, 2704.2205f), pair(V3(1200.8969f, 2696.5352f, 37.91305f), V3(1198.7032f, 2705.8027f, 38.22236f))),					// Discount Store 4
			pair(pair(-0.2361f, 6516.0454f), pair(V3(-3.1778638f, 6519.246f, 31.333563f), V3(0.24288772f, 6515.4546f, 31.88943f))),					// Discount Store 5
			pair(pair(-715.3598f, -155.7742f), pair(V3(-719.4518f, -158.38512f, 36.999577f), V3(-712.2488f, -155.63303f, 37.4151f))),					// Ponsonbys 1
			pair(pair(-158.2199f, -304.9663f), pair(V3(-151.84828f, -306.8067f, 38.70402f), V3(-159.52165f, -303.4513f, 39.733074f))),				// Ponsonbys 2
			pair(pair(-1455.0045f, -233.1862f), pair(V3(-1458.1075f, -227.35103f, 49.14472f), V3(-1453.9937f, -235.01907f, 49.801964f))),				// Ponsonbys 3
			pair(pair(-1199.8092f, -776.6886f), pair(V3(-1206.4244f, -782.0857f, 17.092798f), V3(-1197.94f, -775.6148f, 17.324402f))),				// Suburban 1
			pair(pair(618.1857f, 2752.5667f), pair(V3(618.5425f, 2743.446f, 42.01316f), V3(617.98596f, 2754.42f, 42.088135f))),						// Suburban 2
			pair(pair(126.6853f, -212.5027f), pair(V3(129.19165f, -205.22305f, 54.50649f), V3(125.54211f, -216.36665f, 54.55783f))),					// Suburban 3
			pair(pair(-3168.9663f, 1055.2869f), pair(V3(-3165.6106f, 1061.4877f, 20.839468f), V3(-3170.5708f, 1051.7955f, 20.863214f))),				// Suburban 4
		};
		for (const auto& pair : redirect_pairs)
		{
			if (Util::VDIST(coords.x, coords.y, pair.first.first, pair.first.second) < 0.1f)
			{
				return in_vehicle ? pair.second.first : pair.second.second;
			}
		}
		if (coords.z_is_guessed)
		{
			constexpr pair<pair<float, float>, V3> redirects[] = {
				pair(pair(987.32f, 79.32f), V3(988.54395f, 80.86057f, 80.9906f)),							// The Music Locker Blip
				pair(pair(927.624329f, 44.851101f), V3(922.816223f, 47.206078f, 81.106331f)),				// Casino Blip
				pair(pair(922.816223f, 47.206078f), V3(922.816223f, 47.206078f, 81.106331f)),				// Casino Stand Coords
				pair(pair(113.240173f, 6624.191406f), V3(113.240173f, 6624.191406f, 31.264368f)),			// Beeker's Garage
				pair(pair(-1147.202759f, -1992.451782f), V3(-1147.202759f, -1992.451782f, 12.653099f)),	// Los Santos Customs 1
				pair(pair(724.479248f, -1089.010254f), V3(724.479248f, -1089.010254f, 21.648642f)),		// Los Santos Customs 2
				pair(pair(-354.587860f, -135.499176f), V3(-354.587860f, -135.499176f, 38.479984f)),		// Los Santos Customs 3
				pair(pair(1174.730347f, 2644.499023f), V3(1174.730347f, 2644.499023f, 37.240135f)),		// Los Santos Customs 4
				pair(pair(-376.713f, -1877.56f), V3(-370.091f, -1862.77f, 20.5285f)),						// Arena Workshop
			};
			for (const auto& pair : redirects)
			{
				if (Util::VDIST(coords.x, coords.y, pair.first.first, pair.first.second) < 0.1f)
				{
					return pair.second;
				}
			}
		}
		return coords;
	}

	void TpUtil::teleportWithRedirects(Vector2Plus coords, std::function<void()>&& callback)
	{
		return teleport(followRedirects(coords, g_player_veh.isValid()), std::move(callback));
	}

	void TpUtil::teleport(Vector2Plus coords, std::function<void()>&& callback)
	{
		//util::toast(coords.operator std::string(), TOAST_ALL, false);
		auto do_tp = [callback{ std::move(callback) }](v3&& coords)
		{
			last_tp = coords;
			teleport_exact(std::move(coords));
			if (callback)
			{
				callback();
			}
		};
		if (coords.z_is_guessed)
		{
			FiberPool::queueJob([do_tp{ std::move(do_tp) }, coords{ std::move(coords) }]() mutable
			{
				for (int i = 0; i < 60; i++)
				{
					g_renderer.drawCentredTextThisTick(LANG_GET_W("TPWAIT"));
					STREAMING::REQUEST_COLLISION_AT_COORD(coords.x, coords.y, coords.z);
					float ground_z = get_ground_z(coords.x, coords.y);
					if (ground_z != -200.0f)
					{
						coords.z = ground_z;
						break;
					}
					Script::current()->yield();
				}
				do_tp(std::move(coords));
			});
		}
		else
		{
			do_tp(std::move(coords));
		}
	}

	void TpUtil::teleport_exact(v3 coords)
	{
		onPreTp(g_player_ent.getPos());
		undo_stack.push(PositionState::saveSimple());
		coords.z += (g_player_ent.getDimensions().z * 0.5f);
		g_player_ent.setPos(coords);
		g_player_ent.setRot({ 0.0f, 0.0f, g_player_ent.getRot().z });
		onPostTp(coords);
	}

	void TpUtil::undo_teleport()
	{
		if (!undo_stack.empty())
		{
			onPreTp(g_player_ped.getPos());
			undo_stack.top().restore();
			undo_stack.pop();
		}
		onPostTp(g_player_ped.getPos());
	}

	void TpUtil::teleport_to_blip(const Blip blip)
	{
		const Entity ent = HUD::GET_BLIP_INFO_ID_ENTITY_INDEX(blip);
		if (ent != 0)
		{
			teleport(AbstractEntity::get(ent).getPos());
		}
		else
		{
			auto coords = v3(HUD::GET_BLIP_COORDS(blip));
			coords.z += 0.5f;
			teleport(coords);
		}
	}

	void TpUtil::teleport_to_veh(const Vehicle veh)
	{
		auto ent = AbstractEntity::get(veh);
		const int seat = ent.getFreeSeat();
		if (seat == -3)
		{
			teleport(ent.getPos());
		}
		else
		{
			onPreTp(g_player_ped.getPos());
			PED::SET_PED_INTO_VEHICLE(g_player_ped, veh, seat);
			VEHICLE::SET_VEHICLE_ENGINE_ON(veh, true, true, false);
			onPostTp(g_player_ped.getPos());
		}
	}

	void TpUtil::onPreTp(const v3&& pos)
	{
		CommandTeleportParticle::playTpParticle(pos);
	}

	void TpUtil::onPostTp(const v3& pos)
	{
		last_tp_time = get_current_time_millis();
		CommandTeleportParticle::playTpParticle(pos);
	}
}
