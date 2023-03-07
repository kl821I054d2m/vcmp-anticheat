// https://docs.vc-mp.org/plugins/functions.html#vcmpplugininit

#ifndef INCLUDE_VCMP_H
#define INCLUDE_VCMP_H

#include <vcmpsdk.h>

// Are we on Windows?
#ifdef OS_WINDOWS
#define VCMP_EXPORT __declspec(dllexport)
// Linux assumed.
#else
#define VCMP_EXPORT
#endif

// Compatible with C++!
#ifdef __cplusplus
extern "C"
{
#endif
	VCMP_EXPORT uint32_t VcmpPluginInit(PluginFuncs*, PluginCallbacks*,
		PluginInfo*);
#ifdef __cplusplus
}
#endif

#endif
