/* Copyright JS Foundation and other contributors, http://js.foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "jerryscript-port.h"
#include "jerryscript-port-default.h"
#include "jerryscript-debugger.h"

/**
 * Actual log level
 */
static jerry_log_level_t jerry_port_default_log_level = JERRY_LOG_LEVEL_TRACE;

/**
 * Get the log level
 *
 * @return current log level
 */
jerry_log_level_t jerry_port_default_get_log_level (void)
{
    return jerry_port_default_log_level;
} /* jerry_port_default_get_log_level */

/**
 * Set the log level
 */
void jerry_port_default_set_log_level (jerry_log_level_t level) /**< log level */
{
    jerry_port_default_log_level = level;
} /* jerry_port_default_set_log_level */


#define LOG_BUFFER_SIZE   256

/**
 * Default implementation of jerry_port_log. Prints log message to the standard
 * error with 'vfprintf' if message log level is less than or equal to the
 * current log level.
 *
 * If debugger support is enabled, printing happens first to an in-memory buffer,
 * which is then sent both to the standard error and to the debugger client.
 */
void jerry_port_log (jerry_log_level_t level, /**< message log level */
                const char *format, /**< format string */
                ...)  /**< parameters */
{
    if (level <= jerry_port_default_log_level)
    {
        va_list args;
        char buffer[LOG_BUFFER_SIZE] = {0};

        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer) - 1, format, args);
        va_end(args);

        printf("%s", buffer);
    }
} /* jerry_port_log */

/**
 * Default implementation of jerry_port_print_char. Uses 'putchar' to
 * print a single character to standard output.
 */
void jerry_port_print_char (char c) /**< the character to print */
{
    printf("%c", c);
} /* jerry_port_print_char */
