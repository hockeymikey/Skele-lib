#include "wrapper.hpp"

using namespace CAP;

extern "C" JNIEXPORT jlong JNICALL Java_com_cogi_audioprocessor_AudioProcessor_init(JNIEnv* env, jclass cls, jint sampleRate, jint quality) {
    Mp3Compressor *compressor = new Mp3Compressor(quality, sampleRate);

    return (jlong) compressor;
}

extern "C" JNIEXPORT jint JNICALL Java_com_cogi_audioprocessor_AudioProcessor_encode(JNIEnv* env, jclass cls, jlong pointer, jshortArray inputBuff, jbyteArray mp3buff, jint samples) {
    Mp3Compressor *compressor = (Mp3Compressor*) pointer;
    jboolean isCopy = false;
    jshort *bufferIn = env->GetShortArrayElements(inputBuff, &isCopy);
    jbyte *mp3buf = env->GetByteArrayElements(mp3buff, &isCopy);

    int mp3bufSize = env->GetArrayLength(mp3buff);

    int result = compressor->process(bufferIn, samples, (u_char *)mp3buf, mp3bufSize);

    env->ReleaseShortArrayElements(inputBuff, bufferIn, JNI_ABORT);
    env->ReleaseByteArrayElements(mp3buff, mp3buf, 0);
    return result;
}

extern "C" JNIEXPORT jint JNICALL Java_com_cogi_audioprocessor_AudioProcessor_flush(JNIEnv* env, jclass cls, jlong pointer, jbyteArray outputBuff) {
    jboolean isCopy = false;
    jbyte *mp3buff = env->GetByteArrayElements(outputBuff, &isCopy);
    Mp3Compressor *compressor = (Mp3Compressor*) pointer;
    auto out = AudioBuffer(reinterpret_cast<int16_t *>(mp3buff), 1050);
    int result = compressor->flush(out);
    env->ReleaseByteArrayElements(outputBuff, mp3buff, 0);
    return result;
}

extern "C" JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_close(JNIEnv* env, jclass cls, jlong pointer) {
    Mp3Compressor *compressor = (Mp3Compressor*) pointer;
    delete compressor;
}

extern "C" JNIEXPORT void JNICALL Java_com_cogi_audioprocessor_AudioProcessor_encodeFile(JNIEnv *env, jclass jcls, jlong pointer, jstring in_source_path, jstring in_target_path) {
    Mp3Compressor *compressor = (Mp3Compressor*) pointer;

    jboolean isCopy = false;
    const char *source_path, *target_path;

    source_path = env->GetStringUTFChars(in_source_path, &isCopy);
    target_path = env->GetStringUTFChars(in_target_path, &isCopy);

    compressor->encodeFile(source_path, target_path);
}