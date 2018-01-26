#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include "fmod_errors.h"

int main() {
	FMOD_RESULT result;
	FMOD::System *system = NULL;

	// Create the main system object.
	result = FMOD::System_Create(&system);
	if (result != FMOD_OK){
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}

	// Initialize FMOD.
	result = system->init(512, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK){
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}

	FMOD::Sound *sound;
	result = system->createSound("Audio/brentasaurus.wav", FMOD_CREATESTREAM, NULL, &sound);
	if (result != FMOD_OK) {
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		//exit(-1);
	}

	FMOD::DSP* dsp;
	result = system->createDSPByType(FMOD_DSP_TYPE::FMOD_DSP_TYPE_FFT, &dsp);
	if (result != FMOD_OK) {
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		//exit(-1);
	}

	FMOD::Channel *channel;
	result = system->playSound(sound, NULL, false, &channel);
	if (result != FMOD_OK) {
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		//exit(-1);
	}

	result = channel->addDSP(0, dsp);
	if (result != FMOD_OK) {
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		//exit(-1);
	}

	FMOD_DSP_PARAMETER_FFT *fft;
	dsp->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void **)&fft, 0, 0, 0);

	int sampleRate;
	system->getSoftwareFormat(&sampleRate, NULL, NULL);
	printf("sample rate: %d\n", sampleRate);
	while (true) {
		scanf("%*c");
		system->update();
		printf("numChannels: %d, length: %d\n", fft->numchannels, fft->length);
		for (int ch = 0; ch < fft->numchannels; ch++) {
			for (int bin = 0; bin < fft->length/2; bin++)
				printf("%f (%f), ", fft->spectrum[ch][bin], sampleRate/((float)fft->length)*(bin+1));
			printf("\n");
		}
	}
	system->release();

	return 0;
}