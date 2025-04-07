#include "AbstractEntity.hpp"

#include <fmt/core.h>

#include <soup/ObfusString.hpp>
#include <soup/rand.hpp>

#include "AbstractModel.hpp"
#include "AbstractPlayer.hpp"
#include "atStringHash.hpp"
#include "Box.hpp"
#include "CObject.hpp"
#include "CommandLockOutfit.hpp"
#include "CPedHeadBlendData.hpp"
#include "CPickup.hpp"
#include "CPlayerInfo.hpp"
#include "distance_sort.hpp"
#include "eDamageFlags.hpp"
#include "eEntityType.hpp"
#include "eExplosionTag.hpp"
#include "eMigrationType.hpp"
#include "ePedConfigFlags.hpp"
#include "eTaskType.hpp"
#include "Exceptional.hpp"
#include "ExecCtx.hpp"
#include "explosion.hpp"
#include "FiberPool.hpp"
#include "FileLogger.hpp"
#include "FlowEvent.hpp"
#include "fwPool.hpp"
#include "fwScriptGuid.hpp"
#include "get_current_time_millis.hpp"
#include "get_vehicle_display_name.hpp"
#include "gta_input.hpp"
#include "gta_net_object_mgr.hpp"
#include "gta_ped.hpp"
#include "gta_replayinterface.hpp"
#include "gta_vehicle.hpp"
#include "Gui.hpp"
#include "is_session.hpp"
#include "joaatToString.hpp"
#include "Metrics.hpp"
#include "Names.hpp"
#include "natives.hpp"
#include "PedBoneId.hpp"
#include "PedModel.hpp"
#include "PedType.hpp"
#include "pointers.hpp"
#include "Renderer.hpp"
#include "Script.hpp"
#include "ScriptGlobal.hpp"
#include "script_thread.hpp"
#include "SpectateMgr.hpp"
#include "TargetingData.hpp"
#include "tbFreecam.hpp"
#include "TickQueues.hpp"
#include "TranscendentVehicle.hpp"
#include "Util.hpp"
#include "vehicle_items.hpp"
#include "VehicleMods.hpp"
#include "weapons.hpp"

namespace Stand
{
	void AbstractEntity::updatePlayerPed()
	{
		g_player_ped = AbstractEntity::get((*pointers::ped_factory)->m_local_ped);

		{
			bool on_foot = !g_player_ped.isInAnyVehicle();
			if (on_foot != player_ped_was_on_foot)
			{
				g_gui.processToggleCorrelation(TC_SCRIPT_NOYIELD, ToggleCorrelation::ON_FOOT, on_foot);
				player_ped_was_on_foot = on_foot;
			}
		}
		{
			bool aiming = g_player_ped.isAiming();
			if (aiming != player_ped_was_aiming)
			{
				g_gui.processToggleCorrelation(TC_SCRIPT_NOYIELD, ToggleCorrelation::AIMING, aiming);
				player_ped_was_aiming = aiming;
			}
		}
		if (!g_overwriting_cam_ent)
		{
			g_cam_ped = g_player_ped;
		}
		CommandLockOutfit::onTick();
	}

	void AbstractEntity::updatePlayerVehAndEnt()
	{
		g_player_veh = g_player_ped.getVehicle(false);
		updatePlayerEnt();
	}

	void AbstractEntity::updatePlayerEnt()
	{
		g_player_ent = g_player_veh.isValid() ? g_player_veh : g_player_ped;
		if (!g_overwriting_cam_ent)
		{
			g_cam_veh = g_player_veh;
			g_cam_ent = g_player_ent;
		}
	}

	void AbstractEntity::updatePlayerPedVehAndEnt()
	{
		updatePlayerPed();
		updatePlayerVehAndEnt();
	}

	void AbstractEntity::updateCamEnt()
	{
		g_cam_ent = g_cam_veh.isValid() ? g_cam_veh : g_cam_ped;
	}

	void AbstractEntity::onTick()
	{
		updatePlayerPedVehAndEnt();
		for (const auto& entry : control_requests)
		{
			if (entry.second.first > get_current_time_millis())
			{
				auto ent = AbstractEntity::get(entry.first);
				if (ent.isValid())
				{
					if (!ent.isOwner())
					{
						NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(entry.first);
						continue;
					}
					//g_logger.log(fmt::format("Got control for entity {}.", entry.first));
					__try
					{
						for (const auto& callback_entry : entry.second.second)
						{
							auto ent = AbstractEntity::get(entry.first);
							callback_entry.second.first(ent);
						}
					}
					__EXCEPTIONAL()
					{
					}
					goto erase_control_request;
				}
				else
				{
					g_logger.log(fmt::format(fmt::runtime(soup::ObfusString("Dropping control request for entity {} due to end of existence.").str()), entry.first));
				}
			}
			else
			{
				__try
				{
					const auto p = AbstractEntity::get(entry.first).getOwner();
					g_logger.log(fmt::format(fmt::runtime(soup::ObfusString("Dropping control request for entity {} from {} due to timeout.").str()), entry.first, p.getName()));
					if (p.isInTimeout())
					{
						g_logger.log(soup::ObfusString("Note: Player has been timed out. This wound is a self-inflicted one."));
					}
					else
					{
						p.triggerDetection(FlowEvent::MOD_NONET, {}, 75);
					}
				}
				__EXCEPTIONAL()
				{
				}
			}
			__try
			{
				for (const auto& callback_entry : entry.second.second)
				{
					auto ent = AbstractEntity::get(entry.first);
					callback_entry.second.second(ent);
				}
			}
			__EXCEPTIONAL()
			{
			}
		erase_control_request:
			control_requests.erase(entry.first);
		}
	}

	AbstractEntity AbstractEntity::getSmart(int64_t handle_or_ptr)
	{
		if (handle_or_ptr == -1)
		{
			return invalid();
		}
		if (handle_or_ptr > 0xFFFFFF)
		{
			return get(reinterpret_cast<CPhysical*>(handle_or_ptr));
		}
		return get((Entity)handle_or_ptr);
	}

	AbstractEntity AbstractEntity::getNetwork(uint16_t network_id)
	{
		if (auto n = (*pointers::network_object_mgr)->find_object_by_id(network_id, true))
		{
			if (auto ent = n->GetEntity())
			{
				return AbstractEntity((CPhysical*)ent);
			}
		}

		return AbstractEntity::invalid();
	}

	v3 AbstractEntity::getEffectiveUserPos()
	{
		if (g_tb_freecam.isEnabled())
		{
			return g_tb_freecam.pos;
		}
		return g_player_ent.getPos();
	}

	AbstractEntity AbstractEntity::getPersonalVehicle()
	{
		const auto blip = ScriptGlobal(GLOBAL_PV_BLIP).get<Blip>();
		if (blip != 0)
		{
			auto ent = HUD::GET_BLIP_INFO_ID_ENTITY_INDEX(blip);
			if (ent != 0)
			{
				return get(ent);
			}
		}
		if (g_player_veh.isUserPersonalVehicle())
		{
			return g_player_veh;
		}
		return invalid();
	}

	AbstractEntity AbstractEntity::getNearestVisible(std::vector<AbstractEntity>& entites, bool los_check)
	{
		HashedDistanceSort<AbstractEntity>::sort(entites, [](AbstractEntity& ent)
		{
			const v3 pos = ent.getPos();
			if (pos.isNull())
			{
				return FLT_MAX;
			}
			const rage::Vector2 screen_pos = pos.getScreenPos();
			return abs(0.5f - screen_pos.x) + abs(0.5f - screen_pos.y);
		});
		const Vector3 cam_pos = CAM::GET_FINAL_RENDERED_CAM_COORD();
		for (AbstractEntity& ent : entites)
		{
			const v3 pos = ent.getPos();
			const rage::Vector2 screen_pos = pos.getScreenPos();
			if (screen_pos.x >= 0.3f && screen_pos.x <= 0.7f && screen_pos.y >= 0.3f && screen_pos.y <= 0.7f)
			{
				if (!los_check
					|| ent.isInLineOfSight(cam_pos, pos)
					)
				{
					return ent;
				}
			}
		}
		return invalid();
	}

	void AbstractEntity::getAll(const std::function<bool(AbstractEntity&&)>& collector)
	{
		getAllPeds(collector) && getAllVehicles(collector) && getAllObjects(collector);
	}

	bool AbstractEntity::getAllPeds(const std::function<bool(AbstractEntity&&)>& collector)
	{
		if (CReplayInterfaceGame* const replay = *pointers::replay_interface)
		{
			if (auto* const ped_interface = replay->ped_interface)
			{
				for (const auto& ent : ped_interface->list->entities)
				{
					if (ent.ptr != nullptr)
					{
						if (!collector(AbstractEntity(ent.ptr)))
						{
							return false;
						}
					}
				}
			}
		}
		return true;
	}

	bool AbstractEntity::getAllVehicles(const std::function<bool(AbstractEntity&&)>& collector)
	{
		if (CReplayInterfaceGame* const replay = *pointers::replay_interface)
		{
			if (auto* const veh_interface = replay->veh_interface)
			{
				for (const auto& ent : veh_interface->list->entities)
				{
					if (ent.ptr != nullptr)
					{
						if (!collector(AbstractEntity(ent.ptr)))
						{
							return false;
						}
					}
				}
			}
		}
		return true;
	}

	bool AbstractEntity::getAllObjects(const std::function<bool(AbstractEntity&&)>& collector)
	{
		if (CReplayInterfaceGame* const replay = *pointers::replay_interface)
		{
			if (auto* const object_interface = replay->object_interface)
			{
				for (const auto& ent : object_interface->list->entities)
				{
					if (ent.ptr != nullptr)
					{
						if (!collector(AbstractEntity(ent.ptr)))
						{
							return false;
						}
					}
				}
			}
		}
		return true;
	}

	void AbstractEntity::getAllPickups(const std::function<bool(AbstractEntity&&)>& collector)
	{
		if (CReplayInterfaceGame* const replay = *pointers::replay_interface)
		{
			if (auto* const pickup_interface = replay->pickup_interface)
			{
				for (const auto& ent : pickup_interface->list->entities)
				{
					if (ent.ptr != nullptr)
					{
						if (!collector(AbstractEntity(ent.ptr)))
						{
							return;
						}
					}
				}
			}
		}
	}

	void AbstractEntity::getAllObjectsIncludingPickups(const std::function<bool(AbstractEntity&&)>& collector)
	{
		if (getAllObjects(collector))
		{
			getAllPickups(collector);
		}
	}

	void AbstractEntity::getAllPeds(std::vector<AbstractEntity>& vec)
	{
		getAllPeds([&](AbstractEntity&& ent)
		{
			vec.push_back(std::move(ent));
			return true;
		});
	}

	void AbstractEntity::getAllVehicles(std::vector<AbstractEntity>& vec)
	{
		getAllVehicles([&](AbstractEntity&& ent)
		{
			vec.push_back(std::move(ent));
			return true;
		});
	}

	void AbstractEntity::getAllObjects(std::vector<AbstractEntity>& vec)
	{
		getAllObjects([&](AbstractEntity&& ent)
		{
			vec.push_back(std::move(ent));
			return true;
		});
	}

	void AbstractEntity::getAllPickups(std::vector<AbstractEntity>& vec)
	{
		getAllPickups([&](AbstractEntity&& ent)
		{
			vec.push_back(std::move(ent));
			return true;
		});
	}

	bool AbstractEntity::hasHandle() const
	{
		return handle != INVALID_GUID
			|| (ptr != nullptr
				&& ptr->extensions.Get(*pointers::extension_id_script_guid) != nullptr
				)
			;
	}

	bool AbstractEntity::ensureHandle()
	{
		if (handle != INVALID_GUID)
		{
			return true;
		}
		return findHandle();
	}

	bool AbstractEntity::findHandle()
	{
		if (ptr != nullptr)
		{
			if (auto guid_ext = ptr->extensions.Get(*pointers::extension_id_script_guid))
			{
				handle = (*pointers::script_guid_pool)->GetIndex(guid_ext);
				return true;
			}
			if (ExecCtx::get().hasGameTls())
			{
				handle = pointers::CTheScripts_GetGUIDFromEntity(*ptr);
				++Metrics::entity_handles_this_tick;
				return true;
			}
		}
		return false;
	}

	bool AbstractEntity::isHandleEquals(Entity handle)
	{
		return /*hasHandle() &&*/ getHandle() == handle;
	}

	void AbstractEntity::releaseHandle()
	{
		if (handle != INVALID_GUID)
		{
			SHAPETEST::RELEASE_SCRIPT_GUID_FROM_ENTITY(handle);
			handle = INVALID_GUID;
		}
	}

	bool AbstractEntity::setHandle(Entity desired_handle)
	{
		// Do we have an existing handle but in the wrong slot?
		if (hasHandle()
			&& (getHandle() >> 8) != (desired_handle >> 8)
			)
		{
			if (!(*pointers::script_guid_pool)->GetIsFree(desired_handle >> 8))
			{
				// Desired slot is taken, bail.
				return false;
			}

			// Dellocate existing handle
			releaseHandle();
		}

		// Need to allocate a handle?
		if (handle == INVALID_GUID)
		{
			if (!(*pointers::script_guid_pool)->GetIsFree(desired_handle >> 8))
			{
				// Desired slot is taken, don't bother.
				return false;
			}

			// Allocate handle in correct slot
			const auto first_free = (*pointers::script_guid_pool)->m_nFirstFreeIndex;
			const auto last_free = (*pointers::script_guid_pool)->m_nLastFreeIndex;
			(*pointers::script_guid_pool)->m_nFirstFreeIndex = (desired_handle >> 8);
			ensureHandle();
			(*pointers::script_guid_pool)->m_nFirstFreeIndex = first_free;
			(*pointers::script_guid_pool)->m_nLastFreeIndex = last_free;
		}

		// Set desired reference value
		if ((handle & POOL_FLAG_REFERENCEMASK) != (desired_handle & POOL_FLAG_REFERENCEMASK))
		{
			setHandleReference(desired_handle & POOL_FLAG_REFERENCEMASK);
		}

		return true;
	}

	void AbstractEntity::setHandleReference(uint8_t reference)
	{
		SOUP_ASSERT((*pointers::script_guid_pool)->GetIsFree(handle >> 8) == false);

		reference &= POOL_FLAG_REFERENCEMASK;

		handle &= ~POOL_FLAG_REFERENCEMASK;
		handle |= reference;

		(*pointers::script_guid_pool)->SetReference(handle >> 8, reference);
	}

	bool AbstractEntity::ensurePointer()
	{
		if (ptr != nullptr)
		{
			return true;
		}
		return findPointer();
	}

	bool AbstractEntity::findPointer()
	{
		if (handle != INVALID_GUID)
		{
			if (auto guid = (*pointers::script_guid_pool)->GetAt(handle))
			{
				ptr = reinterpret_cast<CPhysical*>(guid->m_pBase);
				return true;
			}
		}
		return false;
	}

	bool AbstractEntity::ensureHandleAndPointer()
	{
		if (handle == INVALID_GUID)
		{
			return findHandle();
		}
		else if (ptr == nullptr)
		{
			return findPointer();
		}
		else
		{
			return true;
		}
	}

	Entity AbstractEntity::getHandle()
	{
		if (handle == INVALID_GUID)
		{
			findHandle();
		}
		return handle;
	}

	CPhysical* AbstractEntity::getPointer()
	{
		if (ptr == nullptr)
		{
			findPointer();
		}
		return ptr;
	}

	void* AbstractEntity::getNetObjectImpl()
	{
		auto* const ptr = getPointer();
		if (ptr != nullptr)
		{
			return ptr->m_net_object;
		}
		return nullptr;
	}

	AbstractEntity::operator Entity()
	{
		return getHandle();
	}

	AbstractEntity::operator CPed* ()
	{
		return getCPed();
	}

	AbstractEntity::operator CVehicle* ()
	{
		return getCVehicle();
	}

	bool AbstractEntity::operator==(AbstractEntity& b)
	{
		if (handle != INVALID_GUID && b.handle != INVALID_GUID)
		{
			return handle == b.handle;
		}
		return getPointer() == b.getPointer();
	}

	bool AbstractEntity::operator==(AbstractEntity&& b)
	{
		return operator==(b);
	}

	bool AbstractEntity::operator!=(AbstractEntity& b)
	{
		return !operator==(b);
	}

	void AbstractEntity::operator=(const AbstractEntity& b)
	{
		this->ptr = b.ptr;
		this->handle = b.handle;
	}

	std::size_t AbstractEntity::hash() const noexcept
	{
		return (((uint64_t)ptr) << 32) | (uint32_t)handle;
	}

	void AbstractEntity::reset() noexcept
	{
		this->ptr = nullptr;
		this->handle = INVALID_GUID;
	}

	bool AbstractEntity::isValid() const noexcept
	{
		return ptr != nullptr || handle != INVALID_GUID;
	}

	bool AbstractEntity::isFullyUsable()
	{
		return ensureHandleAndPointer() && ptr != nullptr && handle != INVALID_GUID;
	}

	bool AbstractEntity::exists()
	{
		return ensureHandleAndPointer()
			&& AbstractEntity::get(handle).getPointer() == ptr;
	}

	eEntityType AbstractEntity::getType()
	{
		if (ensurePointer())
		{
			return (eEntityType)ptr->type;
		}
		return ENTITY_TYPE_NOTHING;
	}

	bool AbstractEntity::isPed()
	{
		return getType() == ENTITY_TYPE_PED;
	}

	bool AbstractEntity::isVehicle()
	{
		return getType() == ENTITY_TYPE_VEHICLE;
	}

	bool AbstractEntity::isObject()
	{
		return getType() == ENTITY_TYPE_OBJECT;
	}

	bool AbstractEntity::isPickup()
	{
		return isObject()
			&& ensureHandle()
			&& OBJECT::IS_OBJECT_A_PICKUP(handle);
	}

	static std::array<std::unordered_map<Entity, std::string>, 2> name_caches = {};

	std::string AbstractEntity::getName(name_t gender)
	{
		if (ensureHandle())
		{
			auto* name_cache = &name_caches.at(gender);
			auto entry = name_cache->find(handle);
			if (entry != name_cache->end())
			{
				return entry->second;
			}
			std::string name{ Names::randFirstName(gender) };
			name.push_back(' ');
			name.append(Names::randLastName());
			name_cache->emplace(handle, name);
			return name;
		}
		return {};
	}

	std::string AbstractEntity::getName(bool include_owner)
	{
		std::string name = {};
		if (isVehicle())
		{
			if (auto mi = getModelInfo())
			{
				name = get_vehicle_display_name(reinterpret_cast<CVehicleModelInfo*>(getModelInfo()));
			}
		}
		else if (isPed())
		{
			if (isAPlayer())
			{
				return getPlayer().getNameWithFlags();
			}

			const Hash model_hash = getModel();
			switch (model_hash)
			{
			case ATSTRINGHASH("s_m_y_ranger_01"):
			case ATSTRINGHASH("s_m_y_uscg_01"):
				name = getName(NAME_MALE);
				break;

			case ATSTRINGHASH("s_f_y_ranger_01"):
				name = getName(NAME_FEMALE);
				break;

			default:
				switch (PED::GET_PED_TYPE(operator Entity()))
				{
				case PED_TYPE_CIVMALE:
					name = getName(NAME_MALE);
					break;

				case PED_TYPE_CIVFEMALE:
					name = getName(NAME_FEMALE);
					break;

				default:
					auto model = PedModel::fromHash(model_hash);
					if (model != nullptr)
					{
						name = model->menu_name.getLocalisedUtf8();
					}
					break;
				}
			}
		}
		else
		{
			name = joaatToString(getModelHash());
		}
		if (include_owner && getNetObject() != nullptr)
		{
			name.append((const char*)u8" - ").append(getOwner().getNameWithFlags());
		}
		return name;
	}

	v3 AbstractEntity::getPos()
	{
		if (ensurePointer())
		{
			return ptr->matrix.getTranslate();
		}
		return {};
	}

	v3 AbstractEntity::getVisualPos()
	{
		if (ensurePointer())
		{
			return { ptr->matrix.x, ptr->matrix.y, ptr->matrix.z };
		}
		return {};
	}

	void AbstractEntity::setPos(const v3& pos)
	{
		if (ensureHandle())
		{
			BOOL graceful = TRUE;
			if (isPed())
			{
				if (isParachuting() // Can't keep this task if we want the coords to be updated.
					|| PED::IS_PED_IN_COVER(*this, FALSE) // Can keep the task, but it would be buggy.
					)
				{	
					graceful = FALSE;
				}
			}
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(handle, pos.x, pos.y, pos.z, /* KeepTasks */ graceful, /* KeepIK */ graceful, /* DoWarp */ graceful);
		}
	}

	void AbstractEntity::setOrigin(const v3& pos)
	{
		setPos(pos - getOriginOffset());
	}

	v3 AbstractEntity::getRot()
	{
		if (ensureHandle())
		{
			__try
			{
				return ENTITY::GET_ENTITY_ROTATION(handle, 2);
			}
			__EXCEPTIONAL()
			{
			}
		}
		return {};
	}

	void AbstractEntity::setRot(const v3& rot)
	{
		if (ensureHandle())
		{
			ENTITY::SET_ENTITY_ROTATION(handle, rot.x, rot.y, rot.z, 2, true);
		}
	}

	bool AbstractEntity::isInLineOfSight()
	{
		return isInLineOfSight(CAM::GET_FINAL_RENDERED_CAM_COORD());
	}

	bool AbstractEntity::isInLineOfSight(const v3& observer_pos)
	{
		return isInLineOfSight(observer_pos, getPos());
	}

	bool AbstractEntity::isInLineOfSight(const v3& observer_pos, const v3& pos)
	{
		if (pos.z > observer_pos.z || pos.distance(observer_pos) < 1000.0f)
		{
			const int shapetest_handle = SHAPETEST::START_EXPENSIVE_SYNCHRONOUS_SHAPE_TEST_LOS_PROBE(observer_pos.x, observer_pos.y, observer_pos.z, pos.x, pos.y, pos.z, -1, g_player_ped, 0);
			BOOL hit;
			Vector3 endCoords;
			Vector3 surfaceNormal;
			Entity entityHit;
			SHAPETEST::GET_SHAPE_TEST_RESULT(shapetest_handle, &hit, &endCoords, &surfaceNormal, &entityHit);
			if (!hit || entityHit == operator Entity() || (isPed() && PED::GET_VEHICLE_PED_IS_IN(operator Entity(), false) == entityHit))
			{
				return true;
			}
		}
		return false;
	}

	AbstractModel AbstractEntity::getModel()
	{
		return getModelHash();
	}

	CBaseModelInfo* AbstractEntity::getModelInfo()
	{
		if (ensurePointer())
		{
			return ptr->archetype;
		}
		return nullptr;
	}

	Hash AbstractEntity::getModelHash()
	{
		if (auto model_info = getModelInfo())
		{
			return model_info->hash;
		}
		return 0;
	}

	AbstractPlayer AbstractEntity::getOwner()
	{
		if (*pointers::is_session_started
			&& ensurePointer()
			&& ptr->m_net_object != nullptr
			)
		{
			return AbstractPlayer(ptr->m_net_object->owner_id);
		}
		return g_player;
	}

	bool AbstractEntity::isOwner()
	{
		return getOwner() == g_player;
	}

	void AbstractEntity::requestControl(Hash reason, std::function<void(AbstractEntity&)>&& callback, std::function<void(AbstractEntity&)>&& timeout_callback)
	{
		if (!ensureHandle())
		{
			return;
		}
		if (isOwner())
		{
			callback(*this);
		}
		else if (control_requests.find(handle) == control_requests.end())
		{
			//g_logger.log(fmt::format("Requesting control of entity {} for {}...", handle, reason));
			control_requests.emplace(handle, std::pair<time_t, std::unordered_map<Hash, std::pair<std::function<void(AbstractEntity&)>, std::function<void(AbstractEntity&)>>>>{ get_current_time_millis() + 6000, { {std::move(reason), {std::move(callback), std::move(timeout_callback)} } } });
		}
		else if (isAPlayer())
		{
			timeout_callback(*this);
		}
		else
		{
			if (control_requests.at(handle).second.find(reason) == control_requests.at(handle).second.end())
			{
				control_requests.at(handle).second.emplace(std::move(reason), std::pair(std::move(callback), std::move(timeout_callback)));
			}
		}
	}

	bool AbstractEntity::canBeDeleted()
	{
		return isValid()
			&& exists()
			&& !isAPlayer()
			&& *this != g_preview_ent
			;
	}

	void AbstractEntity::removeFromPlaneOfExistence()
	{
		if (canBeDeleted())
		{
			removeFromPlaneOfExistenceNocheck();
		}
	}

	void AbstractEntity::removeFromPlaneOfExistenceNocheck()
	{
#ifdef STAND_DEBUG
		SOUP_ASSERT(ExecCtx::get().isScript()); // Needed, at least for vehicles.
#endif
		removeFromPlaneOfExistenceRemoveRelationships();
		if (isOwner())
		{
			removeFromPlaneOfExistenceAfterControlRequest();
		}
		else
		{
			if (getOwner().isBlockingRequestControl())
			{
				// Don't even bother requesting control and just delete locally only.
				// We need to do this or else anti-entity spam might not be fast enough.
				removeFromPlaneOfExistenceForce();
			}
			else
			{
				requestControl(ATSTRINGHASH("delete"), [](AbstractEntity& ent)
				{
					ent.removeFromPlaneOfExistenceAfterControlRequest();
				}, [](AbstractEntity& ent)
				{
					ent.removeFromPlaneOfExistenceForce();
				});
			}
		}
	}

	void AbstractEntity::removeFromPlaneOfExistenceRemoveRelationships()
	{
		if (isVehicle())
		{
			makePassengersLeave();
			
			if (TranscendentVehicle::active && TranscendentVehicle::ent == *this)
			{
				TranscendentVehicle::active = false;
			}

			if (*this == g_player_veh)
			{
				g_player_veh.reset();
				updatePlayerEnt();
			}
		}
	}

	void AbstractEntity::removeFromPlaneOfExistenceAfterControlRequest()
	{
		ExecCtx::get().ensureScript([this]
		{
			removeFromPlaneOfExistenceRemoveRelationships();

			if (auto handle = operator Entity())
			{
				// Reduce performance impact of entity
				ENTITY::SET_ENTITY_HAS_GRAVITY(handle, FALSE);
				ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(handle, FALSE, FALSE);
				if (isPed())
				{
					TASK::CLEAR_PED_TASKS_IMMEDIATELY(handle);
				}
				ENTITY::SET_ENTITY_VELOCITY(handle, 0.0f, 0.0f, 0.0f);
			}

			removeFromPlaneOfExistenceInner();
		});
	}

	bool AbstractEntity::removeFromPlaneOfExistenceForce()
	{
		return removeFromPlaneOfExistenceInner();
	}

	bool AbstractEntity::removeFromPlaneOfExistenceInner()
	{
		// Is fine to be called outside of a script thread, at least for vehicles.

		if (ensurePointer())
		{
			if (!isOwner())
			{
				// If we only wanted to delete it locally, we could do this even as owner, but that would be very rude.
				if (ptr->m_net_object != nullptr)
				{
					(*pointers::network_object_mgr)->unregisterNetworkObject(ptr->m_net_object, 0, true, true);
				}
			}

			switch (getType())
			{
			case ENTITY_TYPE_PED:
				return pointers::CPedFactory_DestroyPed(*pointers::ped_factory, static_cast<CPed*>(ptr), false);

			case ENTITY_TYPE_VEHICLE:
				pointers::vehicle_commands__DeleteScriptVehicle(static_cast<CVehicle*>(ptr));
				return true;

			case ENTITY_TYPE_OBJECT:
				pointers::CObjectPopulation_DestroyObject(static_cast<CObject*>(ptr), false);
				return true;
			}
		}
		return false;
	}

	bool AbstractEntity::isInvulnerable()
	{
		if (ensurePointer())
		{
			if (auto p = getPlayer(); p.isValid())
			{
				if (NETWORK::NETWORK_IS_ACTIVITY_SESSION())
				{
					return ptr->m_nPhysicalFlags.bNotDamagedByAnything
						|| ptr->m_nPhysicalFlags.bNotDamagedByAnythingButHasReactions
						;
				}

				if (p.isInInterior() && !p.isInStore())
				{
					return ptr->m_nPhysicalFlags.bNotDamagedBySteam
						|| ptr->m_nPhysicalFlags.bNotDamagedBySmoke
						;
				}
			}

			if (ptr->m_nPhysicalFlags.bNotDamagedByBullets
				|| ptr->m_nPhysicalFlags.bNotDamagedByCollisions
				|| ptr->m_nPhysicalFlags.bNotDamagedByMelee
				|| ptr->m_nPhysicalFlags.bIgnoresExplosions
				|| ptr->m_nPhysicalFlags.bNotDamagedByAnything
				|| ptr->m_nPhysicalFlags.bNotDamagedByAnythingButHasReactions
				|| ptr->m_nPhysicalFlags.bOnlyDamagedByRelGroup
				)
			{
				return true;
			}
			if (!isVehicle())
			{
				if (ptr->m_nPhysicalFlags.bNotDamagedByFlames // khanjali has this set
					|| ptr->m_nPhysicalFlags.bNotDamagedBySteam
					|| ptr->m_nPhysicalFlags.bNotDamagedBySmoke
					)
				{
					return true;
				}
			}
		}
		return false;
	}

	float AbstractEntity::getHealth()
	{
		if (ensurePointer())
		{
			return ptr->m_health;
		}
		return 0.0f;
	}

	void AbstractEntity::setHealth(float health)
	{
		if (isPed())
		{
			if (getHealth() <= 0.0f)
			{
				if (health > 0.0f && ensureHandle())
				{
					PED::RESURRECT_PED(handle);
					ENTITY::SET_ENTITY_HEALTH(handle, (int)health, 0, 0);
					TASK::CLEAR_PED_TASKS_IMMEDIATELY(handle);
				}
			}
			else if (ptr != nullptr)
			{
				((CPed*)ptr)->m_health = health;
			}
			else if (handle != INVALID_GUID)
			{
				// Using ENTITY::SET_ENTITY_HEALTH to kill entities seems to trigger RAC
				if (health <= 0.0f)
				{
					PED::APPLY_DAMAGE_TO_PED(handle, (int)getHealth(), true, 0, 0);
				}
				else
				{
					ENTITY::SET_ENTITY_HEALTH(handle, (int)health, 0, 0);
				}
			}
		}
		else if (ensureHandle())
		{
			ENTITY::SET_ENTITY_HEALTH(handle, (int)health, 0, 0);
		}
	}

	bool AbstractEntity::isDead()
	{
		if (ptr != nullptr)
		{
			if (isPed())
			{
				return ((CPed*)ptr)->m_health <= 0.0f;
			}
			else if(ensureHandle())
			{
				return ENTITY::IS_ENTITY_DEAD(handle, false);
			}
		}
		else if (handle != INVALID_GUID)
		{
			return ENTITY::IS_ENTITY_DEAD(handle, false);
		}
		return true;
	}

	bool AbstractEntity::isFrozen()
	{
		if (isAPlayer())
		{
			return getPlayer().isFrozen();
		}
		if (auto p = getPointer())
		{
			return (p->base_flags & rage::fwEntity::IS_FIXED);
		}
		return false;
	}

	void AbstractEntity::freeze()
	{
		if (isAPlayer())
		{
			getPlayer().freeze(true);
		}
		else
		{
			requestControl(ATSTRINGHASH("freeze"), [](AbstractEntity& ent)
			{
				ENTITY::FREEZE_ENTITY_POSITION(ent, true);
				ent.setCanMigrate(false);
			});
		}
	}

	void AbstractEntity::unfreeze()
	{
		if (isAPlayer())
		{
			getPlayer().freeze(false);
		}
		else
		{
			requestControl(ATSTRINGHASH("unfreeze"), [](AbstractEntity& ent)
			{
				ENTITY::FREEZE_ENTITY_POSITION(ent, false);
				ent.setCanMigrate(true);
			});
		}
	}

	Vector3 AbstractEntity::getDimensions()
	{
		Vector3 min;
		Vector3 dimensions;
		MISC::GET_MODEL_DIMENSIONS(getModel(), &min, &dimensions);
		dimensions -= min;
		if (*this == g_player_ped)
		{
			dimensions *= g_gui.character_scale_multiplier;
		}
		return dimensions;
	}

	Vector3 AbstractEntity::getOrigin()
	{
		Vector3 min;
		Vector3 max;
		MISC::GET_MODEL_DIMENSIONS(getModel(), &min, &max);
		return ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(operator Entity(), ((max.x - min.x) / 2.0f) + min.x, ((max.y - min.y) / 2.0f) + min.y, ((max.z - min.z) / 2.0f) + min.z);
	}

	v3 AbstractEntity::getOriginOffset()
	{
		return getOrigin() - getPos();
	}

	Box AbstractEntity::getBox()
	{
		if (ensureHandle())
		{
			Vector3 upVector, rightVector, forwardVector, position;
			ENTITY::GET_ENTITY_MATRIX(handle, &rightVector, &forwardVector, &upVector, &position);
			return Box(getOrigin(), getDimensions() * 0.5f, rightVector, forwardVector, upVector);
		}
		return Box();
	}

	float AbstractEntity::airDistance(AbstractEntity& b)
	{
		float dist = getPos().distance(b.getPos());
		dist -= getDimensions().max();
		dist -= b.getDimensions().max();
		return std::max(0.0f, dist);
	}

	bool AbstractEntity::canTarget(const TargetingData& data, bool ped_to_vehicle, bool consider_veh_as_player)
	{
		bool ret = false;
		if (auto wpn = Weapon::find(g_player_ped.getSelectedWeapon()))
		{
			if (wpn->category == Weapon::THROWABLE)
			{
				return ret;
			}
		}
		if (ensureHandle())
		{
			auto ent = *this;
			if (data.use_range
				&& ent.getPos().distance(g_player_ped.getPos()) > data.range
				)
			{
				return false;
			}
			switch (getType())
			{
			case ENTITY_TYPE_PED:
				if (isAPlayer() && data.excludes.isExcluded(getPlayer()))
				{
					break;
				}
				if (!ENTITY::IS_ENTITY_DEAD(ent, false))
				{
					if (PED::IS_PED_A_PLAYER(ent))
					{
						if (data.players
							&& !ent.isPassiveMode()
							)
						{
							ret = true;
						}
					}
					else
					{
						if (data.peds)
						{
							ret = true;
						}
					}
				}
				if (!ped_to_vehicle)
				{
					break;
				}
				ent = ent.getVehicle(false);
				if (!ent.isValid())
				{
					break;
				}
				[[fallthrough]];
			case ENTITY_TYPE_VEHICLE:
				if (consider_veh_as_player
					&& ent.getDriver().isAPlayer()
					)
				{
					ret = data.players;
				}
				else
				{
					ret = data.vehicles && !ENTITY::IS_ENTITY_DEAD(ent, false);
				}
				break;

			case ENTITY_TYPE_OBJECT:
				ret = data.objects;
				break;
			}
		}
		return ret;
	}

	rage::fwExtension* AbstractEntity::getExtension(uint32_t id)
	{
		if (ensurePointer())
		{
			return getExtensionInner(id);
		}
		return nullptr;
	}

	rage::fwExtension* AbstractEntity::getExtensionInner(uint32_t id) const
	{
		return ptr->extensions.Get(id);
	}

	void AbstractEntity::setCanMigrate(bool can_migrate)
	{
		if (auto netobj = this->getNetObject())
		{
			if (can_migrate)
			{
				netobj->global_flags &= ~rage::netObject::GLOBALFLAG_PERSISTENTOWNER;
			}
			else
			{
				netobj->global_flags |= rage::netObject::GLOBALFLAG_PERSISTENTOWNER;
			}
		}
	}

	bool AbstractEntity::getCanMigrate()
	{
		if (auto netobj = this->getNetObject())
		{
			return !(netobj->global_flags & rage::netObject::GLOBALFLAG_PERSISTENTOWNER);
		}
		return false;
	}

	void AbstractEntity::setAlwaysSynch(bool always_synch)
	{
		if (auto netobj = this->getNetObject())
		{
			if (always_synch)
			{
				netobj->global_flags |= rage::netObject::GLOBALFLAG_CLONEALWAYS;
			}
			else
			{
				netobj->global_flags &= ~rage::netObject::GLOBALFLAG_CLONEALWAYS;
			}
		}
	}

	uint16_t AbstractEntity::getNetObjId()
	{
		if (auto net = getNetObject())
		{
			return net->object_id;
		}
		return NETWORK_INVALID_OBJECT_ID;
	}

	bool AbstractEntity::isNetObjId(uint16_t id)
	{
		if (auto net = getNetObject())
		{
			if (net->object_id == id)
			{
				return true;
			}
		}
		return false;
	}

	void AbstractEntity::giveControl(AbstractPlayer p)
	{
		if (p == g_player)
		{
			return;
		}
		if (auto netply = p.getCNetGamePlayer())
		{
			if (auto netobj = getNetObject())
			{
				pointers::CGiveControlEvent_Trigger(reinterpret_cast<const rage::netPlayer*>(netply), netobj, rage::MIGRATE_FORCED);
			}
		}
	}

	bool AbstractEntity::isInWater()
	{
		if (auto pEntity = getPointer())
		{
			if (pEntity->m_net_object
				&& pEntity->m_net_object->is_clone
				&& static_cast<CNetObjPhysical*>(pEntity->m_net_object)->m_isInWater
				)
			{
				return true;
			}
			else if (pEntity->type == ENTITY_TYPE_PED)
			{
				const CPed* pPed = static_cast<const CPed*>(pEntity);

				if (pPed->m_PedConfigFlags.m_Flags.get(CPED_CONFIG_FLAG_InVehicle)
					&& pPed->current_or_last_vehicle
					)
				{
					if (pPed->current_or_last_vehicle->m_nVehicleFlags.bIsDrowning)
					{
						return true;
					}
				}
				else if (pPed->m_PedConfigFlags.m_Flags.get(CPED_CONFIG_FLAG_IsSwimming))
				{
					return true;
				}
				else
				{
					return pPed->m_nPhysicalFlags.bIsInWater;
				}
			}
		}
		return false;
	}

	float AbstractEntity::getSpeed()
	{
		SOUP_IF_UNLIKELY (isParachuting())
		{
			return std::max(ENTITY::GET_ENTITY_SPEED(*this), ENTITY::GET_ENTITY_SPEED(ENTITY::GET_ENTITY_ATTACHED_TO(*this)));
		}
		return ENTITY::GET_ENTITY_SPEED(*this);
	}

	Vector3 AbstractEntity::getBonePos(PedBoneId boneId)
	{
		if (ensureHandle())
		{
			auto bone_idx = PED::GET_PED_BONE_INDEX(handle, boneId);
			if (bone_idx != -1)
			{
				return ENTITY::GET_WORLD_POSITION_OF_ENTITY_BONE(handle, bone_idx);
			}
		}
		return {};
	}

	Vector3 AbstractEntity::getHeadPos()
	{
		return getBonePos(SKEL_Head);
	}

	bool AbstractEntity::isAiming()
	{
		return ensureHandle()
			&& PED::GET_PED_CONFIG_FLAG(handle, 78, 1)
			;
	}

	bool AbstractEntity::isAPlayer()
	{
		return getPlayerInfo() != nullptr;
	}

	bool AbstractEntity::couldBePlayer()
	{
		return *pointers::is_session_started ? getModel().isOnlineCharacter() : isAPlayer();
	}

	CPlayerInfo* AbstractEntity::getPlayerInfo()
	{
		if (ensurePointer() && isPed())
		{
			return getCPedRaw()->player_info;
		}
		return nullptr;
	}

	compactplayer_t AbstractEntity::getPlayerId()
	{
		if (ensurePointer() && isPed())
		{
			auto cped = getCPedRaw();
			if (cped->IsAPlayerPed())
			{
				if (cped->m_net_object)
				{
					return cped->m_net_object->owner_id;
				}
				return g_player; // make sure this works in SP
			}
		}
		return -1;
	}

	AbstractPlayer AbstractEntity::getPlayer()
	{
		return AbstractPlayer(getPlayerId());
	}

	float AbstractEntity::getMaxHealth()
	{
		if (ensurePointer() && isPed())
		{
			return getCPedRaw()->max_health;
		}
		return 0.0f;
	}

	float AbstractEntity::getArmour()
	{
		if (ensurePointer() && isPed())
		{
			return getCPedRaw()->m_armor;
		}
		return 0.0f;
	}

	void AbstractEntity::setMaxHealth(float max_health)
	{
		SOUP_IF_LIKELY (auto cped = getCPed())
		{
			const bool was_at_max = cped->m_health == cped->max_health;
			cped->max_health = max_health;

			if (cped->m_health > max_health || was_at_max)
			{
				cped->m_health = max_health;
			}
		}
	}

	void AbstractEntity::setArmour(float armor)
	{
		if (isPed())
		{
			if (ptr != nullptr)
			{
				((CPed*)ptr)->m_armor = armor;
			}
			else if (handle != INVALID_GUID)
			{
				PED::SET_PED_ARMOUR(handle, (int)armor);
			}
		}
	}

	float AbstractEntity::getMaxArmour()
	{
		return is_session_or_transition_active() ? 50.0f : 100.0f;
	}

	bool AbstractEntity::isFriend()
	{
		return isAPlayer() ? getPlayer().isFriend() : PED::GET_RELATIONSHIP_BETWEEN_PEDS(g_player_ped, operator Entity()) <= 1;
	}

	bool AbstractEntity::isInAnyVehicle()
	{
		if (auto p = getCPed())
		{
			return p->is_in_vehicle;
		}
		return false;
	}

	int AbstractEntity::getSeat(AbstractEntity& veh)
	{
		if (ensureHandle() && veh.ensureHandle())
		{
			const int passengers = veh.getMaxPassengers();
			for (int i = -1; i < passengers; i++)
			{
				if (VEHICLE::GET_PED_IN_VEHICLE_SEAT(veh.handle, i, 0) == handle)
				{
					return i;
				}
			}
		}
		return -3;
	}

	void AbstractEntity::giveWeapons(std::vector<hash_t>&& weapons, const bool force)
	{
		if (!isAPlayer() || *this == g_player_ped)
		{
			requestControl(ATSTRINGHASH("arm"), [force, weapons{ std::move(weapons) }](AbstractEntity& ped)
			{
				if (ped.ensureHandle())
				{
					for (const auto& weapon : weapons)
					{
						if (!WEAPON::HAS_PED_GOT_WEAPON(ped, weapon, FALSE))
						{
							WEAPON::GIVE_WEAPON_TO_PED(ped.handle, weapon, 9999, force, force);
						}
					}
				}
			});
		}
		else
		{
			const compactplayer_t p = getPlayerId();
			if (TickQueues::arm_queue.find(p) == TickQueues::arm_queue.end())
			{
				std::queue<hash_t> queue = {};
				for (const auto& weapon : weapons)
				{
					queue.emplace(weapon);
				}
				TickQueues::arm_queue.emplace(p, queue);
			}
		}
	}

	void AbstractEntity::giveAllWeapons()
	{
		giveWeapons(Weapon::getAllHashes());
	}

	void AbstractEntity::removeWeapons(std::vector<hash_t>&& weapons)
	{
		if (!isAPlayer() || *this == g_player_ped)
		{
			requestControl(ATSTRINGHASH("disarm"), [weapons{ std::move(weapons) }](AbstractEntity& ped)
			{
				if (ped.ensureHandle())
				{
					for (const auto& weapon : weapons)
					{
						WEAPON::REMOVE_WEAPON_FROM_PED(ped.handle, weapon);
					}
				}
			});
		}
		else
		{
			auto p = (uint64_t)getPlayerId();
			for (const auto& weapon : weapons)
			{
				// WEAPON::HAS_PED_GOT_WEAPON is unreliable for players
				const auto key = (p << 32) | (uint32_t)weapon;
				auto i = TickQueues::disarm_queue.find(key);
				if (i == TickQueues::disarm_queue.end())
				{
					//util::toast(fmt::format("Enqueued removal of {} for {} ({})", weapon.first, p.get_name(), key), TOAST_LOGGER);
					TickQueues::disarm_queue.emplace(key, std::pair<bool, time_t>{ true, get_current_time_millis() + 3000ll });
				}
				else if (!i->second.first && i->second.second < get_current_time_millis())
				{
					i->second.first = true;
				}
			}
		}
	}

	void AbstractEntity::removeAllWeapons()
	{
		removeWeapons(Weapon::getAllHashesIncludingWeirdos());
	}

	AbstractEntity AbstractEntity::getCurrentWeaponEntity()
	{
		if (ensureHandle())
		{
			return get(WEAPON::GET_CURRENT_PED_WEAPON_ENTITY_INDEX(handle, 0));
		}
		return invalid();
	}

	v3 AbstractEntity::getCurrentWeaponPos()
	{
		v3 pos = getCurrentWeaponEntity().getPos();
		const auto weapon = Weapon::find(WEAPON::GET_SELECTED_PED_WEAPON(operator Entity()));
		if (weapon != nullptr && weapon->category == Weapon::SNIPER)
		{
			pos.z -= 0.006f;
		}
		else
		{
			pos.z -= 0.01f;
		}
		return pos;
	}

	CPedHeadBlendData* AbstractEntity::getHeadBlendData()
	{
		return static_cast<CPedHeadBlendData*>(getExtension(*pointers::entity_extension_id_head_blend_data));
	}

	int AbstractEntity::getComponentTexture(int component_id)
	{
		if (component_id == 2 && getModel().isOnlineCharacter())
		{
			CPedHeadBlendData* const head_blend_data = getHeadBlendData();
			if (head_blend_data != nullptr)
			{
				return head_blend_data->hair_colour_id;
			}
		}
		return PED::GET_PED_TEXTURE_VARIATION(*this, component_id);
	}

	int AbstractEntity::getComponentMaxDrawable(int component_id)
	{
		return PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(*this, component_id) - 1;
	}

	void AbstractEntity::ensureHasParachute()
	{
		if (ensureHandle() && !WEAPON::HAS_PED_GOT_WEAPON(handle, ATSTRINGHASH("GADGET_PARACHUTE"), false))
		{
			giveParachute();
		}
	}

	void AbstractEntity::giveParachute()
	{
		WEAPON::GIVE_DELAYED_WEAPON_TO_PED(handle, ATSTRINGHASH("GADGET_PARACHUTE"), 1, false);
	}

	void AbstractEntity::randomiseOutfit()
	{
		if (ensureHandle())
		{
			for (const PedComponent& component : ped_components)
			{
				auto drawable = soup::rand.t<int>(0, getComponentMaxDrawable(component.id));
				PED::SET_PED_COMPONENT_VARIATION(
					handle,
					component.id,
					drawable,
					soup::rand.t<int>(0, PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(handle, component.id, drawable) - 1),
					0
				);
			}
			for (const PedProp& prop : ped_props)
			{
				auto drawable = soup::rand.t<int>(0, PED::GET_NUMBER_OF_PED_PROP_DRAWABLE_VARIATIONS(handle, prop.id) - 1);
				PED::SET_PED_PROP_INDEX(
					handle,
					prop.id,
					drawable,
					soup::rand.t<int>(0, PED::GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(handle, prop.id, drawable) - 1),
					true,
					1
				);
			}
		}
	}

	void AbstractEntity::godmodeEnable(const bool no_ragdoll)
	{
		if (auto p = getCPed())
		{
			if (no_ragdoll)
			{
				p->m_nPhysicalFlags.bNotDamagedByAnything = true;
			}
			else
			{
				p->m_nPhysicalFlags.bNotDamagedByAnythingButHasReactions = true;
			}
		}
		if (ensureHandle())
		{
			ENTITY::SET_ENTITY_PROOFS(handle, 1, 1, 1, 1, 1, 1, 1, 1);
		}
	}

	void AbstractEntity::godmodeDisable()
	{
		if (auto p = getCPed())
		{
			p->m_nPhysicalFlags.bNotDamagedByAnything = false;
			p->m_nPhysicalFlags.bNotDamagedByAnythingButHasReactions = false;
		}
		if (ensureHandle())
		{
			ENTITY::SET_ENTITY_PROOFS(handle, 0, 0, 0, 0, 0, 0, 0, 0);
		}
	}

	void AbstractEntity::revive()
	{
		if (ensureHandle() && getHealth() <= 0.0f)
		{
			setHealth((float)PED::GET_PED_MAX_HEALTH(handle));
		}
	}

	bool AbstractEntity::isInBubble()
	{
		return getPos().z != -50.0f;
	}

	AbstractEntity AbstractEntity::getVehicle(bool include_last)
	{
		if (auto p = getCPed())
		{
			if (auto veh = p->getVehicle(include_last))
			{
				return AbstractEntity::get(veh);
			}
		}
		return invalid();
	}

	bool AbstractEntity::isOnGround()
	{
		return ensureHandle()
			&& PED::GET_PED_CONFIG_FLAG(handle, 60, FALSE) // on ground
			&& !PED::GET_PED_CONFIG_FLAG(g_player_ped, 65, FALSE) // in water
			&& !PED::IS_PED_RAGDOLL(g_player_ped)
			;
	}

	bool AbstractEntity::hasHeadroom()
	{
		auto pos = getPos();
		const int shapetest_handle = SHAPETEST::START_EXPENSIVE_SYNCHRONOUS_SHAPE_TEST_LOS_PROBE(pos.x, pos.y, pos.z, pos.x, pos.y, pos.z + 10.0f, 1, *this, 0);
		BOOL hit;
		Vector3 endCoords;
		Vector3 surfaceNormal;
		Entity entityHit;
		SHAPETEST::GET_SHAPE_TEST_RESULT(shapetest_handle, &hit, &endCoords, &surfaceNormal, &entityHit);
		return !hit;
	}

	void AbstractEntity::setConfigFlag(int flag, bool value)
	{
		if (isPed())
		{
			PED::SET_PED_CONFIG_FLAG(*this, flag, value);
		}
	}

	void AbstractEntity::setCombatAttr(int attr_id, bool value)
	{
		if (isPed())
		{
			PED::SET_PED_COMBAT_ATTRIBUTES(*this, attr_id, value);
		}
	}

	static const hash_t cage_objects[] = {
		ATSTRINGHASH("prop_gold_cont_01"),
		ATSTRINGHASH("prop_rub_cage01a"),
		ATSTRINGHASH("prop_fnclink_03e"),
		ATSTRINGHASH("stt_prop_stunt_tube_s"),
	};

	void AbstractEntity::cage(Hash cage_model)
	{
		const auto cage_hash = cage_model == 0 ? ARRAY_RAND(cage_objects) : cage_model;

		switch (cage_hash)
		{
		case ATSTRINGHASH("prop_gold_cont_01"):
			{
				Util::createObject(cage_hash, this->getPos()).freeze();

				break;
			}

		case ATSTRINGHASH("stt_prop_stunt_tube_s"):
			{
				auto obj = Util::createObject(cage_hash, this->getPos());
				auto rot = obj.getRot();
				rot.y += 90.0f;

				obj.setRot(rot);
				obj.freeze();

				break;
			}

		case ATSTRINGHASH("prop_rub_cage01a"):
			{
				auto pos = this->getPos();
				pos.z -= 1.0f;

				auto obj1 = Util::createObject(cage_hash, pos);
				auto obj2 = Util::createObject(cage_hash, pos);

				auto rot = obj1.getRot();
				rot.z += 90.0f;

				obj2.setRot(rot);
				obj2.freeze();
				obj1.freeze();

				break;
			}

		case ATSTRINGHASH("prop_fnclink_03e"):
			{
				auto pos = this->getPos() - 1.0f;

				auto obj1 = Util::createObject(cage_hash, pos);
				auto obj2 = Util::createObject(cage_hash, pos);

				pos.x += 2.9f;
				pos.y += 2.9f;

				auto obj3 = Util::createObject(cage_hash, pos);
				auto obj4 = Util::createObject(cage_hash, pos);

				auto rot = obj1.getRot();

				rot.z += 90.0f;
				obj2.setRot(rot);

				rot.z += 180.0f;
				obj3.setRot(rot);

				rot.z += 270.0f;
				obj4.setRot(rot);

				obj1.freeze();
				obj2.freeze();
				obj3.freeze();
				obj4.freeze();

				break;
			}
		}
	}

	void AbstractEntity::ragdoll()
	{
		if (isAPlayer())
		{
			if (pointers::CRagdollRequestEvent_Trigger)
			{
				pointers::CRagdollRequestEvent_Trigger(getCPed()->m_net_object->object_id);
			}
		}
		else
		{
			NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(*this);

			if (getVehicle(false).isValid())
			{
				TASK::CLEAR_PED_TASKS_IMMEDIATELY(*this);
			}

			PED::SET_PED_TO_RAGDOLL(*this, 3000, 0, 0, true, true, false);
		}
	}

	Hash AbstractEntity::getSelectedWeapon()
	{
		if (const auto cped = getCPed())
		{
			if (const auto mgr = cped->weapon_manager)
			{
				return mgr->selector.selected_weapon_hash;
			}
		}

		return 0;
	}

	void AbstractEntity::sendMagicBullet(Hash weapon, float damage, bool nullify)
	{
		if (damage != 0.0f)
		{
			auto pos = getPos();
			pointers::CWeaponDamageEvent_Trigger(
				(CEntity*)g_player_ped.getCPed(), // pParentEntity
				*this, // pHitEntity
				&pos, // worldHitPosition
				0, // hitComponent
				true, // bOverride
				weapon, // weaponHash
				damage * (nullify ? 0.5f : 1.0f), // weaponDamage
				0, // tyreIndex
				0, // suspensionIndex
				eDamageFlags::IsAccurate | eDamageFlags::AllowHeadShot | eDamageFlags::IgnorePedFlags, // damageFlags
				0, // actionResultId
				0, // meleeId
				0, // nForcedReactionDefinitionId
				false, // hitEntityWeapon
				false, // hitWeaponAmmoAttachment
				false, // silenced
				false, // firstBullet
				nullptr // hitDirection
			);
		}
	}

	AbstractEntity AbstractEntity::getWeapon()
	{
		if (auto ped = getCPed())
		{
			if (ped->weapon_manager->equippedWeapon.object)
			{
				return AbstractEntity::get(ped->weapon_manager->equippedWeapon.object);
			}
		}
		return AbstractEntity::invalid();
	}

	v3 AbstractEntity::getWeaponMuzzlePos()
	{
		if (auto weapon = getWeapon(); weapon.isValid())
		{
			auto mi = static_cast<CWeaponModelInfo*>(weapon.getModelInfo());
			if (mi->GetModelType() == MI_TYPE_WEAPON)
			{
				return ENTITY::GET_ENTITY_BONE_POSTION(weapon, mi->m_nBoneIndices[/* WEAPON_MUZZLE */ 3]);
			}
		}
		return {};
	}

	bool AbstractEntity::isPassiveMode()
	{
		if (auto obj = getNetObject<CNetObjPlayer>())
		{
			return obj->m_bIsPassive;
		}
		return false;
	}

	bool AbstractEntity::isParachuting()
	{
		const auto state = PED::GET_PED_PARACHUTE_STATE(*this);
		return state == 1 || state == 2;
	}

	int AbstractEntity::getMaxPassengers()
	{
		return VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(operator Entity());
	}

	int AbstractEntity::getFreeSeat()
	{
		if (ensureHandle())
		{
			const int passengers = getMaxPassengers();
			for (int i = -1; i < passengers; i++)
			{
				if (VEHICLE::GET_PED_IN_VEHICLE_SEAT(handle, i, 0) == 0)
				{
					return i;
				}
			}
		}
		return -3;
	}

	void AbstractEntity::makePassengersLeave()
	{
		if (ensureHandle())
		{
			const int passengers = getMaxPassengers();
			for (int i = -1; i < passengers; i++)
			{
				const Ped ped = VEHICLE::GET_PED_IN_VEHICLE_SEAT(handle, i, 0);
				if (ped)
				{
					TASK::CLEAR_PED_TASKS_IMMEDIATELY(ped);
				}
			}
			if (*this == g_player_veh)
			{
				updatePlayerVehAndEnt();
			}
		}
	}

	AbstractEntity AbstractEntity::getPedInSeat(int seat)
	{
		if (!ensureHandle())
		{
			return AbstractEntity::invalid();
		}
		return AbstractEntity::get(VEHICLE::GET_PED_IN_VEHICLE_SEAT(handle, seat, 0));
	}

	AbstractEntity AbstractEntity::getDriver()
	{
		return getPedInSeat(-1);
	}

	bool AbstractEntity::isAPlayerInAnySeat()
	{
		if (ensureHandle())
		{
			for (int i = -1; i < getMaxPassengers(); i++)
			{
				if (getPedInSeat(i).isAPlayer())
				{
					return true;
				}
			}
		}
		
		return false;
	}

	bool AbstractEntity::isOwnerOfVehicleAndDriver()
	{
		if (!isOwner())
		{
			return false;
		}
		AbstractEntity driver = getDriver();
		return !driver.isValid() || !driver.isAPlayer() || driver == g_player_ped;
	}

	void AbstractEntity::setVehicleCanBeDamaged(const bool toggle)
	{
		if (ensureHandle())
		{
			ENTITY::SET_ENTITY_CAN_BE_DAMAGED(handle, toggle);
			VEHICLE::SET_VEHICLE_ENGINE_CAN_DEGRADE(handle, toggle);
			VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(handle, toggle);
			for (int i = 0; i < VEHICLE::GET_NUMBER_OF_VEHICLE_DOORS(handle); i++)
			{
				VEHICLE::SET_DOOR_ALLOWED_TO_BE_BROKEN_OFF(handle, i, toggle);
			}
		}
	}

	void AbstractEntity::repair()
	{
		if (ensureHandle())
		{
			const bool vtol_was_on = isVtolOn();

			// SET_VEHICLE_FIXED will call CVehicleDamage::FixVehicleDamage.
			// CVehicleDamage::FixVehicleDamage stores then restores our velocity, but R* zeros out the stored velocity when it's > 80 m/s.
			const auto ent_vel = ENTITY::GET_ENTITY_VELOCITY(handle);
			const auto ent_ang_vel = ENTITY::GET_ENTITY_ROTATION_VELOCITY(handle);

			VEHICLE::SET_VEHICLE_FIXED(handle);
			ENTITY::SET_ENTITY_VELOCITY(handle, ent_vel.x, ent_vel.y, ent_vel.z); // Restore for CVehicleDamage::FixVehicleDamage
			ENTITY::SET_ENTITY_ANGULAR_VELOCITY(handle, ent_ang_vel.x, ent_ang_vel.y, ent_ang_vel.z); // Restore for CVehicleDamage::FixVehicleDamage
			ENTITY::SET_ENTITY_HEALTH(handle, 1000, 0, 0);
			VEHICLE::SET_VEHICLE_ENGINE_HEALTH(handle, 1000.0f);
			VEHICLE::SET_VEHICLE_PETROL_TANK_HEALTH(handle, 1000.0f);
			VEHICLE::SET_VEHICLE_UNDRIVEABLE(handle, FALSE);
			if (getDriver().isValid())
			{
				VEHICLE::SET_VEHICLE_ENGINE_ON(handle, true, true, false);
			}
			ENTITY::FORCE_ENTITY_AI_AND_ANIMATION_UPDATE(handle);

			if (getModel().hasVtol())
			{
				// Can't do this in the same tick, so doing it in the next tick.
				FiberPool::queueJob([veh{ *this }, vtol_was_on]() mutable
				{
					if (!vtol_was_on
						&& VEHICLE::GET_VEHICLE_FLIGHT_NOZZLE_POSITION(veh) != 0.0f
						)
					{
						veh.toggleVtolOff();
					}
				});
			}

			// If the vehicle was exploded, it'll be on fire, which would cause the vehicle to just blow up again after being fixed.
			FIRE::STOP_ENTITY_FIRE(handle);
		}
		if (ensurePointer())
		{
			// Despite all the natives we call, the vehicle is still considered destroyed (e.g. after it was exploded),
			// which can be observed as IS_ENTITY_DEAD considers it still dead, and our ped will not attempt to mount it.
			static_cast<CVehicle*>(ptr)->m_nDEflags.nStatus = getDriver().isValid() ? STATUS_PLAYER : STATUS_PHYSICS;
		}
	}

	void AbstractEntity::wash()
	{
		if (ensureHandle())
		{
			VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(handle);
			VEHICLE::SET_VEHICLE_DIRT_LEVEL(handle, 0.0f);
			if (VEHICLE::GET_DOES_VEHICLE_HAVE_DAMAGE_DECALS(handle) || !GRAPHICS::DOES_VEHICLE_HAVE_CREW_EMBLEM(handle, 0))
			{
				GRAPHICS::REMOVE_DECALS_FROM_VEHICLE(handle);
			}
			for (int i = 0; i <= 7; i++)
			{
				VEHICLE::FIX_VEHICLE_WINDOW(handle, i);
			}
		}
	}

	int AbstractEntity::getUpgradeValue(int modType)
	{
		if (ensureHandle())
		{
			if (VehicleMods::isBooleanIncludeLights(modType))
			{
				if (VEHICLE::IS_TOGGLE_MOD_ON(handle, modType))
				{
					return 1;
				}
			}
			else if (modType == VehicleMods::livery && VEHICLE::GET_NUM_VEHICLE_MODS(handle, VehicleMods::livery) == 0)
			{
				return VEHICLE::GET_VEHICLE_LIVERY(handle) - 1;
			}
			else
			{
				auto value = VEHICLE::GET_VEHICLE_MOD(handle, modType);
				if (auto opt = getNextGenRemapping(modType); opt.has_value())
				{
					for (int i = 0; i != opt.value().size(); ++i)
					{
						if (opt.value().at(i) == value)
						{
							return i;
						}
					}
				}
				else
				{
					return value;
				}
			}
		}
		return -1;
	}

	int AbstractEntity::getUpgradeMaxValue(int modType)
	{
		if (VehicleMods::isBooleanIncludeLights(modType))
		{
			return 1;
		}
		if (ensureHandle())
		{
			if (auto opt = getNextGenRemapping(modType); opt.has_value())
			{
				return (int)(opt.value().size() - 1);
			}

			auto num_mods = VEHICLE::GET_NUM_VEHICLE_MODS(handle, modType);
			if (num_mods == 0 && modType == VehicleMods::livery)
			{
				return VEHICLE::GET_VEHICLE_LIVERY_COUNT(handle) - 2;
			}
			return num_mods - 1;
		}
		return -1;
	}

	bool AbstractEntity::isUpgradeApplicable(int modType)
	{
		return getUpgradeMaxValue(modType) != -1;
	}

	bool AbstractEntity::hasUpgradeMax(int modType)
	{
		return getUpgradeValue(modType) == getUpgradeMaxValue(modType);
	}

	void AbstractEntity::applyUpgrade(int modType, int value)
	{
		if (ensureHandle())
		{
			if (modType == VehicleMods::livery && VEHICLE::GET_NUM_VEHICLE_MODS(handle, modType) == 0)
			{
				VEHICLE::SET_VEHICLE_LIVERY(handle, value + 1);
			}
			else if (VehicleMods::isBooleanIncludeLights(modType))
			{
				VEHICLE::TOGGLE_VEHICLE_MOD(handle, modType, value != 0);
			}
			else
			{
				const bool custom_tires = (modType == VehicleMods::front_wheels || modType == VehicleMods::back_wheels)
					? VEHICLE::GET_VEHICLE_MOD_VARIATION(handle, VehicleMods::front_wheels) == 1
					: false;

				if (value != -1)
				{
					if (auto opt = getNextGenRemapping(modType); opt.has_value())
					{
						//if (value < opt.value().size())
						{
							value = opt.value().at(value);
						}
					}
				}

				VEHICLE::SET_VEHICLE_MOD(handle, modType, value, custom_tires);
			}
		}
	}

	void AbstractEntity::applyUpgradeMax(int modType)
	{
		applyUpgrade(modType, getUpgradeMaxValue(modType));
	}

	void AbstractEntity::fullyUpgrade()
	{
		if (ensureHandle())
		{
			VEHICLE::SET_VEHICLE_MOD_KIT(handle, 0);
			VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(handle, false);
			for (const auto& modType : VehicleMods::all)
			{
				applyUpgradeMax(modType);
			}
			applyNeonPerformanceUpgrade();
		}
	}

	bool AbstractEntity::applyPerformanceUpgrade()
	{
		bool changed = false;
		for (const auto& modType : VehicleMods::performance)
		{
			if (!hasUpgradeMax(modType))
			{
				applyUpgradeMax(modType);
				changed = true;
			}
		}
		changed |= applyNeonPerformanceUpgrade();
		return changed;
	}

	bool AbstractEntity::applyNeonPerformanceUpgrade()
	{
		bool changed = false;
		for (int neon_id = 0; neon_id != 4; ++neon_id)
		{
			if (!VEHICLE::GET_VEHICLE_NEON_ENABLED(*this, neon_id))
			{
				VEHICLE::SET_VEHICLE_NEON_ENABLED(*this, neon_id, TRUE);
				VEHICLE::SET_VEHICLE_NEON_COLOUR(*this, 0, 0, 0);
				changed = true;
			}
		}
		return changed;
	}

	bool AbstractEntity::applyPerformanceUpgradeWithSpoiler()
	{
		bool changed = applyPerformanceUpgrade();
		if (getUpgradeValue(VehicleMods::spoiler) == -1)
		{
			applyUpgrade(VehicleMods::spoiler, 0);
			changed = true;
		}
		return changed;
	}

	void AbstractEntity::downgrade()
	{
		VEHICLE::SET_VEHICLE_MOD_KIT(handle, 0);

		for (int i = 0; i < VehicleMods::_NUM_TYPES; i++)
		{
			if (VehicleMods::isBooleanIncludeLights(i))
			{
				VEHICLE::TOGGLE_VEHICLE_MOD(handle, i, false);
			}
			else
			{
				VEHICLE::REMOVE_VEHICLE_MOD(handle, i);
			}
		}
	}

	void AbstractEntity::startEngine()
	{
		if (ensureHandle())
		{
			VEHICLE::SET_VEHICLE_ENGINE_ON(handle, true, true, false);
			VEHICLE::SET_HELI_BLADES_FULL_SPEED(handle);
		}
	}

	bool AbstractEntity::isVtolOn()
	{
		return ensureHandle()
			&& VEHICLE::GET_VEHICLE_FLIGHT_NOZZLE_POSITION(handle) == 1.0f
			;
	}

	void AbstractEntity::toggleVtolOff()
	{
		if (ensureHandle())
		{
			VEHICLE::SET_VEHICLE_FLIGHT_NOZZLE_POSITION_IMMEDIATE(handle, 0.0f);
		}
	}

	void AbstractEntity::destroy()
	{
		makePassengersLeave();
		FiberPool::queueJob([veh{ *this }]() mutable
		{
			const auto deadline = get_current_time_millis() + 3000;
			while (!veh.isDead() && !IS_DEADLINE_REACHED(deadline))
			{
				auto pos = veh.getPos();
				FIRE::ADD_EXPLOSION(pos.x, pos.y, pos.z, EXP_TAG_STICKYBOMB, 1.0f, true, false, 0.0f, false);
				if (is_session_started())
				{
					CExplosionManager::CExplosionArgs args{ EXP_TAG_STICKYBOMB, pos };
					args.m_bMakeSound = false;
					args.m_bNoFx = true;
					args.m_sizeScale = 1.0f;
					args.m_fCamShake = 0.0f;
					args.m_bAttachedToVehicle = true;
					args.m_pAttachEntity = (CEntity*)veh.getPointer();
					pointers::CExplosionEvent_Trigger(&args, nullptr);
				}
				Script::current()->yield();
			}
		});
	}

	void AbstractEntity::call()
	{
		requestControl(ATSTRINGHASH("call"), [](AbstractEntity& veh)
		{
			const v3 pos = g_player_ped.getPos();
			PED::SET_PED_INTO_VEHICLE(g_player_ped, veh, -1);
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(veh, pos.x, pos.y, pos.z, false, false, false);
			ENTITY::SET_ENTITY_ROTATION(veh, 0, 0, CAM::GET_GAMEPLAY_CAM_ROT(0).z, 0, true);
		}, [](auto)
		{
			Util::toast(LOC("VEHCTRL"));
		});
	}

	int AbstractEntity::getHeadlightsVariation()
	{
		if (ensureHandle()
			&& VEHICLE::IS_TOGGLE_MOD_ON(handle, VehicleMods::xenon_lights)
			)
		{
			int colour = VEHICLE::GET_VEHICLE_XENON_LIGHT_COLOR_INDEX(handle);
			if (colour != 255)
			{
				return colour + 2;
			}
			return 1;
		}
		return 0;
	}

	void AbstractEntity::setHeadlightsVariation(int value)
	{
		if (ensureHandle())
		{
			if (value > 0)
			{
				VEHICLE::TOGGLE_VEHICLE_MOD(handle, VehicleMods::xenon_lights, true);
				if (value == 1)
				{
					VEHICLE::SET_VEHICLE_XENON_LIGHT_COLOR_INDEX(handle, 255);
				}
				else
				{
					VEHICLE::SET_VEHICLE_XENON_LIGHT_COLOR_INDEX(handle, value - 2);
				}
			}
			else
			{
				VEHICLE::TOGGLE_VEHICLE_MOD(handle, VehicleMods::xenon_lights, false);
			}
		}
	}

	int AbstractEntity::getPrimaryColour()
	{
		if (ensureHandle())
		{
			int primary;
			int secondary;
			VEHICLE::GET_VEHICLE_COLOURS(handle, &primary, &secondary);
			return primary;
		}
		return -1;
	}

	void AbstractEntity::setPrimaryColour(int value)
	{
		if (ensureHandle())
		{
			int primary;
			int secondary;
			VEHICLE::GET_VEHICLE_COLOURS(handle, &primary, &secondary);
			VEHICLE::SET_VEHICLE_COLOURS(handle, value, secondary);
		}
	}

	int AbstractEntity::getSecondaryColour()
	{
		if (ensureHandle())
		{
			int primary;
			int secondary;
			VEHICLE::GET_VEHICLE_COLOURS(handle, &primary, &secondary);
			return secondary;
		}
		return -1;
	}

	void AbstractEntity::setSecondaryColour(int value)
	{
		if (ensureHandle())
		{
			int primary;
			int secondary;
			VEHICLE::GET_VEHICLE_COLOURS(handle, &primary, &secondary);
			VEHICLE::SET_VEHICLE_COLOURS(handle, primary, value);
		}
	}

	bool AbstractEntity::isUserPersonalVehicle()
	{
		return ensureHandle() && DECORATOR::DECOR_GET_INT(handle, "Player_Vehicle") == NETWORK::NETWORK_HASH_FROM_PLAYER_HANDLE(g_player);
	}

	void AbstractEntity::slingshot()
	{
		slingshot(Direction::FORWARD);
	}

	void AbstractEntity::slingshot(Direction direction)
	{
		v3 force_vector = this->getPos();

		switch (direction)
		{
		case Direction::FORWARD:
			force_vector = ENTITY::GET_ENTITY_FORWARD_VECTOR(*this) * 2000.0f;
			break;

		case Direction::BACK:
			force_vector = ENTITY::GET_ENTITY_FORWARD_VECTOR(*this) * -2000.0f;
			break;

		case Direction::UP:
			force_vector.z *= 2000.0f;
			break;

		case Direction::DOWN:
			force_vector.z *= -2000.0f;
			break;

		case Direction::LEFT:
		{
			auto rot = this->getRot();
			rot.z += 90.0f;
			this->setRot(rot);
			force_vector = ENTITY::GET_ENTITY_FORWARD_VECTOR(*this) * 2000.0f;
			break;
		}

		case Direction::RIGHT:
			auto rot = this->getRot();
			rot.z -= 90.0f;
			this->setRot(rot);
			force_vector = ENTITY::GET_ENTITY_FORWARD_VECTOR(*this) * 2000.0f;
			break;
		}

		ENTITY::APPLY_FORCE_TO_ENTITY(*this, 1, force_vector.x, force_vector.y, force_vector.z, 0.0f, 0.0f, 0.0f, 1, false, false, true, false, false);
	}

	[[nodiscard]] std::optional<std::vector<uint8_t>> AbstractEntity::getNextGenRemapping(int modType)
	{
		switch (this->getModel())
		{
		case ATSTRINGHASH("sentinel"):
			switch (modType)
			{
			case VehicleMods::spoiler: return std::vector<uint8_t>{ 0, 1, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };
			case VehicleMods::livery: return std::vector<uint8_t>{ 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
			case VehicleMods::suspension: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
			case VehicleMods::speakers: return std::vector<uint8_t>{};
			case VehicleMods::brakes: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::engine: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::transmission: return std::vector<uint8_t>{ 0, 1, 2 };
			}
			break;

		case ATSTRINGHASH("banshee"):
			switch (modType)
			{
			case VehicleMods::spoiler: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::livery: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
			case VehicleMods::suspension: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
			case VehicleMods::speakers: return std::vector<uint8_t>{};
			case VehicleMods::brakes: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::engine: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::transmission: return std::vector<uint8_t>{ 0, 1, 2 };
			}
			break;

		case ATSTRINGHASH("hakuchou2"):
			switch (modType)
			{
			case VehicleMods::livery: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
			case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
			case VehicleMods::speakers: return std::vector<uint8_t>{};
			case VehicleMods::brakes: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::engine: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::transmission: return std::vector<uint8_t>{ 0, 1, 2 };
			}
			break;

		case ATSTRINGHASH("deveste"):
			switch (modType)
			{
			case VehicleMods::livery: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			case VehicleMods::spoiler: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8 };
			case VehicleMods::suspension: return std::vector<uint8_t>{ 0 };
			case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
			case VehicleMods::speakers: return std::vector<uint8_t>{};
			case VehicleMods::brakes: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::engine: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::transmission: return std::vector<uint8_t>{ 0, 1, 2 };
			}
			break;

		case ATSTRINGHASH("brioso"):
			switch (modType)
			{
			case VehicleMods::livery: return std::vector<uint8_t>{ 0, 1 };
			case VehicleMods::spoiler: return std::vector<uint8_t>{ 0, 1 };
			case VehicleMods::suspension: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
			case VehicleMods::speakers: return std::vector<uint8_t>{};
			case VehicleMods::brakes: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::engine: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::transmission: return std::vector<uint8_t>{ 0, 1, 2 };
			}
			break;

		case ATSTRINGHASH("feltzer3"):
			switch (modType)
			{
			case VehicleMods::livery: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
			case VehicleMods::spoiler: return std::vector<uint8_t>{ 0, 1 };
			case VehicleMods::suspension: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::hood: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7 };
			case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
			case VehicleMods::speakers: return std::vector<uint8_t>{};
			case VehicleMods::brakes: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::engine: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::transmission: return std::vector<uint8_t>{ 0, 1, 2 };
			}
			break;

		case ATSTRINGHASH("vigero2"):
			switch (modType)
			{
				case VehicleMods::livery: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12 };
				case VehicleMods::spoiler: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
				case VehicleMods::sideskirt: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
				case VehicleMods::suspension: return std::vector<uint8_t>{ 0, 1, 2, 3 };
				case VehicleMods::front_bumper: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
				case VehicleMods::rear_bumper: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
				case VehicleMods::hood: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25 };
				case VehicleMods::roof: return std::vector<uint8_t>{ 0, 1 };
				case VehicleMods::fender: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
				case VehicleMods::right_fender: return std::vector<uint8_t>{ 0, 1, 2, 3, 4 };
				case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
				case VehicleMods::speakers: return std::vector<uint8_t>{};
				case VehicleMods::brakes: return std::vector<uint8_t>{ 0, 1, 2 };
				case VehicleMods::engine: return std::vector<uint8_t>{ 0, 1, 2, 3 };
				case VehicleMods::transmission: return std::vector<uint8_t>{ 0, 1, 2 };
			}
			break;

		case ATSTRINGHASH("entity3"):
			switch (modType)
			{
			case VehicleMods::livery: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			case VehicleMods::spoiler: return std::vector<uint8_t>{ 0, 1, 4, 5, 8, 9, 12, 13, 16, 17, 20, 21, 23, 24, 27, 28 };
			case VehicleMods::hood: return std::vector<uint8_t>{ 0, 2, 3, 5, 6, 8, 9, 11, 12, 14, 15, 17, 18 };
			case VehicleMods::sideskirt: return std::vector<uint8_t>{ 0, 3, 4, 7, 8 };
			case VehicleMods::suspension: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::front_bumper: return std::vector<uint8_t>{ 0, 4, 7, 8, 11, 12 };
			case VehicleMods::rear_bumper: return std::vector<uint8_t>{ 0 };
			case VehicleMods::roof: return std::vector<uint8_t>{ 0, 1, 3, 4 };
			case VehicleMods::fender: return std::vector<uint8_t>{ 0, 1 };
			case VehicleMods::right_fender: return std::vector<uint8_t>{};
			case VehicleMods::grille: return std::vector<uint8_t>{ 0, 1 };
			case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
			case VehicleMods::speakers: return std::vector<uint8_t>{};
			case VehicleMods::brakes: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::engine: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::transmission: return std::vector<uint8_t>{ 0, 1, 2 };
			}
			break;

		case ATSTRINGHASH("issi8"):
			switch (modType)
			{
			case VehicleMods::spoiler: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
			case VehicleMods::hood: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8 };
			case VehicleMods::livery: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			case VehicleMods::suspension: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
			case VehicleMods::speakers: return std::vector<uint8_t>{};
			case VehicleMods::brakes: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::engine: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::transmission: return std::vector<uint8_t>{ 0, 1, 2 };
			}
			break;

		case ATSTRINGHASH("turismo2"):
			switch (modType)
			{
			case VehicleMods::livery: return std::vector<uint8_t>{};
			case VehicleMods::spoiler: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
			case VehicleMods::suspension: return std::vector<uint8_t>{};
			case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
			case VehicleMods::speakers: return std::vector<uint8_t>{};
			case VehicleMods::brakes: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::engine: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::transmission: return std::vector<uint8_t>{ 0, 1, 2 };
			}
			break;

		case ATSTRINGHASH("monstrociti"):
			switch (modType)
			{
			case VehicleMods::livery: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			case VehicleMods::suspension: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::hood: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
			case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
			case VehicleMods::speakers: return std::vector<uint8_t>{};
			case VehicleMods::brakes: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::engine: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::transmission: return std::vector<uint8_t>{ 0, 1, 2 };
			}
			break;

		case ATSTRINGHASH("buffalo5"):
			switch (modType)
			{
			case VehicleMods::livery: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			case VehicleMods::spoiler: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7 };
			case VehicleMods::suspension: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::hood: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
			case VehicleMods::speakers: return std::vector<uint8_t>{};
			case VehicleMods::brakes: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::engine: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::transmission: return std::vector<uint8_t>{ 0, 1, 2 };
			}
			break;

		case ATSTRINGHASH("coureur"):
			switch (modType)
			{
			case VehicleMods::livery: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			case VehicleMods::spoiler: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22 };
			case VehicleMods::sideskirt: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 };
			case VehicleMods::suspension: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::front_bumper: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8 };
			case VehicleMods::rear_bumper: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::hood: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22 };
			case VehicleMods::roof: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 };
			case VehicleMods::fender: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7 };
			case VehicleMods::grille: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
			case VehicleMods::speakers: return std::vector<uint8_t>{};
			case VehicleMods::brakes: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::engine: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::transmission: return std::vector<uint8_t>{ 0, 1, 2 };
			}
			break;
			
		case ATSTRINGHASH("stingertt"):
			switch (modType)
			{
			case VehicleMods::spoiler: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8 };
			case VehicleMods::sideskirt: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 };
			case VehicleMods::suspension: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::front_bumper: return std::vector<uint8_t>{ 0, 1 };
			case VehicleMods::rear_bumper: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7 };
			case VehicleMods::hood: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
			case VehicleMods::fender: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
			case VehicleMods::grille: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6 };
			case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
			case VehicleMods::speakers: return std::vector<uint8_t>{};
			case VehicleMods::brakes: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::engine: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::transmission: return std::vector<uint8_t>{ 0, 1, 2 };
			}
			break;

		case ATSTRINGHASH("eurosX32"):
			switch (modType)
			{
			case VehicleMods::livery: return std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
			case VehicleMods::sideskirt: return std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
			case VehicleMods::rear_bumper: return std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
			case VehicleMods::hood: return std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
			case VehicleMods::roof: return std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8};
			case VehicleMods::right_fender: return std::vector<uint8_t>{0, 1, 2, 3, 4, 5};
			case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
			case VehicleMods::speakers: return std::vector<uint8_t>{};
			case VehicleMods::tank: return std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
			}
			break;

		case ATSTRINGHASH("niobe"):
			switch (modType)
			{
			case VehicleMods::spoiler: return std::vector<uint8_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8 };
			case VehicleMods::livery: return std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
			case VehicleMods::hood: return std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7};
			case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
			case VehicleMods::speakers: return std::vector<uint8_t>{};
			case VehicleMods::brakes: return std::vector<uint8_t>{ 0, 1, 2 };
			case VehicleMods::engine: return std::vector<uint8_t>{ 0, 1, 2, 3 };
			case VehicleMods::transmission: return std::vector<uint8_t>{ 0, 1, 2 };
			}
			break;

#if false
		case ATSTRINGHASH("deez"):
			switch (modType)
			{
				//case VehicleMods::livery: return std::vector<uint8_t>{ 0, 1 };
				//case VehicleMods::spoiler: return std::vector<uint8_t>{ 0, 1 };
				//case VehicleMods::suspension: return std::vector<uint8_t>{ 0, 1, 2 };
				//case VehicleMods::shifter_leavers: return std::vector<uint8_t>{};
				//case VehicleMods::speakers: return std::vector<uint8_t>{};
				//case VehicleMods::brakes: return std::vector<uint8_t>{ 0, 1, 2 };
				//case VehicleMods::engine: return std::vector<uint8_t>{ 0, 1, 2, 3 };
				//case VehicleMods::transmission: return std::vector<uint8_t>{ 0, 1, 2 };
			}
			break;
#endif
		}

		return std::nullopt;
	}

	void AbstractEntity::makeIndestructible()
	{
		setVehicleCanBeDamaged(false);

		auto model = getModel();
		if (VEHICLE::IS_THIS_MODEL_A_PLANE(model))
		{
			VEHICLE::SET_PLANE_PROPELLER_HEALTH(*this, 1000.0f);
		}
		else if (VEHICLE::IS_THIS_MODEL_A_HELI(model))
		{
			auto* const rotorcraft = (CRotaryWingAircraft*)getPointer();
			rotorcraft->main_rotor_health = 1000.0f;
			rotorcraft->tail_rotor_health = 1000.0f;
			rotorcraft->tail_boom_health = 1000.0f;
		}
	}

	void AbstractEntity::detachWheel(int wheelIndex)
	{
		if (auto cveh = getCVehicle())
		{
			if (wheelIndex < cveh->num_wheels)
			{
				pointers::CVehicleDamage_BreakOffWheel(&cveh->m_vehicleDamage, wheelIndex, 1.0f, 0.0f, 0.0f, false, false);
			}
		}
	}

	void AbstractEntity::setPlateText(const std::string& text)
	{
		setPlateText(text.c_str());
	}

	void AbstractEntity::setPlateText(const char* text)
	{
		if (ensureHandle())
		{
			// The game tries to correct plate based on PV data, so we should override this as well.
			if (isUserPersonalVehicle())
			{
				strcpy(ScriptGlobal(GLOBAL_PVS).at(ScriptGlobal(GLOBAL_MOST_RECENT_PV_INDEX).get<int>(), GLOBAL_PVS_ELMSIZE).at(GLOBAL_PVS_PLATE_TEXT).as<char*>(), text);
			}

			VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(handle, text);
		}
	}

	void AbstractEntity::setPlateBasedOnSeed()
	{
		if (ensureHandleAndPointer())
		{
			auto text = Util::getRandomLicensePlate(ptr->m_randomSeed);
			setPlateText(text.c_str());
		}
	}

	void AbstractEntity::randomiseSeedAndUpdatePlate()
	{
		if (ensureHandleAndPointer())
		{
			uint16_t seed = soup::rand.t<uint16_t>(0, -1);
			ptr->m_randomSeed = seed;
			auto text = Util::getRandomLicensePlate(seed);
			setPlateText(text.c_str());
		}
	}

	bool AbstractEntity::isOnRoad()
	{
		const auto coords = getPos();
		return PATHFIND::IS_POINT_ON_ROAD(coords.x, coords.y, coords.z, *this);
	}

	AbstractPlayer AbstractEntity::getPersonalVehicleOwner()
	{
		const auto hash = DECORATOR::DECOR_GET_INT(*this, "Player_Vehicle");

		if (hash != 0)
		{
			for (const auto& p : AbstractPlayer::listAll())
			{
				if (hash == NETWORK::NETWORK_HASH_FROM_PLAYER_HANDLE(p))
				{
					return p;
				}
			}
		}

		return AbstractPlayer::invalid();
	}

	[[nodiscard]] static float GET_BOMB_OFFSET(hash_t model)
	{
		float fBombOffset = 0.0f;
		switch (model)
		{
		case ATSTRINGHASH("cuban800"):
			fBombOffset = 0.5f;
			break;

		case ATSTRINGHASH("mogul"):
			fBombOffset = 0.45f;
			break;

		case ATSTRINGHASH("rogue"):
			fBombOffset = 0.46f;
			break;

		case ATSTRINGHASH("starling"):
			fBombOffset = 0.55f;
			break;

		case ATSTRINGHASH("seabreeze"):
			fBombOffset = 0.5f;
			break;

		case ATSTRINGHASH("tula"):
			fBombOffset = 0.6f;
			break;

		case ATSTRINGHASH("bombushka"):
			fBombOffset = 0.43f;
			break;

		case ATSTRINGHASH("hunter"):
			fBombOffset = 0.5f;
			break;

		case ATSTRINGHASH("avenger"):
		case ATSTRINGHASH("avenger3"):
			fBombOffset = 0.36f;
			break;

		case ATSTRINGHASH("akula"):
			fBombOffset = 0.4f;
			break;

		case ATSTRINGHASH("volatol"):
			fBombOffset = 0.54f;
			break;

		case ATSTRINGHASH("strikeforce"):
			fBombOffset = 0.7f;
			break;

		case ATSTRINGHASH("alkonost"):
			fBombOffset = 0.4f;
			break;

		case ATSTRINGHASH("streamer216"):
			fBombOffset = 0.4f;
			break;
		}
		return fBombOffset;
	}

	[[nodiscard]] static Vector3 GET_INTERP_POINT_VECTOR_FOR_VEHICLE_WEAPON(const Vector3& a, const Vector3& b, float t)
	{
		return { soup::lerp(a.x, b.x, t), soup::lerp(a.y, b.y, t), soup::lerp(a.z, b.z, t) };
	}

	std::pair<Vector3, Vector3> AbstractEntity::getBombBayPos()
	{
		const auto model_hash = getModelHash();
		const auto bomb_offset = GET_BOMB_OFFSET(model_hash);
		if (bomb_offset != 0.0f && ensureHandle())
		{
			Vector3 modelMin, modelMax;
			MISC::GET_MODEL_DIMENSIONS(model_hash, &modelMin, &modelMax);

			Vector3 tempFrontBottomLeft, tempFrontBottomRight, tempBackBottomLeft, tempBackBottomRight;

			tempFrontBottomLeft.x = modelMin.x;
			tempFrontBottomLeft.y = modelMax.y;
			tempFrontBottomLeft.z = modelMin.z;

			tempFrontBottomRight.x = modelMax.x;
			tempFrontBottomRight.y = modelMax.y;
			tempFrontBottomRight.z = modelMin.z;

			tempBackBottomLeft.x = modelMin.x;
			tempBackBottomLeft.y = modelMin.y;
			tempBackBottomLeft.z = modelMin.z;

			tempBackBottomRight.x = modelMax.x;
			tempBackBottomRight.y = modelMin.y;
			tempBackBottomRight.z = modelMin.z;

			auto FrontBottomLeft = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(handle, tempFrontBottomLeft.x, tempFrontBottomLeft.y, tempFrontBottomLeft.z);
			auto FrontBottomRight = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(handle, tempFrontBottomRight.x, tempFrontBottomRight.y, tempFrontBottomRight.z);
			auto BackBottomLeft = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(handle, tempBackBottomLeft.x, tempBackBottomLeft.y, tempBackBottomLeft.z);
			auto BackBottomRight = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(handle, tempBackBottomRight.x, tempBackBottomRight.y, tempBackBottomRight.z);

			auto vB1 = GET_INTERP_POINT_VECTOR_FOR_VEHICLE_WEAPON(FrontBottomLeft, FrontBottomRight, 0.5f);
			auto vB2 = GET_INTERP_POINT_VECTOR_FOR_VEHICLE_WEAPON(BackBottomLeft, BackBottomRight, 0.5f);

			vB1.z += 0.4f;
			vB2.z += 0.4f;

			auto vB3 = GET_INTERP_POINT_VECTOR_FOR_VEHICLE_WEAPON(vB1, vB2, bomb_offset);

			vB1.z -= 0.2f;
			vB2.z -= 0.2f;

			auto vB4 = GET_INTERP_POINT_VECTOR_FOR_VEHICLE_WEAPON(vB1, vB2, bomb_offset - 0.0001f);

			return { vB3, vB4 };
		}
		return { {}, {} };
	}
}
