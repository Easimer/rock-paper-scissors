#pragma once
#include "common.h"
#include <stddef.h>

enum waveform_t {
	wlinear,
	wsine, wsquare, wsawtooth
};

struct waveform_entry {
	waveform_t type;
	float freq, amplitude, length;
};

API void generate_wave(const waveform_entry* entries, size_t count, u8* buf, size_t buf_siz, size_t sampling_rate);
