#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <cmath>

#pragma comment(lib, "winmm.lib")

#define SAMPLE_RATE 44100
#define PI 3.141

#define C4  261
#define D4  294
#define E4  329
#define F4  349
#define G4  392
#define A4  440
#define B4  494

#define C5  523
#define D5  587
#define E5  659
#define F5  698
#define G5  784
#define G5S 830
#define A5  880
#define B5  988
#define C6  1047
#define REST 0

// generate a sine wave tone
void generateTone(short* buffer, int frequency, int samples, double volume) {
    double amplitude = volume * 32767.0;
    for (int i = 0; i < samples; i++) {
        if (frequency == REST) {
            buffer[i] = 0;
        } else {
            buffer[i] = (short)(amplitude * sin(2.0 * PI * frequency * i / SAMPLE_RATE));
        }
    }
}

void playTone(int frequency, int durationMs, double volume, int deviceId) {
    int samples = (SAMPLE_RATE * durationMs) / 1000;
    short* buffer = new short[samples];

    generateTone(buffer, frequency, samples, volume);

    // set up WAVEFORMATEX structure
    WAVEFORMATEX wfx;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = SAMPLE_RATE;
    wfx.nAvgBytesPerSec = SAMPLE_RATE * sizeof(short);
    wfx.nBlockAlign = sizeof(short);
    wfx.wBitsPerSample = 16;
    wfx.cbSize = 0;

    // prepare WAVEHDR structure
    WAVEHDR whdr;
    whdr.lpData = (LPSTR)buffer;
    whdr.dwBufferLength = samples * sizeof(short);
    whdr.dwFlags = 0;
    whdr.dwLoops = 0;

    // open wave out device
    HWAVEOUT hwo;
    // waveOutOpen(&hwo, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    waveOutOpen(&hwo, deviceId, &wfx, 0, 0, CALLBACK_NULL);
    waveOutPrepareHeader(hwo, &whdr, sizeof(WAVEHDR));
    waveOutWrite(hwo, &whdr, sizeof(WAVEHDR));

    // wait until sound finished
    while (waveOutUnprepareHeader(hwo, &whdr, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING) {
        Sleep(10);
    }

    waveOutClose(hwo);

    delete[] buffer;
}

int main() {
    // melody and rhythm
    struct Note {
        int frequency;
        int duration;
    } marioTheme[] = {
        {E5, 150}, {E5, 150}, {REST, 150}, {E5, 150},
        {REST, 150}, {C5, 150}, {E5, 150}, {REST, 150},
        {G5, 150}, {REST, 450}, {G5, 150}, {REST, 450},
        {C5, 150}, {REST, 300}, {G4, 150}, {REST, 300},
        {E4, 150}, {REST, 150}, {A4, 150}, {REST, 150},
        {B4, 150}, {REST, 150}, {A5, 150}, {REST, 150},
        {G5, 150}, {REST, 150}, {E5, 150}, {REST, 150},
        {G5S, 150}, {REST, 150}, {A5, 150}, {REST, 150},
        {F5, 150}, {G5, 150}, {REST, 150}, {E5, 150},
        {REST, 150}, {C5, 150}, {D5, 150}, {B4, 150},
        {REST, 450}
    };

    int numNotes = sizeof(marioTheme) / sizeof(marioTheme[0]);

    // play at volume (0.5 = 50% volume)
    double volume = 1;

    int numDev = waveOutGetNumDevs();

    for (int d = 0; d < numDev; d++) {
        for (int i = 0; i < numNotes; i++) {
            playTone(marioTheme[i].frequency, marioTheme[i].duration, volume, d);
        }
    }

    return 0;
}

