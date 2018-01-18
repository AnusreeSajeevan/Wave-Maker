//
// Created by Design on 18-01-2018.
//
#include <android/log.h>
#include "AudioEngine.h"
#include <thread>
#include <mutex>

aaudio_data_callback_result_t dataCallback(
        AAudioStream *stream,
        void *userData,
        void *audioData,
        int32_t numFrames) {

    ((Oscillator *) (userData))->render(static_cast<float *>(audioData), numFrames);
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

void errorCallback(AAudioStream *stream,
                   void *userData,
                   aaudio_result_t error){
    if (error == AAUDIO_ERROR_DISCONNECTED){
        std::function<void(void)> restartFunction = std::bind(&AudioEngine::restart,
                                                              static_cast<AudioEngine *>(userData));
        new std::thread(restartFunction);
    }
}

bool AudioEngine::start() {
    //Audio streams in AAudio are represented by the AAudioStream object, and to create one we need an AAudioStreamBuilder:
    AAudioStreamBuilder *streamBuilder;

    AAudio_createStreamBuilder(&streamBuilder);

    //our audio format is floating point numbers
    AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_FLOAT);

    //we will be outputting in mono, that is one channel
    AAudioStreamBuilder_setChannelCount(streamBuilder, 1);

    //we want the lowest possible latency. so set to low latency performance mode
    AAudioStreamBuilder_setPerformanceMode(streamBuilder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setDataCallback(streamBuilder, ::dataCallback, &oscillator_);
    AAudioStreamBuilder_setErrorCallback(streamBuilder, ::errorCallback, this);


    //Once all the parameters are set , we open the stream and check the result:
    aaudio_result_t result = AAudioStreamBuilder_openStream(streamBuilder, &stream_);

    //if the result is anything other than AAUDIO_OK, we will output the result to android monitor, and return false
    if (result != AAUDIO_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "AudioEngine", "Error opening stream %s",
                            AAudio_convertResultToText(result));
        return false;
    }

    /**
     * We deliberately didn't set the stream's sample rate, because we want to use its native sample rate -
     * i.e. the rate which avoids resampling and added latency. Now that the stream is open we can query it to find out what
     * the native sample rate is:
     */
    // Retrieves the sample rate of the stream for our oscillator.
    int32_t sampleRate = AAudioStream_getSampleRate(stream_);
    oscillator_.setSampleRate(sampleRate);

    // Starts the stream.
    result = AAudioStream_requestStart(stream_);
    if (result != AAUDIO_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "AudioEngine", "Error starting stream %s",
                            AAudio_convertResultToText(result));
        return false;
    }

    AAudioStreamBuilder_delete(streamBuilder);
    return true;
}

void AudioEngine::restart(){

    static std::mutex restartingLock;
    if (restartingLock.try_lock()){
        stop();
        start();
        restartingLock.unlock();
    }
}

void AudioEngine::stop() {
    if (stream_ != nullptr) {
        AAudioStream_requestStop(stream_);
        AAudioStream_close(stream_);
    }
}

void AudioEngine::setToneOn(bool isToneOn) {
    oscillator_.setWaveOn(isToneOn);
}
