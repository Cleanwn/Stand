util.require_natives()
menu.action(menu.my_root(), "Crash", {""}, "", function()
	util.request_model("cs_lestercrest")
	util.request_model("cs_josh")
	local pid = players.user()
	local PlayerPed = PLAYER.GET_PLAYER_PED_SCRIPT_INDEX(pid)
	local PlayerPos = ENTITY.GET_ENTITY_COORDS(PlayerPed, true)
	PlayerPos.x = PlayerPos.x + 3      
	for i = 1, 10 do
		local Ped = entities.create_ped(26, util.joaat("cs_lestercrest"), PlayerPos, ENTITY.GET_ENTITY_HEADING(PlayerPed))  
		local time = util.current_time_millis() + 1500
		while time > util.current_time_millis() do
			PED.SET_PED_RANDOM_COMPONENT_VARIATION(Ped,0)
			NETWORK.NETWORK_REQUEST_CONTROL_OF_ENTITY(Ped)
			PED.SET_PED_COMPONENT_VARIATION(Ped, 3, 3, math.random(0, 3), 0)
			PED.SET_PED_COMPONENT_VARIATION(Ped, 8, 2, math.random(0, 3), 0)
			PED.SET_PED_COMPONENT_VARIATION(Ped, 9, 2, math.random(0, 3), -1)
			PED.SET_PED_COMPONENT_VARIATION(Ped, 12, 2, math.random(0, 3), -1)
			PED.SET_PED_COMPONENT_VARIATION(Ped, 13, 2, math.random(0, 3), -1)
			util.yield(50)
		end
		TASK.TASK_START_SCENARIO_IN_PLACE(Ped, "WORLD_HUMAN_YOGA", 0, false)
		util.yield(100)
	end
	util.yield(1000)

	for i = 1, 10 do
		local Ped = entities.create_ped(26, util.joaat("cs_josh"), PlayerPos, ENTITY.GET_ENTITY_HEADING(PlayerPed))
		local time = util.current_time_millis() + 1500
		while time > util.current_time_millis() do
			PED.SET_PED_RANDOM_COMPONENT_VARIATION(Ped,0)
			NETWORK.NETWORK_REQUEST_CONTROL_OF_ENTITY(Ped)
			PED.SET_PED_COMPONENT_VARIATION(Ped, 3, 0, math.random(0, 3), 0)
			PED.SET_PED_COMPONENT_VARIATION(Ped, 8, 0, math.random(0, 3), 0)
			PED.SET_PED_COMPONENT_VARIATION(Ped, 9, 0, math.random(0, 3), -1)
			PED.SET_PED_COMPONENT_VARIATION(Ped, 12, 0, math.random(0, 3), -1)
			PED.SET_PED_COMPONENT_VARIATION(Ped, 13, 0, math.random(0, 3), -1)
			util.yield(50)
		end
		TASK.TASK_START_SCENARIO_IN_PLACE(Ped, "WORLD_HUMAN_YOGA", 0, false)
		util.yield(100)
	end
end)
