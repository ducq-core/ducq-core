commands.echo = {
	exec = function(ducq, msg)
		return ducq:send( tostring(msg) )
	end
}
