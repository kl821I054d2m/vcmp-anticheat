# anticheat04rel32/64
set(ANTICHEAT_PLUGIN_OUTPUT_NAME anticheat${PLUGIN_OUTPUT_NAME_SUFFIX})

add_library(
	${ANTICHEAT_PLUGIN_OUTPUT_NAME} SHARED
	main.cpp
	callbacks.cpp
	events.cpp
	utils.cpp
	core.cpp
	console.cpp
	acsettings.cpp
	player.cpp
	playermanager.cpp
	module.cpp
)
