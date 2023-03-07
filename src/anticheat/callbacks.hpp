// This header file is (and should be) included ONLY by callbacks.cpp and
// main.cpp thus it is safe not to add an include guard here.

#include <vcmp.h>

namespace Callback
{
	void OnServerShutdown(void);
	void OnServerFrame(float);

	void OnPlayerConnect(int32_t);
	void OnPlayerDisconnect(int32_t, vcmpDisconnectReason);
	void OnPlayerModuleList(int32_t, const char*);
}
