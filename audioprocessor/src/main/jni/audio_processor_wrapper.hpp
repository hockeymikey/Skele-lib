#include <jni.h>
#include "audio_processor.hpp"

#include "mp3_compressor.hpp"
#include "stream_writer.hpp"

#ifndef WRAPPER_H_
#define WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL Java_com_cogi_audioprocessor_AudioProcessor_init(JNIEnv *env, jclass jcls, jint sampleRate, jint quality);

JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_initWithKillThresholdSeconds(JNIEnv *env, jclass jcls, jlong pointer, jint killThresholdSeconds);

JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_setKillThresholdSeconds(JNIEnv *env, jclass jcls, jlong pointer, jint killThresholdSeconds);

JNIEXPORT jboolean JNICALL Java_com_cogi_audioprocessor_AudioProcessor_samplesAreBeingWrittenToFilepath(JNIEnv *env, jclass jcls, jlong pointer, jstring filepath);

JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_startHighlightWithRecommendedDelay__JILjava_lang_String_2I(JNIEnv *env, jclass jcls, jlong pointer, jint recommendedDelay, jstring mp3Path, jint compressionQuality);

JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_startHighlightWithRecommendedDelay__JILjava_lang_String_2Ljava_lang_String_2I(JNIEnv *env, jclass jcls, jlong pointer, jint recommendedDelay, jstring wavePath, jstring mp3Path, jint compressionQuality);

JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_startHighlightWithRecommendedDelay__JILjava_lang_String_2(JNIEnv *env, jclass jcls, jlong pointer, jint recommendedDelay, jstring wavePath);

JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_startHighlightWithRecommendedDelay__JI(JNIEnv *env, jclass jcls, jlong pointer, jint recommendedDelay);

JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_stopHighlightWithQueueFlushed(JNIEnv *env, jclass jcls, jlong pointer, jboolean flushQueue);

JNIEXPORT jdouble JNICALL Java_com_cogi_audioprocessor_AudioProcessor_bufferedAudioSizeInSeconds(JNIEnv *env, jclass jcls, jlong pointer);

JNIEXPORT jdouble JNICALL Java_com_cogi_audioprocessor_AudioProcessor_sampleCountLimit(JNIEnv *env, jclass jcls);

JNIEXPORT jobject JNICALL Java_com_cogi_audioprocessor_AudioProcessor_processSamples(JNIEnv *env, jclass jcls, jlong pointer, jint samples,jint sampleCount);

JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_close(JNIEnv* env, jclass cls, jlong pointer);

#ifdef __cplusplus
}
#endif

#endif /* WRAPPER_H_ */
