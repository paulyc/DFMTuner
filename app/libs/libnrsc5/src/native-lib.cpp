#include <jni.h>
#include <string>

#include "output.h"


static struct {
  JNIEnv *env;
  jobject tuner;
  jmethodID audio_cb_id;
  jmethodID log_id;
} cb_data;

extern "C" void audio_cb(void *data, uint32_t num_bytes) { // num_bytes always AUDIO_FRAME_BYTES
  //uint32_t num_samples = num_bytes / BYTES_PER_SAMPLE;
  //if (num_samples != AUDIO_FRAME_BYTES) {
  //  abort();
 // }
  jbyteArray copy = cb_data.env->NewByteArray(num_bytes);
  cb_data.env->SetByteArrayRegion(copy, 0, num_bytes, (jbyte*)data);
  cb_data.env->CallVoidMethodA(cb_data.tuner, cb_data.audio_cb_id, (jvalue*)&copy);
}

extern "C" JNIEXPORT jstring

JNICALL
Java_com_github_paulyc_hdfmtuner_tuner_TunerActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" void configure(unsigned int frequency, unsigned int program, void(*audio_cb)(void*,uint32_t));
extern "C" void log_jni(const char *msg) {
  jstring jmsg = cb_data.env->NewStringUTF(msg);
  cb_data.env->CallVoidMethodA(cb_data.tuner, cb_data.log_id, (jvalue*)&jmsg);
}

extern "C" JNIEXPORT void
JNICALL Java_com_github_paulyc_hdfmtuner_tuner_TunerActivity_configure(
        JNIEnv *env,
        jobject this_,
        jint frequency,
        jint program
      ) {
  cb_data.env = env;
  cb_data.tuner = this_;
  cb_data.audio_cb_id = env->GetMethodID(env->GetObjectClass(this_), "onAudioData", "([B)V");
  cb_data.log_id = env->GetMethodID(env->GetObjectClass(this_), "onLogMessage", "(Ljava/lang/String;)V");
  configure(frequency, program, audio_cb);
}
