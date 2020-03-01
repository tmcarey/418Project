#pragma once

#ifndef LOGGER
#include <stdio.h>
#include <cstdarg>
#include <string>
#define LOG_LEVEL 3
#define LOGGER 1

void Log(const char* log, ...);

void LogDebug(const char* log, ...);

void LogError(const char* log, ...);


void LogWarn(const char* log, ...);



#endif
