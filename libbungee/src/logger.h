/*
logger.h: bungee logging framework

This file is part of Bungee.

Copyright 2012 Red Hat, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _LOGGER_H
#define _LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

/* GLib log levels that are considered fatal by default. */
#define G_LOG_FATAL_MASK (G_LOG_FLAG_RECURSION | G_LOG_LEVEL_ERROR)
/* BNG_LOG is only accessible to libbungee code. G_LOG_DOMAIN is declared in libbungee/src/Makefile.am */
#ifdef G_LOG_DOMAIN
#define BNG_LOG(level, format, ...) g_log (G_LOG_DOMAIN, level, "[%s:%d] "format, __FILE__, __LINE__, ##__VA_ARGS__)
#define BNG_INFO(format, ...) g_log (G_LOG_DOMAIN, level, format, ##__VA_ARGS__)
#else
#define BNG_LOG(level, format, ...) g_log (NULL, level, "[%s:%d] "format, __FILE__, __LINE__, ##__VA_ARGS__)
#define BNG_INFO(format, ...) g_log (NULL, level, format, ##__VA_ARGS__)
#endif

#define BNG_WARNING(format, ...) BNG_LOG (G_LOG_LEVEL_WARNING, format, ##__VA_ARGS__)
#define BNG_CRITICAL(format, ...) BNG_LOG (G_LOG_LEVEL_CRITICAL, format, ##__VA_ARGS__)
#define BNG_ERROR(format, ...) BNG_LOG (G_LOG_LEVEL_ERROR, format, ##__VA_ARGS__)
#define BNG_DEBUG(format, ...) BNG_LOG (G_LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* _LOGGER_H */
