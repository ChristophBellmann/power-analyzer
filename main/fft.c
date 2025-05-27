#include "fft.h"
#include <math.h>
#include <stdlib.h>

static size_t s_len = 0;

void fft_init(size_t len)
{
    s_len = len;
}

void fft_execute(const float *time_data, float *out_real, float *out_imag)
{
    if (s_len == 0) return;
    for (size_t k = 0; k < s_len; ++k) {
        float sum_real = 0.0f;
        float sum_imag = 0.0f;
        for (size_t n = 0; n < s_len; ++n) {
            float angle = -2.0f * (float)M_PI * (float)k * (float)n / (float)s_len;
            sum_real += time_data[n] * cosf(angle);
            sum_imag += time_data[n] * sinf(angle);
        }
        out_real[k] = sum_real;
        out_imag[k] = sum_imag;
    }
}

void fft_get_magnitudes(const float *real, const float *imag, float *mag, size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        mag[i] = sqrtf(real[i] * real[i] + imag[i] * imag[i]);
    }
}
