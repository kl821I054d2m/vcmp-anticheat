#include "module.hpp"
#include "utils.hpp"
#include <string.h>

Module::Module(unsigned long moduleSize, const char* modulePath)
{
	size = moduleSize;
	strcpy(path, modulePath);

	name = strrchr(path, '\\');
	if (name)
	{
		*name++ = '\0';
	}
	else
	{
		name = path;
	}
}

bool Module::BelongsToWindows() const
{
	// Is module within System32 folder?
	return !!StrCaseStr(path, "C:\\Windows\\System32");
}

bool Module::operator==(const Module& module) const
{
	return size == module.size && !StrCaseCmp(path, module.path) &&
		!StrCaseCmp(name, module.name);
}

bool Module::operator!=(const Module& module) const
{
	return !(*this == module);
}

void ModuleList::PushModule(Module* module)
{
	// Do we not have enough room to store a new module?
	if (!(moduleCount % MODULE_LIST_ITEMS_PER_REALLOC))
	{
		// Create a new, bigger list.
		Module** newModuleList = new Module*[moduleCount +
			MODULE_LIST_ITEMS_PER_REALLOC];
		// Copy old list contents to the new one.
		memcpy(newModuleList, moduleList, moduleCount * sizeof(Module*));

		// Old list is no longer useful so free it.
		delete[] moduleList;
		moduleList = newModuleList;
	}

	// Push the module.
	moduleList[moduleCount++] = module;
}

ModuleList::ModuleList(char* list)
{
	moduleList = nullptr;
	moduleCount = 0;

	// Attempt to retrieve Net and Build versions line (first one).
	char* line = strtok(list, "\n");
	if (!line)
	{
		return;
	}

	// We won't be needing it whatsoever so just skip it and parse the rest of
	// the list.
	Module* newModule;
	while (line = strtok(nullptr, "\n"))
	{
		// Attempt to parse current line.
		newModule = ParseModuleListLine(line);
		if (!newModule)
		{
			continue;
		}

		// Discard the module if it belongs to Windows. Can this be easily
		// spoofed, if at all?
		if (newModule->BelongsToWindows())
		{
			delete newModule;
			continue;
		}

		PushModule(newModule);
	}
}

ModuleList::~ModuleList()
{
	for (size_t i = 0; i < moduleCount; ++i)
	{
		delete moduleList[i];
	}
	delete[] moduleList;
}

Module* ModuleList::FindModule(const char* moduleName) const
{
	Module* module;
	for (size_t i = 0; i < moduleCount; ++i)
	{
		module = moduleList[i];
		// Windows file names are case-insensitive by standard; so, perform a
		// case-insensitive comparison.
		if (!StrCaseCmp(moduleName, module->GetName()))
		{
			return module;
		}
	}

	// No module with such name was found.
	return nullptr;
}

Module* ModuleList::FindForbiddenModule(const char* gtaPath,
	const char* clientPath) const
{
	// The Miles Sound System directory path.
	char mssPath[MAX_PATH + 4];
	sprintf(mssPath, "%s\\mss", gtaPath);

	Module* module;
	for (size_t i = 0; i < moduleCount; ++i)
	{
		module = moduleList[i];
		// Is current module in GTA's directory?
		if (!StrCaseCmp(module->GetPath(), gtaPath))
		{
			// Whitelist these modules.
			if (!StrCaseCmp(module->GetName(), "gta-vc.exe") ||
				!StrCaseCmp(module->GetName(), "Mss32.dll"))
			{
				continue;
			}
		}
		// Is current module in the MSS directory?
		else if (!StrCaseCmp(module->GetPath(), mssPath))
		{
			// Whitelist these modules.
			if (!StrCaseCmp(module->GetName(), "Mp3dec.asi") ||
				!StrCaseCmp(module->GetName(), "Mssa3d.m3d") ||
				!StrCaseCmp(module->GetName(), "Mssa3d2.m3d") ||
				!StrCaseCmp(module->GetName(), "Mssds3dh.m3d") ||
				!StrCaseCmp(module->GetName(), "Mssds3ds.m3d") ||
				!StrCaseCmp(module->GetName(), "Msseax.m3d") ||
				!StrCaseCmp(module->GetName(), "msseax3.m3d") ||
				!StrCaseCmp(module->GetName(), "Mssfast.m3d") ||
				!StrCaseCmp(module->GetName(), "Mssrsx.m3d") ||
				!StrCaseCmp(module->GetName(), "Reverb3.flt"))
			{
				continue;
			}
		}
		// Is current module in VC:MP installation directory?
		else if (!StrCaseCmp(module->GetPath(), clientPath))
		{
			// Whitelist these modules.
			if (!StrCaseCmp(module->GetName(), "libpng16.dll") ||
				!StrCaseCmp(module->GetName(), "vcmp-game.dll") ||
				!StrCaseCmp(module->GetName(), "vcmp-steam.dll"))
			{
				continue;
			}
		}

		// We have just encountered a blacklisted module.
		return module;
	}

	// All clean!
	return nullptr;
}

Module* ModuleList::operator==(const ModuleList& list) const
{
	// Is our list empty?
	if (!moduleCount)
	{
		// Is target list also empty?
		if (!list.moduleCount)
		{
			// Both lists are empty. We can't therefore perform any comparison.
			return nullptr;
		}

		// Our list is empty but target one is not so just return target one's
		// last item.
		return list.moduleList[list.moduleCount - 1];
	}

	// Is target list empty?
	if (!list.moduleCount)
	{
		// Target list is empty but ours is not so just return our last item.
		return moduleList[moduleCount - 1];
	}

	Module** smallestModuleList;
	size_t smallestModuleListIndex;

	Module** greatestModuleList;
	size_t greatestModuleListIndex;

	// Is target list equal or greater than our list?
	if (list.moduleCount >= moduleCount)
	{
		// Our list is the smallest one.
		smallestModuleList = moduleList;
		smallestModuleListIndex = moduleCount - 1;

		// Target list is the greatest one.
		greatestModuleList = list.moduleList;
		greatestModuleListIndex = list.moduleCount - 1;
	}
	// Our list is greater than that of target's.
	else
	{
		// Target list is the smallest one.
		smallestModuleList = list.moduleList;
		smallestModuleListIndex = list.moduleCount - 1;

		// Our list is the greatest one.
		greatestModuleList = moduleList;
		greatestModuleListIndex = moduleCount - 1;
	}

	Module* leftModule;
	Module* rightModule;
	while (true)
	{
		leftModule = smallestModuleList[smallestModuleListIndex];
		rightModule = greatestModuleList[greatestModuleListIndex];

		// Do current modules differ from each other?
		if (*rightModule != *leftModule)
		{
			return rightModule;
		}

		// Have we reached the beginning of the smallest module list?
		if (!smallestModuleListIndex)
		{
			// Have we reached the beginning of the greatest module list as
			// well, meaning both lists have the same size?
			if (!greatestModuleListIndex)
			{
				// All items from both lists have been compared against each
				// other and no discrepancy was found whatsoever.
				return nullptr;
			}

			// Greatest module list still has items left to be compared against
			// but since both lists don't have the same size we can't keep up -
			// if we did we would overflow. So, just return whatever module we
			// are stopping at.
			return rightModule;
		}

		// Iterate backwards.
		--smallestModuleListIndex;
		--greatestModuleListIndex;
	}
}
