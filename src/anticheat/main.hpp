#ifndef ANTICHEAT_MAIN_HPP
#define ANTICHEAT_MAIN_HPP

#define STRICT_ANTICHEAT 0

// <Windows.h>'s MAX_PATH -- there is no such header on Linux so we have to
// define it ourselves.
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef UINT24_MAX
#define UINT24_MAX 0xFFFFFF
#endif

/* Plugin settings */
#define PLUGIN_NAME    "Anti-Cheat"
#define PLUGIN_VERSION 0x100 // v1.0.0

/* VC:MP limits */
#define MAX_PLAYERS       100
#define PLAYER_UID_LENGTH 40 // Applies for both UID version 1 and 2.

/* Buffer limits */
#define MAX_PLAYER_NAME 24
#define MAX_PLAYER_IPV4 32
#define MAX_PLAYER_UID  64 // Applies for both UID version 1 and 2.

/* Module list limits */
#define MODULE_LIST_ITEMS_PER_REALLOC 16

/* AntiCheat settings */
#define ANTICHEAT_CONFIG_FILE_NAME "anticheat.cfg"
#define ANTICHEAT_CONFIG_FILE_PATH ANTICHEAT_CONFIG_FILE_NAME

#endif
