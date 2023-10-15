# ducq
![ducq](/img/ducq.png)
> a minimalist, extensible message queue framework for your lan

**Project state: proof of concept (usable)**

This an exploration project to learn linux api, socket programming, Lua integration with C and design patterns.

The core concept is to abstract the communication layer so that clients can send and receive payload in a message queue fashion while being on different protocol (raw tcp, websocket, http, your-own-protocol, etc).

Beside a few core commands that should not be changed (like publish and subscribe), the commands that clients send to the server can be added and modified on the fly by dropping Lua script in a specified folder. Thus, even if the main purpose of the server is to dispatch payload to subscribers, it  can also act like a mock REST, a chat, or whatever you implement.

##### Dependencies
* cmocka
* Lua

##### Examples 
* [server implementation example](https://github.com/ducq-core/server)
* [command line client](https://github.com/ducq-core/ducq_cli)
* [esp32 publisher example](https://github.com/ducq-core/esp32_publisher_example)
* [websocket client example](https://github.com/ducq-core/dashbord) in [Preact.js](https://preactjs.com/)

## Todo toward beta version

### interface implementations
- recv: make *count = 0 on error
- ws client-side connection
- http
- https, wss, mqtt (maybe)

### protocol
- formalize difference between "control command" and "application command"
- CONT and END: message with unknown length sent in several packet
- PING : keep alive mecanism

### clients
- review cli program
- make writing clients in Lua trivial

### reactor
- better test with `inotify`
- take out internal interator and review commands that use it
- extract log object
- review interface
- end-to-end tests (with Lua client ?)

### command
- Lua command should comply with the (name, doc, exec) structure
- dispatcher should return them with `list_commands`

### features
- channels ("route with behavior")
    - should be extensible like commands
    - in Lua context or reactor aggregate ?
    - should be CRUDable
    - implementation ideas:
        - control access channel
        - send last message received on subscription channel

### C++
- fix C++ warnings (esp32 compilation)
