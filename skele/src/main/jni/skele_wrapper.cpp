#include "skele_wrapper.hpp"
#include <string>


using namespace CAP;

extern "C" JNIEXPORT jboolean JNICALL Java_com_cogi_skele_Skele_testlocks(JNIEnv* env, jclass cls) {
    Skele *processor = new Skele();

    __android_log_print(ANDROID_LOG_DEBUG, "LOG_TAG", "Testing stuff!");
    processor->testingLocks();
}
