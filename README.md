# ducq
> a minimalist, extensible message queue framework for your lan

**Project state: proof of concept**

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

### test/review
- better `reactor` unit tests for `round_table()`
- review if `listen()` has read error
  - this function shoud differentiate between message protocol and implementation specific protocol
  - reconnection logic responsibility

### features
- lua: client-side
- ws:  client-side connection
- http: client and server side
