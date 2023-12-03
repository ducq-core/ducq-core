commands.lua_command = {
	doc = "test listing lua command",
	exec = function(ducq, msg)
		ducq.send("test lua")
	end
} 
