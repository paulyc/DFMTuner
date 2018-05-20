#include <jni.h>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <queue>
#include <typeinfo>
#include <unistd.h>
#include <fcntl.h>

#include "../../../libs/libnrsc5/src/output.h"

template <int BufSz>
class BufferPool
{
public:
  BufferPool() {
    for (int i = 0; i < DEFAULT_POOL_SIZE; ++i) {
      _pool.push(alloc());
    }
  }
  virtual ~BufferPool() {
    while (!_pool.empty()) {
      _pool.pop();
    }
  }
  std::unique_ptr<uint8_t[]> get() {
    if (_pool.empty()) {
      return alloc();
    } else {
      std::unique_ptr<uint8_t[]> buf;
      buf.swap(_pool.front());
      _pool.pop();
      return buf;
    }
  }
  void put(std::unique_ptr<uint8_t[]> buf) {
    _pool.push(buf);
  }
private:
  std::unique_ptr<uint8_t[]> alloc() {
    return std::unique_ptr<uint8_t[]>(new uint8_t[BufSz]);
  }

  static const int DEFAULT_POOL_SIZE = 10;
  std::queue<std::unique_ptr<uint8_t[]>> _pool;
};

class BufferLogger {
public:
  BufferLogger() {
    _f = fmemopen(NULL, 4096, "rw");
  }
  virtual ~BufferLogger() {
    fclose(_f);
  }
  void redirectOutput() {
    const int fd = fileno(_f);
    dup2(fd, fileno(stderr));
    dup2(fd, fileno(stdout));
  };
  void log(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(_f, fmt, args);
    va_end(args);
  }
private:
  FILE *_f;
};

extern "C" void configure(unsigned int frequency, unsigned int program, void(*audio_cb)(void*,uint32_t));

class TunerExecutor {
public:
  TunerExecutor() :
      _logger(new BufferLogger) {
    _logger->redirectOutput();
  }
  virtual ~TunerExecutor() {
  }
  struct Event {
    virtual void f() {}
  };
  struct ConfigureEvent : Event {
    unsigned long frequency;
    int program;
  };
  struct ProcessAudioEvent : Event {
    std::unique_ptr<uint8_t[]> buffer;
  };
  struct StopEvent : Event {};
  void run() {
    _t.reset(new std::thread([this]() {
      while (true) {
        const std::unique_ptr<Event> ev = popEvent();
        const std::type_info &ti = typeid(*ev);

        if (ti == typeid(ProcessAudioEvent)) {
        } else if (ti == typeid(ConfigureEvent)) {

        } else if (ti == typeid(StopEvent)) {
          break;
        }
      }
    }));
  }

  void stop() {
    if (_t != nullptr) {
      pushEvent(new StopEvent);
      _t->join();
      _t.reset(nullptr);
    }
  }

  void configure(unsigned long frequency, int program) {
    ConfigureEvent *e = new ConfigureEvent;
    e->frequency = frequency;
    e->program = program;
    pushEvent(e);
  }

  void onAudioCallback(void *buf, size_t sz) {
    // num_bytes always AUDIO_FRAME_BYTES
    //uint32_t num_samples = num_bytes / BYTES_PER_SAMPLE;
    //if (num_samples != AUDIO_FRAME_BYTES) {
    //  abort();
    // }

    ProcessAudioEvent *e = new ProcessAudioEvent;
    e->buffer = _p.get();
    memcpy(&e->buffer[0], buf, sz);
    pushEvent(e);
  }

  void pushEvent(Event *e) {
    std::lock_guard<std::mutex> lock(_m);

    _q.push(std::unique_ptr<Event>(e));
    _c.notify_one();
  }

  std::unique_ptr<Event> popEvent() {
    std::unique_lock<std::mutex> lock(_m);
    std::unique_ptr<Event> ev;

    lock.lock();
    while (_q.empty()) {
      _c.wait(lock);
    }
    _q.front().swap(ev);
    _q.pop();
    lock.unlock();
    return ev;
  }

private:
  BufferPool<AUDIO_FRAME_BYTES> _p;
  std::queue<std::unique_ptr<Event>> _q;
  std::unique_ptr<std::thread> _t;
  std::mutex _m;
  std::condition_variable _c;
  std::shared_ptr<BufferLogger> _logger;
};

//public native String stringFromJNI();
//public native void configure(int frequency, int program);
//public native void poll();

static std::unique_ptr<TunerExecutor> executor;

extern "C" JNIEXPORT jstring JNICALL
Java_com_github_paulyc_dfmtuner_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" void audio_cb(void *data, uint32_t num_bytes) {
  // put in queue
  executor->onAudioCallback(data, num_bytes);

}

extern "C" JNIEXPORT void JNICALL
Java_com_github_paulyc_dfmtuner_Tuner_init(
    JNIEnv *env,
    jobject this_
) {
  if (executor == nullptr) {
    executor.reset(new TunerExecutor);
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_paulyc_dfmtuner_Tuner_start(
    JNIEnv *env,
    jobject this_
) {
  if (executor != nullptr) {
    executor->run();
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_paulyc_dfmtuner_Tuner_stop(
    JNIEnv *env,
    jobject this_
) {
  if (executor != nullptr) {
    executor->stop();
  }
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_github_paulyc_dfmtuner_Tuner_pollAudioData(
    JNIEnv *env,
    jobject this_
) {
  /*
  jmethodID onLogMessage = env->GetMethodID(env->GetObjectClass(this_), "onLogMessage", "(Ljava/lang/String;)V");
  jmethodID onAudioData = env->GetMethodID(env->GetObjectClass(this_), "onAudioData", "([B)V");

  //fetch from queue

  pthread_mutex_lock(&cb_data.lock);
  while (!cb_data.log_q.empty()) {
    const std::string &msg = cb_data.log_q.front();
    cb_data.log_q.pop();
    jstring jmsg = env->NewStringUTF(msg.c_str());
    env->CallVoidMethodA(this_, onLogMessage, (jvalue*)&jmsg);
  }
  while (!cb_data.q.empty()) {
    const q_entry &ent = cb_data.q.front();
    cb_data.q.pop();
    jbyteArray a = env->NewByteArray(ent.num_bytes);
    //env->GetByteArrayRegion(a, 0, ent.num_bytes, &buf);
    env->SetByteArrayRegion(a, 0, ent.num_bytes, (jbyte*)ent.data);
    env->ReleaseByteArrayElements(a, (jbyte*)ent.data, 0);
    env->CallVoidMethodA(this_, onAudioData, (jvalue*)&a);
  }
  pthread_mutex_unlock(&cb_data.lock);*/
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_paulyc_dfmtuner_Tuner_configure(
    JNIEnv *env,
    jobject this_,
    jint frequency,
    jint program
) {
  if (executor != nullptr) {
    executor->configure(frequency, program);
  }
}
