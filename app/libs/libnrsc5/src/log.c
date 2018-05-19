/*
 * Copyright (c) 2017 rxi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "log.h"

static const char *level_names[] = {
  "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

#ifdef USE_COLOR
static const char *level_colors[] = {
  "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};
#endif

void log_jni(const char *msg) {
  /*jstring jmsg = cb_data.env->NewStringUTF(msg);
  cb_data.env->CallVoidMethodA(cb_data.tuner, cb_data.log_id, (jvalue*)&jmsg);*/
}

void log_log(int level, const char *file, int line, const char *fmt, ...) {
  char msg_out[1024];
  char log_msg[1024];

  va_list args;
  va_start(args, fmt);
  vsnprintf(log_msg, sizeof(log_msg), fmt, args);
  va_end(args);

  /* awesie: cut off everything besides file name */
  size_t slash;
  while (slash = strcspn(file, "/\\"), file[slash] != 0) {
    file += slash + 1;
  }

  /* Get current time */
  time_t t = time(NULL);
  struct tm *lt = localtime(&t);

    char buf[16];
    buf[strftime(buf, sizeof(buf), "%H:%M:%S", lt)] = '\0';
#ifdef USE_COLOR
    snprintf(
      msg_out, sizeof(msg_out), "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m %s\n",
      buf, level_colors[level], level_names[level], file, line, log_msg);
#else
    snprintf(msg_out, sizeof(msg_out), "%s %-5s %s:%d: %s\n", buf, level_names[level], file, line, log_msg);
#endif

    log_jni(msg_out);
}
