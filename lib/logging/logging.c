#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "logging.h"

#define MAX_OUTPUT_FILES 8

static log_level_t currentLogLevel = LEVEL_DEBUG;
static FILE *outputFile[MAX_OUTPUT_FILES];
static size_t currentFileN = 0;

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
    if (!outputFile[0]) {
        outputFile[0] = out;
        currentFileN++;
    }
}

void log_addOutput(FILE *out) {
    if (currentFileN < MAX_OUTPUT_FILES) {
        outputFile[currentFileN++] = out;
    }
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

#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&currentTimeData, &currentTime);
#else
    localtime_r(&currentTime, &currentTimeData);
#endif

    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &currentTimeData);

    for (int i = 0; i < currentFileN; i++) {
        fprintf(outputFile[i], " %s %s %-5s %s:%d:%s(): ", colourOpeners[level], timeStr, levelNames[level], file, line, func);

        va_list args;
        va_start(args, fmt);
        vfprintf(outputFile[i], fmt, args);
        va_end(args);

        fprintf(outputFile[i], "\033[0m\n");
        fflush(outputFile[i]);
    }
}
