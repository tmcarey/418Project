#include "Logger.h"


const char * LogPrefix = "(Oxide):";
const char* DebugPrefix = "(Oxide) DEBUG:";
const char* ErrorPrefix = "(Oxide) ERR:";
const char* WarnPrefix = "(Oxide) WARN:";

void LogDebug(const char* log, ...) {
#if LOG_LEVEL > 2
	va_list vl;
	std::string full(DebugPrefix);
	const char* plog = full.append(log).append("\n").c_str();
	va_start(vl, plog);
	vprintf(plog, vl);
	va_end(vl);
#else
	return;
#endif
}

void Log(const char* log, ...) {
#if LOG_LEVEL > 0
	va_list vl;
	std::string full(LogPrefix);
	const char* plog = full.append(log).append("\n").c_str();
	va_start(vl, plog);
	vprintf(plog, vl);
	va_end(vl);

#else
	return;
#endif
}

void LogNB(const char* log, ...) {
#if LOG_LEVEL > 0
	va_list vl;
	std::string full(LogPrefix);
	const char* plog = full.append(log).c_str();
	va_start(vl, plog);
	vprintf(plog, vl);
	va_end(vl);

#else
	return;
#endif
}


void LogError(const char* log, ...) {
#if LOG_LEVEL > 0
	va_list vl;
	std::string full(ErrorPrefix);
	const char* plog = full.append(log).append("\n").c_str();
	va_start(vl, plog);
	vprintf(plog, vl);
	va_end(vl);
#else
	return;
#endif
}

void LogWarn(const char* log, ...) {
#if LOG_LEVEL > 1
	va_list vl;
	std::string full(WarnPrefix);
	const char* plog = full.append(log).append("\n").c_str();
	va_start(vl, plog);
	vprintf(plog, vl);
	va_end(vl);
#else
	return;
#endif
}