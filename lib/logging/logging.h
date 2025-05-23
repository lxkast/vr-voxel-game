#pragma once

#include <stdio.h>
#include <stdlib.h>


typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} log_level_t;

/*
Initialises the logger, with an output stream/buffer.
Examples would be stdout, stderr, etc.
*/
void log_init(FILE *out);


/*
Sets the log level (The minimum level a log needs to be to be output)
*/
void log_setLevel(log_level_t level);

/*
The actual logging function
*/
void log_log(
    log_level_t level,
    const char *file,
    int line,
    const char *func,
    const char *fmt,
    ...
);


/*
Beautiful macro definitons.
*/
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_info(...) log_log(LOG_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_warn(...) log_log(LOG_WARN, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_fatal(...) \
    do { log_log(LOG_FATAL, __FILE__, __LINE__, __func__, __VA_ARGS__); exit() } while(0)
