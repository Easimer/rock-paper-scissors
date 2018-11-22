#include "synth.h"
#include <math.h>

internal void generate_sinewave(u8* buf, float from, const waveform_entry* entry, size_t sampling_rate) {
	float amplitude = entry->amplitude;
	if(amplitude > 1) {
		amplitude = 1;
	}
	if(amplitude < 0) {
		amplitude = 0;
	}

	size_t samples = entry->length * sampling_rate;
	size_t start = from * sampling_rate;
	size_t end = start + samples;

	for(size_t i = start; i < end; i++) {
		float x = i / (float)sampling_rate;
		buf[i] = ((sin(entry->freq * x) + 1) / 2 * (255 * amplitude));
	}
}

API void generate_wave(const waveform_entry* entries, size_t count, u8* buf, size_t buf_siz, size_t sampling_rate) {
	float t_cur = 0;
	for(size_t i = 0; i < count; i++) {
		switch(entries[i].type) {
			case wsine:
				generate_sinewave(buf, t_cur, entries + i, sampling_rate);
				break;
		}
		t_cur += entries[i].length;
	}	
}
