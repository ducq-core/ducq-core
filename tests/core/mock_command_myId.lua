commands.MyId = {
	exec = function(ducq, msg)
		local id = ducq:id()
		ducq:send(id)
		return 0
	end
}
