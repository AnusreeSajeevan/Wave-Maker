//
// Created by Design on 18-01-2018.
//

#ifndef WAVEMAKER_OSCILLATOR_H
#define WAVEMAKER_OSCILLATOR_H

#include <stdint.h>
#include <atomic>

class Oscillator {
public:
    void setWaveOn(bool isWaveOn);
    void setSampleRate(int32_t sampleRate);
    void render(float *audio, int32_t numFrames);

private:
//    define this as atomis bool cause it will be accessing from multiple threads
    std::atomic<bool> isWaveOn_{false};

    /**
     * phase_ stores the current wave phase, and it is incremented by phaseIncrement_ after each sample is generated.
     */
    double phase_ = 0.0;
    double phaseIncrement_ = 0.0;
};


#endif //WAVEMAKER_OSCILLATOR_H
