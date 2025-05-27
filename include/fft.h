#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void  fft_init(size_t len);
void  fft_execute(const float *time_data, float *out_real, float *out_imag);
void  fft_get_magnitudes(const float *real, const float *imag, float *mag, size_t len);

#ifdef __cplusplus
}
#endif
