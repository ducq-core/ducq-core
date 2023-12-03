commands.ReplyHello = {
	exec = function (ducq, msg)
		ducq:send("hello, world")
		return 0
	end
}
