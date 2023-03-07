#ifndef ANTICHEAT_UTILS_HPP
#define ANTICHEAT_UTILS_HPP

#include "module.hpp"
#include <stdio.h>
#include <stdint.h>

int StrCaseCmp(const char* string1, const char* string2);
char* StrCaseStr(char* string, const char* subString);
const char* StrCaseStr(const char* string, const char* subString);
char* StrDup(const char* string);
char* LeftStrip(char* string);
char* RightStrip(char* string);

// Alternative to fgets().
bool ReadLineFromFile(FILE* file, char* buffer, size_t bufferSize);
void ParseConfigFileLine(char* line, char*& settingName, char*& settingValue);

bool IsLoggingEnabled();

bool IsIPV4Valid(const char* address);
bool IsPlayerIdValid(int32_t playerId);
bool IsPlayerUniqueIdValid(const char* uniqueId);

Module* ParseModuleListLine(char* line);

#endif
