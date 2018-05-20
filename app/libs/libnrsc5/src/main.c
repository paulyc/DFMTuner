/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <complex.h>
#include <getopt.h>
#include <limits.h>
#include <math.h>
#include <string.h>

#ifdef USE_THREADS
#include <pthread.h>
#endif

#include <rtl-sdr.h>
#include <jni.h>

#include "defines.h"
#include "input.h"

#define RADIO_BUFCNT (8)
#define RADIO_BUFFER (512 * 1024)

static int gain_list[128];
static int gain_index, gain_count;

// signal and noise are squared magnitudes
static int snr_callback(void *arg, float snr)
{
    static int best_gain;
    static float best_snr;
    int result = 0;
    rtlsdr_dev_t *dev = arg;

    if (gain_count == 0)
        return result;

    // choose the best gain level
    if (snr >= best_snr)
    {
        best_gain = gain_index;
        best_snr = snr;
    }

    log_info("Gain: %.1f dB, CNR: %.1f dB", gain_list[gain_index] / 10.0, 20 * log10f(snr));

    if (gain_index + 1 >= gain_count || snr < best_snr * 0.5)
    {
        log_debug("Best gain: %d", gain_list[best_gain]);
        gain_index = best_gain;
        gain_count = 0;
    }
    else
    {
        gain_index++;
        // continue searching
        result = 1;
    }

    rtlsdr_set_tuner_gain(dev, gain_list[gain_index]);
    rtlsdr_reset_buffer(dev);
    return result;
}

unsigned int parse_freq(char *s)
{
    double d = strtod(s, NULL);
    if (d < 10000) d *= 1e6;
    return (unsigned int) d;
}

static void help(const char *progname)
{
    fprintf(stderr, "Usage: %s [-v] [-q] [-l log-level] [-d device-index] [-g gain] [-p ppm-error] [-r samples-input] [-w samples-output] [-o audio-output -f adts|hdc|wav] [--dump-aas-files directory] frequency program\n", progname);
}

static input_t input;
static output_t output;

void log_jni(const char *msg) {
  fprintf(stderr, "%s\n", msg);
}

void configure(unsigned int frequency, unsigned int program, void(*audio_cb)(void*,uint32_t)) {
  int err, opt, gain = INT_MIN, ppm_error = 0;

  output.audio_cb = audio_cb;

  /*rtlsdr_open()*/

  //output_init_wav(&output, audio_name);

  output_init(&output);

  input_init(&input, &output, frequency, program);

  uint8_t *buf = malloc(128 * SNR_FFT_COUNT);
  rtlsdr_dev_t *dev;

  int count = rtlsdr_get_device_count();
  if (count == 0) {
    FATAL_EXIT("No devices found!");
  }

  for (int i = 0; i < count; ++i) {
    log_info("[%d] %s", i, rtlsdr_get_device_name(i));
  }

    int indx = 0;//rtlsdr_get_index_by_serial("RTL2838UHIDIR");
    switch (indx) {
      case -1:
        FATAL_EXIT("rtlsdr_get_index_by_serial name was null");
        break;
      case -2:
        FATAL_EXIT("rtlsdr_get_index_by_serial found no devices");
        break;
      case -3:
        FATAL_EXIT("rtlsdr_get_index_by_serial devices were found, but none with matching name");
        break;
      default:
        log_info("rtlsdr_get_index_by_serial returned index %d", indx);
        break;
    }
    err = rtlsdr_open(&dev, indx);
    if (err) FATAL_EXIT("rtlsdr_open error: %d", err);
    err = rtlsdr_set_sample_rate(dev, 1488375);
    if (err) FATAL_EXIT("rtlsdr_set_sample_rate error: %d", err);
    err = rtlsdr_set_tuner_gain_mode(dev, 1);
    if (err) FATAL_EXIT("rtlsdr_set_tuner_gain_mode error: %d", err);
    err = rtlsdr_set_freq_correction(dev, ppm_error);
    if (err && err != -2) FATAL_EXIT("rtlsdr_set_freq_correction error: %d", err);
    err = rtlsdr_set_center_freq(dev, frequency);
    if (err) FATAL_EXIT("rtlsdr_set_center_freq error: %d", err);

    if (gain == INT_MIN)
    {
      gain_count = rtlsdr_get_tuner_gains(dev, gain_list);
      if (gain_count > 0)
      {
        input_set_snr_callback(&input, snr_callback, dev);
        err = rtlsdr_set_tuner_gain(dev, gain_list[0]);
        if (err) FATAL_EXIT("rtlsdr_set_tuner_gain error: %d", err);
      }
    }
    else
    {
      err = rtlsdr_set_tuner_gain(dev, gain);
      if (err) FATAL_EXIT("rtlsdr_set_tuner_gain error: %d", err);
    }

    err = rtlsdr_reset_buffer(dev);
    if (err) FATAL_EXIT("rtlsdr_reset_buffer error: %d", err);

    // special loop for modifying gain (we can't use async transfers)
    while (gain_count)
    {
      // use a smaller buffer during auto gain
      int len = 128 * SNR_FFT_COUNT;

      err = rtlsdr_read_sync(dev, buf, len, &len);
      if (err) FATAL_EXIT("rtlsdr_read_sync error: %d", err);

      input_cb(buf, len, &input);
    }
    free(buf);

    err = rtlsdr_read_async(dev, input_cb, &input, RADIO_BUFCNT, RADIO_BUFFER);
    if (err) FATAL_EXIT("rtlsdr_read_async error: %d", err);
    err = rtlsdr_close(dev);
    if (err) FATAL_EXIT("rtlsdr error: %d", err);
}
