#ifndef ANTICHEAT_MODULE_HPP
#define ANTICHEAT_MODULE_HPP

#include "main.hpp"
#include <stddef.h>

class Module
{
private:
	unsigned long size;
	char path[MAX_PATH];
	char* name;

public:
	Module(unsigned long moduleSize, const char* modulePath);

	bool BelongsToWindows() const;

	unsigned long GetSize() const { return size; }
	const char* GetPath() const { return path; }
	const char* GetName() const { return name; }

	bool operator==(const Module& module) const;
	bool operator!=(const Module& module) const;
};

class ModuleList
{
private:
	Module** moduleList;
	size_t moduleCount;

	void PushModule(Module* module);

public:
	ModuleList(char* list);
	~ModuleList();

	Module* FindModule(const char* moduleName) const;
	Module* FindForbiddenModule(const char* gtaPath,
		const char* clientPath) const;
	Module* operator==(const ModuleList& list) const;
};

#endif
