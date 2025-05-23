#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#include "logging.h"

static log_level_t currentLogLevel = LOG_DEBUG;
static FILE *outputFile;

static const char *levelNames[5] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
};

static const char *colourOpeners[] = {
    "\033[33m",
    "",
    "\033[93m",
    "\033[31m",
    "\033[38;2;255;50;50m"
};


void log_init(FILE *out) {
    if (!outputFile)
        outputFile = out;
}

void log_setLevel(log_level_t level) {
    currentLogLevel = level;
}

void log_log(
    log_level_t level,
    const char *file,
    int line,
    const char *func,
    const char *fmt,
    ...
) {
    if (level < currentLogLevel)
        return;

    const time_t currentTime = time(NULL);

    struct tm currentTimeData;
    localtime_r(&currentTime, &currentTimeData);

    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &currentTimeData);

    fprintf(outputFile, "%s", colourOpeners[level]);
    fprintf(outputFile, "%s %-5s %s:%d:%s(): ", timeStr, levelNames[level], file, line, func);

    va_list args;
    va_start(args, fmt);
    vfprintf(outputFile, fmt, args);
    va_end(args);

    fprintf(outputFile, "\033[0m\n");
    fflush(outputFile);
}