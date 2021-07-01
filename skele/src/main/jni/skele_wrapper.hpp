#include <android/log.h>
#include <jni.h>
#include <../cpp/skele.hpp>

#define  LOG_TAG    "your-log-tag"

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)


#ifndef WRAPPER_H_
#define WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jboolean JNICALL Java_com_cogi_skele_Skele_testlocks(JNIEnv *env, jclass jcls);

#ifdef __cplusplus
}
#endif

#endif /* WRAPPER_H_ */
