#include "gta_net_object_mgr.hpp"

#include <fmt/format.h>

#include "atStringHash.hpp"
#include "Exceptional.hpp"
#include "gta_entity.hpp"
#include "joaatToString.hpp"
#include "pointers.hpp"
#include "synctree_player.hpp"

#ifdef STAND_DEBUG
#include "StringUtils.hpp"
#endif

namespace rage
{
	using namespace Stand;

	netObject* netObjectMgrBase::find_object_by_id(uint16_t object_id, bool can_delete_be_pending)
	{
		return pointers::find_object_by_id(this, object_id, can_delete_be_pending);
	}

	void netObjectMgrBase::unregisterById(uint16_t object_id)
	{
		__try
		{
			if (rage::netObject* netObject = find_object_by_id(object_id, true))
			{
				unregisterNetworkObject_(netObject, true);
			}
		}
		__EXCEPTIONAL()
		{
		}
	}

	[[nodiscard]] bool netSyncTree::extendsVehicle(NetworkObjectType object_type) noexcept
	{
		switch (object_type)
		{
		case NET_OBJ_TYPE_DOOR:
		case NET_OBJ_TYPE_OBJECT:
		case NET_OBJ_TYPE_PED:
		case NET_OBJ_TYPE_PICKUP:
		case NET_OBJ_TYPE_PICKUP_PLACEMENT:
		case NET_OBJ_TYPE_PLAYER:
			return false;
		}
		return true;
	}

	bool netSyncTree::extendsPedBase(NetworkObjectType object_type) noexcept
	{
		switch (object_type)
		{
		case NET_OBJ_TYPE_PED:
		case NET_OBJ_TYPE_PLAYER:
			return true;
		}
		return false;
	}

	bool netSyncTree::extendsPhysicalBase(NetworkObjectType object_type) noexcept
	{
		switch (object_type)
		{
		case NET_OBJ_TYPE_DOOR:
		case NET_OBJ_TYPE_OBJECT:
		case NET_OBJ_TYPE_PICKUP:
			return true;
		}
		return extendsVehicle(object_type);
	}

	bool netSyncTree::extendsDynamicEntityBase(NetworkObjectType object_type) noexcept
	{
		return extendsPedBase(object_type) || extendsPhysicalBase(object_type);
	}

	hash_t& netSyncTree::_model(NetworkObjectType object_type) noexcept
	{
		switch (object_type)
		{
		case NET_OBJ_TYPE_DOOR:
			return static_cast<CDoorSyncTree*>(this)->creation_data.model;

		case NET_OBJ_TYPE_OBJECT:
			return static_cast<CObjectSyncTree*>(this)->creation_data.model;

		case NET_OBJ_TYPE_PED:
			return static_cast<CPedSyncTree*>(this)->creation_data.model;

		case NET_OBJ_TYPE_PICKUP:
			return static_cast<CPickupSyncTree*>(this)->creation_data.model;

		case NET_OBJ_TYPE_PICKUP_PLACEMENT:
			return static_cast<CPickupPlacementSyncTree*>(this)->creation_data.model;

		case NET_OBJ_TYPE_PLAYER:
			return static_cast<CPlayerSyncTree*>(this)->creation_data.model;

		case NET_OBJ_TYPE_AUTOMOBILE:
		case NET_OBJ_TYPE_BIKE:
		case NET_OBJ_TYPE_BOAT:
		case NET_OBJ_TYPE_HELI:
		case NET_OBJ_TYPE_PLANE:
		case NET_OBJ_TYPE_SUBMARINE:
		case NET_OBJ_TYPE_TRAILER:
		case NET_OBJ_TYPE_TRAIN:
			break;
		}
		return static_cast<CVehicleSyncTree*>(this)->creation_data.model;
	}

	hash_t netSyncTree::getModel(NetworkObjectType object_type) noexcept
	{
		return _model(object_type);
	}

	void netSyncTree::setModel(NetworkObjectType object_type, hash_t model) noexcept
	{
		_model(object_type) = model;
	}

	bool netSyncTree::sectorApplies() noexcept
	{
		return static_cast<CProximityMigrateableSyncTreeBase*>(this)->sector.applies;
	}

	bool netSyncTree::sectorPosApplies(NetworkObjectType object_type) const noexcept
	{
		if (object_type == NET_OBJ_TYPE_OBJECT)
		{
			return static_cast<const CObjectSyncTree*>(this)->sector_position.applies;
		}
		if (object_type == NET_OBJ_TYPE_PICKUP)
		{
			return static_cast<const CPickupSyncTree*>(this)->sector_position.applies;
		}
		if (object_type == NET_OBJ_TYPE_PLAYER)
		{
			return static_cast<const CPlayerSyncTree*>(this)->sector_position.applies;
		}
		return static_cast<const CProximityMigrateableSyncTreeBase*>(this)->sector_position.applies;
	}

	bool netSyncTree::posApplies(NetworkObjectType object_type) noexcept
	{
		return sectorApplies() || sectorPosApplies(object_type);
	}

	Sector& netSyncTree::_sector() noexcept
	{
		return static_cast<CProximityMigrateableSyncTreeBase*>(this)->sector.sector;
	}

	Vector3& netSyncTree::_sectorPos(NetworkObjectType object_type) noexcept
	{
		if (object_type == NET_OBJ_TYPE_OBJECT)
		{
			return static_cast<CObjectSyncTree*>(this)->sector_position.pos;
		}
		if (object_type == NET_OBJ_TYPE_PICKUP)
		{
			return static_cast<CPickupSyncTree*>(this)->sector_position.pos;
		}
		if (object_type == NET_OBJ_TYPE_PLAYER)
		{
			return static_cast<CPlayerSyncTree*>(this)->sector_position.pos;
		}
		return static_cast<CProximityMigrateableSyncTreeBase*>(this)->sector_position.pos;
	}

	void netSyncTree::world2sector(const Stand::v3& pos, Sector& outSector, Stand::v3& outSectorPos) noexcept
	{
		const float adjusted_z = (pos.z + 1700.0f);

		const auto chunk_x = int16_t(pos.x / 54.0f);
		const auto chunk_y = int16_t(pos.y / 54.0f);
		const auto chunk_z = int16_t(adjusted_z / 69.0f);

		outSector.x = (chunk_x + 512);
		outSector.y = (chunk_y + 512);
		outSector.z = chunk_z;

		outSectorPos.x = pos.x - (chunk_x * 54.0f);
		outSectorPos.y = pos.y - (chunk_y * 54.0f);
		outSectorPos.z = adjusted_z - (chunk_z * 69.0f);
	}

	Vector3 netSyncTree::sector2world(const Sector& sector, const Stand::v3& sector_pos) noexcept
	{
		return {
			((sector.x - 512.0f) * 54.0f) + sector_pos.x,
			((sector.y - 512.0f) * 54.0f) + sector_pos.y,
			((sector.z * 69.0f) + sector_pos.z) - 1700.0f
		};
	}

	Vector3 netSyncTree::getPos(NetworkObjectType object_type) noexcept
	{
		Sector sector;
		v3 sector_pos;
		CEntity* entity;

		__try
		{
			if (object && (entity = object->GetEntity()))
			{
				world2sector(Stand::v3{ entity->matrix.x, entity->matrix.y, entity->matrix.z }, sector, sector_pos);

				if (sectorApplies())
				{
					sector = _sector();
				}
				if (sectorPosApplies(object_type))
				{
					sector_pos = _sectorPos(object_type);
				}
			}
			else
			{
				sector = _sector();
				sector_pos = _sectorPos(object_type);
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) // in case the object's vftable is fucked
		{
			sector = _sector();
			sector_pos = _sectorPos(object_type);
		}

		return sector2world(sector, sector_pos);
	}

	void netSyncTree::setPos(NetworkObjectType object_type, const Vector3& pos) noexcept
	{
		world2sector(pos, _sector(), _sectorPos(object_type));
	}

	bool netSyncTree::attachApplies(NetworkObjectType object_type) const noexcept
	{
		if (extendsPedBase(object_type))
		{
			return static_cast<const CPedSyncTreeBase*>(this)->attach_data.applies;
		}
		else if (extendsPhysicalBase(object_type))
		{
			return static_cast<const CPhysicalSyncTreeBase*>(this)->attach_data.applies;
		}
		return false;
	}

	ObjectId netSyncTree::getAttachedTo(NetworkObjectType object_type) const noexcept
	{
		if (extendsPedBase(object_type))
		{
			return static_cast<const CPedSyncTreeBase*>(this)->attach_data.attached_to;
		}
		else if (extendsPhysicalBase(object_type))
		{
			if (static_cast<const CPhysicalSyncTreeBase*>(this)->attach_data.is_attached)
			{
				return static_cast<const CPhysicalSyncTreeBase*>(this)->attach_data.attached_to;
			}
		}
		return 0;
	}

	void netSyncTree::setAttachedTo(NetworkObjectType object_type, ObjectId attached_to) noexcept
	{
		if (extendsPedBase(object_type))
		{
			static_cast<CPedSyncTreeBase*>(this)->attach_data.attached_to = attached_to;
		}
		else if (extendsPhysicalBase(object_type))
		{
			if (attached_to == 0)
			{
				static_cast<CPhysicalSyncTreeBase*>(this)->attach_data.is_attached = false;
			}
			else
			{
				static_cast<CPhysicalSyncTreeBase*>(this)->attach_data.is_attached = true;
				static_cast<CPhysicalSyncTreeBase*>(this)->attach_data.attached_to = attached_to;
			}
		}
	}

	bool netSyncTree::isVisible(NetworkObjectType object_type) const noexcept
	{
		if (extendsPedBase(object_type))
		{
			return static_cast<const CPedSyncTreeBase*>(this)->physical_game_state.m_isVisible;
		}
		else if (extendsPhysicalBase(object_type))
		{
			return static_cast<const CPhysicalSyncTreeBase*>(this)->physical_game_state.m_isVisible;
		}
		return true;
	}

#define DUMP_ALL_APPLICABLE_NODES true

	std::string netSyncTree::dump(NetworkObjectType object_type, bool is_create)
	{
		std::string str(NetworkObjectType_toString(object_type));
		str.push_back('{');
		if (object)
		{
			str.append("id=").append(fmt::to_string(object->object_id));
		}
		if (is_create)
		{
			if (object)
			{
				str.append(", ");
			}
			str.append("model=").append(joaatToString(getModel(object_type)));
		}
		switch (object_type)
		{
		case NET_OBJ_TYPE_PICKUP:
			const auto this_pickup = static_cast<const CPickupSyncTree*>(this);
			str.append(", pickup=").append(joaatToString(this_pickup->creation_data.pickup));
			for (uint32_t i = 0; i != this_pickup->creation_data.m_numWeaponComponents; ++i)
			{
				str.append(", weapon_component=").append(joaatToString(this_pickup->creation_data.m_weaponComponents[i]));
			}
			break;
		}
		if (posApplies(object_type))
		{
			__try
			{
				auto pos = getPos(object_type);
				str.append(", pos={").append(getPos(object_type)).append("}");
			}
			__EXCEPTIONAL()
			{
			}
		}
		if (attachApplies(object_type))
		{
			str.append(", attached_to=").append(fmt::to_string(getAttachedTo(object_type)));
		}
		if (object_type == NET_OBJ_TYPE_PLAYER)
		{
			const auto this_player = static_cast<const CPlayerSyncTree*>(this);

			if (this_player->appearance_data.applies)
			{
				str.append(", appearance_model=").append(joaatToString(this_player->appearance_data.m_NewModelHash));
			}

			if (this_player->player_camera.applies)
			{
				str.append(", camera={");
				if (this_player->player_camera.position_is_absolute)
				{
					str.append("absolute, ").append(this_player->player_camera.absolute_pos);
				}
				else
				{
					str.append("relative, ").append(this_player->player_camera.relative_pos);
				}
				str.push_back('}');
			}
		}
		if (extendsPedBase(object_type))
		{
			const auto this_ped_base = static_cast<const CPedSyncTreeBase*>(this);

			if (this_ped_base->ped_game_state.applies)
			{
				str.append(", weapon=").append(joaatToString(this_ped_base->ped_game_state.m_weapon));
			}

			if (this_ped_base->task_data.applies)
			{
				str.append(", primary_task=").append(fmt::to_string(this_ped_base->task_data.primary_task));
			}
		}
		if (extendsVehicle(object_type))
		{
			const auto this_vehicle = static_cast<const CVehicleSyncTree*>(this);

			if (this_vehicle->appearance_data.applies)
			{
				str.append(", wheel_type=").append(fmt::to_string(this_vehicle->appearance_data.m_wheelType));
				str.append(", wheel_mod=").append(fmt::to_string(this_vehicle->appearance_data.m_wheelMod));
				if (this_vehicle->appearance_data.m_hasDifferentRearWheel)
				{
					str.append(", rear_wheel_mod=").append(fmt::to_string(this_vehicle->appearance_data.m_rearWheelMod));
				}
			}
			if (this_vehicle->m_vehicleGameStateNode.applies)
			{
				str.append(", radio=").append(fmt::to_string(this_vehicle->m_vehicleGameStateNode.m_radioStation));
			}
			if (this_vehicle->vehicle_task_data.applies)
			{
				str.append(", task=").append(fmt::to_string(this_vehicle->vehicle_task_data.m_taskType));
			}

			if (object_type == NET_OBJ_TYPE_TRAIN)
			{
				const auto this_train = static_cast<const CTrainSyncTree*>(this);

				if (this_train->train_game_state.applies)
				{
					str.append(", track=").append(fmt::to_string(this_train->train_game_state.m_TrackID));
					str.append(", config=").append(fmt::to_string(this_train->train_game_state.m_TrainConfigIndex));
					str.append(", carriage_config=").append(fmt::to_string(this_train->train_game_state.m_CarriageConfigIndex));
				}
			}
		}
#ifdef STAND_DEBUG
		{
			std::string other_applicable_nodes{};
			for (const auto& n : create_nodes)
			{
				if (n == nullptr)
				{
					break;
				}
				if (!n->applies)
				{
					continue;
				}
				if (n == create_nodes[0]) // creation data
				{
					continue;
				}
				std::string name = castAndGetNodeName(object_type, n);
				if (name != "???")
				{
					name.erase(name.begin()); // C...
					name.erase(name.length() - 4); // ...Node
					if (name.substr(name.length() - 4) == "Data")
					{
						name.erase(name.length() - 4);
					}
				}
#if !DUMP_ALL_APPLICABLE_NODES
				switch (rage::atStringHash(name))
				{
				case ATSTRINGHASH("Sector"):
				case ATSTRINGHASH("SectorPosition"):
				case ATSTRINGHASH("ObjectSectorPos"):
				case ATSTRINGHASH("PickupSectorPos"):
				case ATSTRINGHASH("PlayerSectorPos"):
				case ATSTRINGHASH("PlayerCamera"):
				case ATSTRINGHASH("PedAttach"):
				case ATSTRINGHASH("PhysicalAttach"):
					break;

				default:
#endif
					StringUtils::list_append(other_applicable_nodes, name);
#if !DUMP_ALL_APPLICABLE_NODES
					break;
				}
#endif
			}
			if (!other_applicable_nodes.empty())
			{
#if DUMP_ALL_APPLICABLE_NODES
				str.append(", applicable_nodes={");
#else
				str.append(", other_applicable_nodes={");
#endif		
				str.append(std::move(other_applicable_nodes)).push_back('}');
			}
		}
#endif
		str.push_back('}');
		return str;
	}

#ifdef STAND_DEV
	const char* netSyncTree::castAndGetNodeName(NetworkObjectType object_type, const rage::netSyncDataNode* node) const
	{
		switch (object_type)
		{
		case NET_OBJ_TYPE_AUTOMOBILE: return static_cast<const CAutomobileSyncTree*>(this)->getNodeName(node);
		case NET_OBJ_TYPE_BIKE: return static_cast<const CBikeSyncTree*>(this)->getNodeName(node);
		case NET_OBJ_TYPE_BOAT: return static_cast<const CBoatSyncTree*>(this)->getNodeName(node);
		case NET_OBJ_TYPE_DOOR: return static_cast<const CDoorSyncTree*>(this)->getNodeName(node);
		case NET_OBJ_TYPE_HELI: return static_cast<const CHeliSyncTree*>(this)->getNodeName(node);
		case NET_OBJ_TYPE_OBJECT: return static_cast<const CObjectSyncTree*>(this)->getNodeName(node);
		case NET_OBJ_TYPE_PED: return static_cast<const CPedSyncTree*>(this)->getNodeName(node);
		case NET_OBJ_TYPE_PICKUP: return static_cast<const CPickupSyncTree*>(this)->getNodeName(node);
		case NET_OBJ_TYPE_PICKUP_PLACEMENT: return static_cast<const CPickupPlacementSyncTree*>(this)->getNodeName(node);
		case NET_OBJ_TYPE_PLANE: return static_cast<const CPlaneSyncTree*>(this)->getNodeName(node);
		case NET_OBJ_TYPE_SUBMARINE: return static_cast<const CSubmarineSyncTree*>(this)->getNodeName(node);
		case NET_OBJ_TYPE_PLAYER: return static_cast<const CPlayerSyncTree*>(this)->getNodeName(node);
		case NET_OBJ_TYPE_TRAILER: return static_cast<const CAutomobileSyncTree*>(this)->getNodeName(node);
		case NET_OBJ_TYPE_TRAIN: return static_cast<const CTrainSyncTree*>(this)->getNodeName(node);
		}
		return "???";
	}
#endif
}

bool CSyncedPedVarData::isComponentSet(const int componentId) const noexcept
{
	return m_components_set_bitflag & (1 << componentId);
}

bool CSyncedPedVarData::isInvalid() const noexcept
{
	if (isComponentSet(0) && m_textureId[0] >= 8)
	{
		return true;
	}

	// [[nextgen]]
	if (// male
		m_componentHashes[4] == 159467579
		|| m_componentHashes[6] == 159467579
		|| m_componentHashes[8] == 159467579
		|| m_componentHashes[11] == 159467579
		// female
		|| m_componentHashes[4] == 1224880856
		|| m_componentHashes[6] == 1224880856
		|| m_componentHashes[8] == 1224880856
		|| m_componentHashes[11] == 1224880856
		)
	{
		return true;
	}

	return false;
}

#ifdef STAND_DEV
#define NODE_SWITCH(node) switch (reinterpret_cast<uintptr_t>(node) - reinterpret_cast<uintptr_t>(this))
#define NODE_CASE(tree, node) case offsetof(tree, node): return typeid(decltype(tree::node)).name() + 6 /* "class " */

#define ADDR_IF_NO_NAME false

#if ADDR_IF_NO_NAME
static std::string buf;
#endif

const char* CProximityMigrateableSyncTreeBase::getNodeName(const rage::netSyncDataNode* node) const
{
	NODE_SWITCH(node)
	{
		NODE_CASE(CProximityMigrateableSyncTreeBase, m_migrationNode);
		NODE_CASE(CProximityMigrateableSyncTreeBase, m_globalFlagsNode);
		NODE_CASE(CProximityMigrateableSyncTreeBase, sector);
		NODE_CASE(CProximityMigrateableSyncTreeBase, sector_position);
	}
#if ADDR_IF_NO_NAME
	buf = fmt::to_string((void*)node);
	return buf.c_str();
#else
	return "???";
#endif
}

const char* CEntitySyncTreeBase::getNodeName(const rage::netSyncDataNode* node) const
{
	NODE_SWITCH(node)
	{
		NODE_CASE(CEntitySyncTreeBase, entity_script_info);
		NODE_CASE(CEntitySyncTreeBase, entity_script_game_state);
	}
	return CProximityMigrateableSyncTreeBase::getNodeName(node);
}

const char* CDynamicEntitySyncTreeBase::getNodeName(const rage::netSyncDataNode* node) const
{
	NODE_SWITCH(node)
	{
		NODE_CASE(CDynamicEntitySyncTreeBase, dynamic_entity_game_state);
	}
	return CEntitySyncTreeBase::getNodeName(node);
}

const char* CPedSyncTreeBase::getNodeName(const rage::netSyncDataNode* node) const
{
	NODE_SWITCH(node)
	{
		NODE_CASE(CPedSyncTreeBase, physical_game_state);
		NODE_CASE(CPedSyncTreeBase, attach_data);
		NODE_CASE(CPedSyncTreeBase, ped_game_state);
		NODE_CASE(CPedSyncTreeBase, health_data);
		NODE_CASE(CPedSyncTreeBase, task_data);
		NODE_CASE(CPedSyncTreeBase, task_specific[0]);
		NODE_CASE(CPedSyncTreeBase, task_specific[1]);
		NODE_CASE(CPedSyncTreeBase, task_specific[2]);
		NODE_CASE(CPedSyncTreeBase, task_specific[3]);
		NODE_CASE(CPedSyncTreeBase, task_specific[4]);
		NODE_CASE(CPedSyncTreeBase, task_specific[5]);
		NODE_CASE(CPedSyncTreeBase, task_specific[6]);
		NODE_CASE(CPedSyncTreeBase, task_specific[7]);
	}
	return CDynamicEntitySyncTreeBase::getNodeName(node);
}

const char* CPhysicalSyncTreeBase::getNodeName(const rage::netSyncDataNode* node) const
{
	NODE_SWITCH(node)
	{
		NODE_CASE(CPhysicalSyncTreeBase, m_orientationNode);
		NODE_CASE(CPhysicalSyncTreeBase, m_velocityNode);
		NODE_CASE(CPhysicalSyncTreeBase, attach_data);
		NODE_CASE(CPhysicalSyncTreeBase, physical_game_state);
		NODE_CASE(CPhysicalSyncTreeBase, physical_script_game_state);
	}
	return CDynamicEntitySyncTreeBase::getNodeName(node);
}

const char* CVehicleSyncTree::getNodeName(const rage::netSyncDataNode* node) const
{
	NODE_SWITCH(node)
	{
		NODE_CASE(CVehicleSyncTree, creation_data);
		NODE_CASE(CVehicleSyncTree, m_vehicleProximityMigrationNode);
		NODE_CASE(CVehicleSyncTree, m_vehicleAngVelocityNode);
		NODE_CASE(CVehicleSyncTree, steering_data);
		NODE_CASE(CVehicleSyncTree, m_vehicleControlNode);
		NODE_CASE(CVehicleSyncTree, appearance_data);
		NODE_CASE(CVehicleSyncTree, m_damageStatusNode);
		NODE_CASE(CVehicleSyncTree, m_vehicleGameStateNode);
		NODE_CASE(CVehicleSyncTree, m_vehicleHealthNode);
		NODE_CASE(CVehicleSyncTree, m_vehicleScriptGameStateNode);
		NODE_CASE(CVehicleSyncTree, m_componentReservationNode);
		NODE_CASE(CVehicleSyncTree, vehicle_task_data);
		NODE_CASE(CVehicleSyncTree, gadget_data);
	}
	return CPhysicalSyncTreeBase::getNodeName(node);
}

const char* CPlayerSyncTree::getNodeName(const rage::netSyncDataNode* node) const
{
	NODE_SWITCH(node)
	{
		NODE_CASE(CPlayerSyncTree, creation_data);
		NODE_CASE(CPlayerSyncTree, sector_position);
		NODE_CASE(CPlayerSyncTree, player_camera);
		NODE_CASE(CPlayerSyncTree, player_game_state);
		NODE_CASE(CPlayerSyncTree, appearance_data);
		NODE_CASE(CPlayerSyncTree, extended_game_state);
	}
	return CPedSyncTreeBase::getNodeName(node);
}

const char* CAutomobileSyncTreeBase::getNodeName(const rage::netSyncDataNode* node) const
{
	NODE_SWITCH(node)
	{
		NODE_CASE(CAutomobileSyncTreeBase, automobile_creation_data);
	}
	return CVehicleSyncTree::getNodeName(node);
}
#endif
