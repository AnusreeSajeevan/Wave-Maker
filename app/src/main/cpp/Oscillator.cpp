//
// Created by Design on 18-01-2018.
//

#include "Oscillator.h"
#include <math.h>

#define TWO_PI (3.14159 * 2)
#define AMPLITUDE 0.3
#define FREQUENCY 440.0

/**
 * method allows us to set the desired sample rate for our audio data
 * Based on sampleRate and FREQUENCY it calculates the value of phaseIncrement_, which is used in render.
 * If you want to change the pitch of the sine wave just update FREQUENCY with a new value.
 * @param sampleRate
 */
void Oscillator::setSampleRate(int32_t sampleRate) {
    phaseIncrement_ = (TWO_PI * FREQUENCY) / (double) sampleRate;
}

/**
 * This is used in render to determine whether to output the sine wave or silence.
 * @param isWaveOn is false we just output zeros (silence).
 */
void Oscillator::setWaveOn(bool isWaveOn) {
    isWaveOn_.store(isWaveOn);
}

/**
 *
 * @param audioData
 * @param numFrames is the number of audio frames which we must render.
 */
void Oscillator::render(float *audioData, int32_t numFrames) {

    if (!isWaveOn_.load()) phase_ = 0;

    for (int i = 0; i < numFrames; i++) {

        if (isWaveOn_.load()) {

            // Calculates the next sample value for the sine wave.
            audioData[i] = (float) (sin(phase_) * AMPLITUDE);

            // Increments the phase, handling wrap around.
            phase_ += phaseIncrement_;
            if (phase_ > TWO_PI) phase_ -= TWO_PI;

        } else {
            // Outputs silence by setting sample value to zero.
            audioData[i] = 0;
        }
    }
}