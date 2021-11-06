#ifndef LOGGER_H
#  define LOGGER_H

/**
 * Detailed debug information.
 */
#  define LEVEL_DEBUG 100
/**
 * Interesting events. Examples: User logs in, SQL logs.
 */
#  define LEVEL_INFO 200
/**
 * Normal but significant events.
 */
#  define LEVEL_NOTICE 250
/**
 * Exceptional occurrences that are not errors.
 * Examples: Use of deprecated APIs, poor use of an API, undesirable things that are not necessarily wrong.
 */
#  define LEVEL_WARNING 300
/**
 * Runtime errors that do not require immediate action but should typically be logged and monitored.
 */
#  define LEVEL_ERROR 400
/**
 * Critical conditions.
 * Example: Application component unavailable, unexpected exception.
 */
#  define LEVEL_CRITICAL 500
/**
 * Action must be taken immediately.
 * Example: Entire website down, database unavailable, etc. This should trigger the SMS alerts and wake you up.
 */
#  define LEVEL_ALERT 550
/**
 * Emergency: system is unusable.
 */
#  define LEVEL_EMERGENCY 600

#  ifndef LOG_LEVEL
#    define LOG_LEVEL LEVEL_NOTICE
#  endif

#  if LOG_LEVEL <= LEVEL_DEBUG
#    define LOG_DEBUG(msg, ...) fprintf(stderr, "[DEBG] (%s:%d) " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#  else
#    define LOG_DEBUG(msg, ...)
#  endif

#  if LOG_LEVEL <= LEVEL_INFO
#    define LOG_INFO(msg, ...) fprintf(stderr, "[INFO] (%s:%d) " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#  else
#    define LOG_INFO(msg, ...)
#  endif

#  if LOG_LEVEL <= LEVEL_NOTICE
#    define LOG_NOTICE(msg, ...) fprintf(stderr, "[NOTI] (%s:%d) " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#  else
#    define LOG_NOTICE(msg, ...)
#  endif

#  if LOG_LEVEL <= LEVEL_WARNING
#    define LOG_WARNING(msg, ...) fprintf(stderr, "[WARN] (%s:%d) " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#  else
#    define LOG_WARNING(msg, ...)
#  endif

#  if LOG_LEVEL <= LEVEL_ERROR
#    define LOG_ERROR(msg, ...) fprintf(stderr, "[ERRO] (%s:%d) " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#  else
#    define LOG_ERROR(msg, ...)
#  endif

#  if LOG_LEVEL <= LEVEL_CRITICAL
#    define LOG_CRITICAL(msg, ...) fprintf(stderr, "[CRIT] (%s:%d) " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#  else
#    define LOG_CRITICAL(msg, ...)
#  endif

#  if LOG_LEVEL <= LEVEL_ALERT
#    define LOG_ALERT(msg, ...) fprintf(stderr, "[ALER] (%s:%d) " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#  else
#    define LOG_ALERT(msg, ...)
#  endif

#  if LOG_LEVEL <= LEVEL_EMERGENCY
#    define LOG_EMERGENCY(msg, ...) fprintf(stderr, "[EMER] (%s:%d) " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#  else
#    define LOG_EMERGENCY(msg, ...)
#  endif

#endif /* LOGGER_H */
