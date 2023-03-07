# VC:MP Anti-Cheat Plugin
## Features
- Allows server developers to specify what version of VC:MP players must run in
order to play on their server.
- Strictly forbid modules that do not come with vanilla Vice City.
- When a module list change is detected during gameplay (e.g.: on DLL injection
with a third-party software like Cheat Engine) player will be immediately
kicked.
- It does **not** rely on module extensions. For example, a hacker would rename
a DLL to whatever extension they want and it would still be loadable by an
injector.
- ~~Player IP addresses and UIDs (version 1 and 2) are validated on join and
any subsequent change will be handled with an immediate kick.~~ This feature is
currently disabled as I believe there is no way to change IPs and UIDs while on
gameplay. Please do let me know if I am wrong though!
## Installation steps
1. Click [here](https://github.com/sfwidde/vcmp-anticheat/releases), choose a
plugin based on your needs, then download it.
2. Head to your server's directory, then:
	- Paste the plugin you just downloaded to your `plugins` directory.
	- Load the plugin by appending its name to the `plugins` field in
	`server.cfg`.
	- Create a new file named `anticheat.cfg`, in which we will store
	customizable settings supported by the plugin. File _must_ be present -
	plugin will otherwise refuse to load.
## `anticheat.cfg` configuration file
You can customize plugin's behaviour by messing with the following settings:
- `message_colour`
	- **Description:** Hexadecimal RRGGBB colour used when sending a message to
	the chatbox i.e.: on player kick.
	- **Optional?**
	- [x] Yes: defaults to `0B5FA5`.
	- [ ] No.
- `check_interval`
	- **Description:** Milliseconds interval between each player check. For
	example: if set to 2500, and last check was made just now for a certain
	player, server will now have to wait 2.5 seconds before issuing another
	check on the same player again and determine if a module list change was
	made in the meantime. Its value must be between 1000 and 600000.
	- **Optional?**
	- [x] Yes: defaults to 5000.
	- [ ] No.
- `max_check_refusals`
	- **Description:** Maximum amount of check refusals the server can take
	from a player before it decides to kick them out for not providing their
	module list. The counter will likely to increase only if their game is
	paused. The time it will take for a player to get kicked will highly depend
	on the setting `check_interval` (exactly
	`check_interval * max_check_refusals / 1000` seconds). Its value must be
	between 0 and 1000; setting this to 0 disables this feature completely:
	useful for when your script already handles player kick on pause. It is
	highly recommended that this setting remains enabled otherwise.
	- **Optional?**
	- [x] Yes: defaults to 100.
	- [ ] No.
- `expected_client_size`
	- **Description:** Hexadecimal value representing the size a player's
	client must have in order to play on the server. Its value must be at
	least 1. To find out what size to expect, it is as easy as setting this
	setting's value to any value you want, join your server and, after getting
	kicked, check the console for a fully-detailed reason of the kick which
	includes your client's size as well.
	- **Optional?**
	- [ ] Yes.
	- [x] No.

A fully-customized `anticheat.cfg` would look like this:
```
message_colour       0B5FA5
check_interval       5000
max_check_refusals   100
expected_client_size 09ABCDEF
```