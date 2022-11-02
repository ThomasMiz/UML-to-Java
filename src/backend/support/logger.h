#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdio.h>

// Descomentar para loguear en modo DEBUG (con m�s detalle).
#define DEBUG true

void Log(FILE * const stream, const char * prefix, const char * const format, const char * suffix, va_list arguments);

void LogDebug(const char * const format, ...);

void LogError(const char * const format, ...);

void LogErrorRaw(const char * const format, ...);

void LogInfo(const char * const format, ...);

#endif
