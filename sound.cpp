#include "common.h"
#include "sound.h"
#include <stdio.h>
#include <AL/al.h>
#include <AL/alc.h>
#include "synth.h"

#include <math.h>

internal ALCdevice* gpDevice = nullptr;
internal ALCcontext* gpContext = nullptr;
internal ALuint gaiBuffers[sfx_max];
internal ALuint giSource = 0;

internal void display_error(ALenum err, const char* function, int line) {
	fprintf(stderr, "sound: backend error in %s:%d: ", function, line);
	switch(err) {
		case AL_NO_ERROR:
			fprintf(stderr, "success");
			break;
		case AL_INVALID_NAME:
			fprintf(stderr, "invalid name");
			break;
		case AL_INVALID_ENUM:
			fprintf(stderr, "invalid enum");
			break;
		case AL_INVALID_VALUE:
			fprintf(stderr, "invalid value");
			break;
		case AL_INVALID_OPERATION:
			fprintf(stderr, "invalid operation");
			break;
		case AL_OUT_OF_MEMORY:
			fprintf(stderr, "out of memory");
			break;
	}
	fprintf(stderr, "\n");
}

#define DISPLAY_ERROR(error) display_error(error, __FUNCTION__, __LINE__)

internal bool generate_victory_sound() {
	constexpr size_t sampling_rate = 44100;
	constexpr size_t samples = (size_t)(0.65 * sampling_rate);
	u8 audio[samples];
	
	waveform_entry dat[2];
	constexpr float volume = 0.3;
	dat[0] = {wsine, 8000, volume, 0.25};
	dat[1] = {wsine, 10000, volume, 0.40};

	generate_wave(dat, 2, audio, samples, sampling_rate);
	alBufferData(gaiBuffers[sfx_victory], AL_FORMAT_MONO8, audio, samples * sizeof(u8), sampling_rate);

	ALenum error = alGetError();
	if(error != AL_NO_ERROR) {
		DISPLAY_ERROR(error);
		return false;
	}
	return true;
}

internal bool generate_failure_sound() {
	constexpr size_t sampling_rate = 44100;
	constexpr size_t samples = (size_t)(1.6 * sampling_rate);
	u8 audio[samples];
	
	waveform_entry dat[5];
	constexpr float volume = 0.3;
	dat[0] = {wsine, 6000, volume, 0.45};
	dat[1] = {wsine, 6000, 0, 0.05};
	dat[2] = {wsine, 5000, volume, 0.45};
	dat[3] = {wsine, 5000, 0, 0.05};
	dat[4] = {wsine, 4000, volume, 0.60};

	generate_wave(dat, 5, audio, samples, sampling_rate);
	alBufferData(gaiBuffers[sfx_failure], AL_FORMAT_MONO8, audio, samples * sizeof(u8), sampling_rate);

	ALenum error = alGetError();
	if(error != AL_NO_ERROR) {
		DISPLAY_ERROR(error);
		return false;
	}
	return true;
}

internal bool generate_shake_sound() {
	constexpr size_t sampling_rate = 44100;
	constexpr size_t samples = (size_t)(0.30 * sampling_rate);
	u8 pcm[samples];

	waveform_entry dat[2];
	constexpr float volume = 0.6;
	dat[0] = {wsine, 800, volume, 0.20};
	dat[1] = {wsine, 0, 0, 0.10};

	generate_wave(dat, 2, pcm, samples, sampling_rate);
	alBufferData(gaiBuffers[sfx_shake], AL_FORMAT_MONO8, pcm, samples, sampling_rate);

	return true;
}

internal bool generate_tie_sound() {
	constexpr size_t sampling_rate = 44100;
	constexpr size_t samples = (size_t)(0.60 * sampling_rate);
	u8 pcm[samples];

	waveform_entry dat[3];
	constexpr float volume = 0.6;
	dat[0] = {wsine, 800, volume, 0.20};
	dat[1] = {wsine, 1, 0, 0.20};
	dat[2] = {wsine, 800, volume, 0.20};

	generate_wave(dat, 3, pcm, samples, sampling_rate);
	alBufferData(gaiBuffers[sfx_tie], AL_FORMAT_MONO8, pcm, samples, sampling_rate);

	return true;
}

bool sfx_init() {
	ALenum error;
	alGetError();
	gpDevice = alcOpenDevice(nullptr);
	if(!gpDevice) {
		fprintf(stderr, "sound: failed to open an audio device!\n");
		error = alGetError();
		DISPLAY_ERROR(error);
		return false;
	}
	gpContext = alcCreateContext(gpDevice, nullptr);
	if(!gpContext) {
		fprintf(stderr, "sound: failed to create an audio context!\n");
		error = alGetError();
		DISPLAY_ERROR(error);
		sfx_shutdown();
		return false;
	}
	if(!alcMakeContextCurrent(gpContext)) {
		error = alGetError();
		DISPLAY_ERROR(error);
		fprintf(stderr, "sound: failed to activate the audio context!\n");
		sfx_shutdown();
		return false;
	}

	// Load SFX files
	alGenBuffers(sfx_max, gaiBuffers);
	error = alGetError();
	if(error != AL_NO_ERROR) {
		fprintf(stderr, "sound: failed to generate the audio buffers!\n");
		DISPLAY_ERROR(error);
		sfx_shutdown();
		return false;
	}

	if(!generate_victory_sound()) {
		fprintf(stderr, "sound: failed to generate the victory sound!\n");
		sfx_shutdown();
		return false;
	}
	if(!generate_failure_sound()) {
		fprintf(stderr, "sound: failed to generate the failure sound!\n");
		sfx_shutdown();
		return false;
	}
	if(!generate_shake_sound()) {
		fprintf(stderr, "sound: failed to generate the shake sound!\n");
		sfx_shutdown();
		return false;
	}
	if(!generate_tie_sound()) {
		fprintf(stderr, "sound: failed to generate the tie sound!\n");
		sfx_shutdown();
		return false;
	}

	alGenSources(1, &giSource);
	error = alGetError();
	if(error != AL_NO_ERROR) {
		fprintf(stderr, "sound: failed to create snd source\n");
		DISPLAY_ERROR(error);
		sfx_shutdown();
		return false;
	}

	fprintf(stderr, "sound: initialized!\n");

	return true;
}

bool sfx_shutdown() {
	fprintf(stderr, "sound: SHUTTING DOWN!\n");
	alDeleteBuffers(sfx_max, gaiBuffers);
	if(gpContext) {
		alcDestroyContext(gpContext);
	}
	if(gpDevice) {
		if(!alcCloseDevice(gpDevice)) {
			fprintf(stderr, "sound: failed to close the audio device!!!\n");
			return false;
		}
	}
	return true;
}

bool sfx_play(sfx_t s) {
	ALenum error;
	if(s == sfx_invalid || s >= sfx_max) {
		return false;
	}
	alcMakeContextCurrent(gpContext);
	error = alGetError();
	if(error != AL_NO_ERROR) {
		DISPLAY_ERROR(error);
		return false;
	}

	alSourceStop(giSource);
	if(error != AL_NO_ERROR) {
		DISPLAY_ERROR(error);
		return false;
	}
	alSourcei(giSource, AL_BUFFER, 0);
	if(error != AL_NO_ERROR) {
		DISPLAY_ERROR(error);
		return false;
	}
	alSourcei(giSource, AL_BUFFER, gaiBuffers[s]);
	error = alGetError();
	if(error != AL_NO_ERROR) {
		DISPLAY_ERROR(error);
		return false;
	}
	alSourcePlay(giSource);
	error = alGetError();
	if(error != AL_NO_ERROR) {
		DISPLAY_ERROR(error);
		return false;
	}
	return true;
}
