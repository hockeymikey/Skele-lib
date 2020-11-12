#include <jni.h>
#include "mp3_compressor.hpp"

#ifndef WRAPPER_H_
#define WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL Java_com_cogi_audioprocessor_Mp3Compressor_init(JNIEnv * , jclass, jint quality, jint samples);

JNIEXPORT jint JNICALL Java_com_cogi_audioprocessor_Mp3Compressor_encode(JNIEnv *, jclass, jlong pointer, jshortArray bufferIn, jbyteArray BufferOut, jint samples);

JNIEXPORT jint JNICALL Java_com_cogi_audioprocessor_Mp3Compressor_flush(JNIEnv *, jclass, jlong pointer, jbyteArray buffer);

JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_Mp3Compressor_close(JNIEnv *, jclass, jlong pointer);

JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_Mp3Compressor_encodeFile(JNIEnv *env, jclass jcls, jlong pointer, jstring in_source_path, jstring in_target_path);

#ifdef __cplusplus
}
#endif

#endif /* WRAPPER_H_ */
