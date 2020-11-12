#include <pcm_processor.hpp>
#include <stream_writer_observer.hpp>
#include "audio_processor_wrapper.hpp"
#include <string>

using namespace CAP;

extern "C" JNIEXPORT jlong JNICALL Java_com_cogi_audioprocessor_AudioProcessor_init(JNIEnv* env, jclass cls, jint sampleRate, jint quality) {
    auto q = std::unique_ptr<AbstractCircularQueue>(new CircularQueue<60 * 44100>);

    AudioProcessor *processor = new AudioProcessor(std::move(q));

    AudioProcessorWrapper *wrapper = new AudioProcessorWrapper;
    wrapper->audioprocessor = processor;
    return (jlong) processor;
}

extern "C" JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_initWithKillThresholdSeconds(JNIEnv *env, jclass jcls, jlong pointer, jint killThresholdSeconds) {

    auto q = std::unique_ptr<AbstractCircularQueue>(new CircularQueue<60 * 44100>);
    std::uint32_t streamWriterKillThresholdInSamples = 44100 * killThresholdSeconds;

    AudioProcessorWrapper *wrapper = (AudioProcessorWrapper*) pointer;
    delete wrapper->audioprocessor;
    wrapper->audioprocessor = new AudioProcessor(std::move(q));
    wrapper->killThresholdSeconds = killThresholdSeconds;
}

extern "C" JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_setKillThresholdSeconds(JNIEnv *env, jclass jcls, jlong pointer, jint killThresholdSeconds) {
    auto q = std::unique_ptr<AbstractCircularQueue>(new CircularQueue<60 * 44100>);
    std::uint32_t streamWriterKillThresholdInSamples = 44100 * killThresholdSeconds;

    AudioProcessorWrapper *wrapper = (AudioProcessorWrapper*) pointer;
    delete wrapper->audioprocessor;
    wrapper->audioprocessor = new AudioProcessor(std::move(q));
    wrapper->killThresholdSeconds = killThresholdSeconds;
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_cogi_audioprocessor_AudioProcessor_samplesAreBeingWrittenToFilepath(JNIEnv *env, jclass jcls, jlong pointer, jstring filepath) {
    AudioProcessorWrapper *wrapper = (AudioProcessorWrapper*) pointer;

    return true; //wrapper->audioprocessor->isFileBeingProcessedAtFilepath([filepath cStringUsingEncoding:NSASCIIStringEncoding]);
}

extern "C" JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_changeKillThresholdSeconds(JNIEnv *env, jclass jcls, jlong pointer, jint killThresholdSeconds) {
    auto q = std::unique_ptr<AbstractCircularQueue>(new CircularQueue<60 * 44100>);
    //std::uint32_t streamWriterKillThresholdInSamples = 44100 * killThresholdSeconds;

    AudioProcessorWrapper *wrapper = (AudioProcessorWrapper*) pointer;
    delete wrapper->audioprocessor;
    AudioProcessor *processor = new AudioProcessor(std::move(q));
    wrapper->audioprocessor = processor;
    wrapper->killThresholdSeconds = killThresholdSeconds;
}

extern "C" JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_startHighlightWithRecommendedDelay__JILjava_lang_String_2I(JNIEnv *env, jclass jcls, jlong pointer, jint recommendedDelay, jstring mp3Path, jint compressionQuality) {
    AudioProcessorWrapper *wrapper = (AudioProcessorWrapper*) pointer;

    jboolean isCopy = false;
    std::string cMp3Path = env->GetStringUTFChars(mp3Path, &isCopy);

    auto file2 = std::unique_ptr<File>(new SystemFile(cMp3Path));
    auto mp3Processor = std::unique_ptr<SignalProcessor>(new Mp3Compressor(compressionQuality, 44100));

    auto sw2 = std::make_shared<StreamWriter>(std::move(file2), std::move(mp3Processor));

    auto observer2 = std::make_shared<StreamWriterObserver>();
    observer2->filePath = cMp3Path;
    //observer2->bridge = self;

    sw2->streamWriterObserver = observer2;


    std::vector<std::shared_ptr<StreamWriter>> sws;
    sws.push_back(sw2);

    wrapper->audioprocessor->startHighlight(sws, (std::uint32_t)(recommendedDelay * 44100));
}

extern "C" JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_startHighlightWithRecommendedDelay__JILjava_lang_String_2Ljava_lang_String_2I(JNIEnv *env, jclass jcls, jlong pointer, jint recommendedDelay, jstring wavePath, jstring mp3Path, jint compressionQuality) {
    AudioProcessorWrapper *wrapper = (AudioProcessorWrapper*) pointer;

    jboolean isCopy = false;
    std::string cWavePath = env->GetStringUTFChars(wavePath, &isCopy);

    auto file1 = std::unique_ptr<File>(new SystemFile(cWavePath));
    auto pcmProcessor = std::unique_ptr<SignalProcessor>(new PcmProcessor());

    std::string cMp3Path = env->GetStringUTFChars(mp3Path, &isCopy);
    auto file2 = std::unique_ptr<File>(new SystemFile(cMp3Path));
    auto mp3Processor = std::unique_ptr<SignalProcessor>(new Mp3Compressor(compressionQuality, 44100));

    auto sw1 = std::make_shared<StreamWriter>(std::move(file1), std::move(pcmProcessor));
    auto sw2 = std::make_shared<StreamWriter>(std::move(file2), std::move(mp3Processor));

    auto observer1 = std::make_shared<StreamWriterObserver>();
    observer1->filePath = cWavePath;
    observer1->bridge = (AudioProcessorWrapper*)pointer;

    sw1->streamWriterObserver = observer1;


    auto observer2 = std::make_shared<StreamWriterObserver>();
    observer2->filePath = cMp3Path;
    observer2->bridge = (AudioProcessorWrapper*)pointer;

    sw2->streamWriterObserver = observer2;


    std::vector<std::shared_ptr<StreamWriter>> sws;
    sws.push_back(sw1);
    sws.push_back(sw2);

    wrapper->audioprocessor->startHighlight(sws, (std::uint32_t)(recommendedDelay * 44100));
}

extern "C" JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_startHighlightWithRecommendedDelay__JILjava_lang_String_2(JNIEnv *env, jclass jcls, jlong pointer, jint recommendedDelay, jstring wavePath) {
    AudioProcessorWrapper *wrapper = (AudioProcessorWrapper*) pointer;

    jboolean isCopy = false;
    std::string cWavePath = env->GetStringUTFChars(wavePath, &isCopy);
    auto file1 = std::unique_ptr<File>(new SystemFile(cWavePath));
    auto pcmProcessor = std::unique_ptr<SignalProcessor>(new PcmProcessor());

    auto observer1 = std::make_shared<StreamWriterObserver>();
    observer1->filePath = cWavePath;
    //observer1->bridge = self;

    auto sw = std::make_shared<StreamWriter>(std::move(file1), std::move(pcmProcessor));
    sw->streamWriterObserver = observer1;
    std::vector<std::shared_ptr<StreamWriter>> sws;
    sws.push_back(sw);

    wrapper->audioprocessor->startHighlight(sws, (std::uint32_t)(recommendedDelay * 44100));
}

extern "C" JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_startHighlightWithRecommendedDelay__JI(JNIEnv *env, jclass jcls, jlong pointer,jint recommendedDelay) {
    std::vector<std::shared_ptr<StreamWriter>> sws;
    AudioProcessorWrapper *wrapper = (AudioProcessorWrapper*) pointer;
    wrapper->audioprocessor->startHighlight(sws, (std::uint32_t)(recommendedDelay * 44100));
}

extern "C" JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_stopHighlightWithQueueFlushed(JNIEnv *env, jclass jcls, jlong pointer, jboolean flushQueue) {
    AudioProcessorWrapper *wrapper = (AudioProcessorWrapper*) pointer;
    wrapper->audioprocessor->stopHighlight(flushQueue);
}

extern "C" JNIEXPORT jdouble JNICALL Java_com_cogi_audioprocessor_AudioProcessor_bufferedAudioSizeInSeconds(JNIEnv *env, jclass jcls, jlong pointer) {
    AudioProcessorWrapper *wrapper = (AudioProcessorWrapper*) pointer;
    return wrapper->audioprocessor->circularQueueSize() / 44100.0;
}

extern "C" JNIEXPORT jdouble JNICALL Java_com_cogi_audioprocessor_AudioProcessor_sampleCountLimit(JNIEnv *env, jclass jcls) {
    return AudioBuffer::AudioBufferCapacity;
}

extern "C" JNIEXPORT jobject JNICALL Java_com_cogi_audioprocessor_AudioProcessor_processSamples(JNIEnv *env, jclass jcls, jlong pointer, jint samples,jint sampleCount) {
    AudioProcessorWrapper *wrapper = (AudioProcessorWrapper*) pointer;
    AudioProcessor::Status result = wrapper->audioprocessor->processSamples((std::int16_t *)samples, sampleCount);

    jclass enumClass = env->FindClass("com/cogi/audioprocessor/AudioProcessor$AudioProcessorProcessResult");

    switch (result) {
        case AudioProcessor::Status::FullBuffer: {
            jfieldID fbuffer = env->GetStaticFieldID(enumClass, "FullBuffer",
                                                     "Lcom/cogi/audioprocessor/AudioProcessor/AudioProcessorProcessResult;");
            return env->GetStaticObjectField(enumClass, fbuffer);
        }
        case AudioProcessor::Status::Success: {
            jfieldID succ = env->GetStaticFieldID(enumClass, "Success",
                                                  "Lcom/cogi/audioprocessor/AudioProcessor/AudioProcessorProcessResult;");
            return env->GetStaticObjectField(enumClass, succ);
        }
        case AudioProcessor::Status::PriorityStreamWriterKilledDueToOverflow: {
            jfieldID prsw = env->GetStaticFieldID(enumClass,
                                                  "PriorityStreamWriterKilledDueToOverflow",
                                                  "Lcom/cogi/audioprocessor/AudioProcessor/AudioProcessorProcessResult;");
            return env->GetStaticObjectField(enumClass, prsw);
        }
        case AudioProcessor::Status::NonPriorityStreamWriterKilledDueToOverflow: {
            jfieldID npsw = env->GetStaticFieldID(enumClass,
                                                  "NonPriorityStreamWriterKilledDueToOverflow",
                                                  "Lcom/cogi/audioprocessor/AudioProcessor/AudioProcessorProcessResult;");
            return env->GetStaticObjectField(enumClass, npsw);
        }
        case AudioProcessor::Status::StreamWritersNotRunning: {
            jfieldID swnr = env->GetStaticFieldID(enumClass, "StreamWritersNotRunning",
                                                  "Lcom/cogi/audioprocessor/AudioProcessor/AudioProcessorProcessResult;");
            return env->GetStaticObjectField(enumClass, swnr);
        }
        case AudioProcessor::Status::NonPriorityStreamWriterNotRunning: {
            jfieldID npswnr = env->GetStaticFieldID(enumClass, "NonPriorityStreamWriterNotRunning",
                                                    "Lcom/cogi/audioprocessor/AudioProcessor/AudioProcessorProcessResult;");
            return env->GetStaticObjectField(enumClass, npswnr);
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_close(JNIEnv* env, jclass cls, jlong pointer) {
    AudioProcessorWrapper *wrapper = (AudioProcessorWrapper*) pointer;
    delete wrapper;
}