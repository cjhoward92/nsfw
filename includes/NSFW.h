#ifndef NSFW_H
#define NSFW_H

#include "NativeInterface.h"
#include <nan.h>
#include <uv.h>
#include <vector>

using namespace Nan;

class NSFW : public ObjectWrap {
public:
  static NAN_MODULE_INIT(Init);

  static void cleanupEventCallback(void *arg);
  static void fireErrorCallback(uv_async_t *handle);
  static void fireEventCallback(uv_async_t *handle);
  static void fireWatcherCallback(uv_async_t *handle);
  static void cleanUpWatcherBatonAndHandle(uv_handle_t *handle);
  static void throttledWatcherCallback(uv_work_t *req);

  Persistent<v8::Object> mPersistentHandle;
  uint32_t mDebounceMS;
  uint64_t mLastScheduledCallback;
  uv_async_t mErrorCallbackAsync;
  uv_async_t mEventCallbackAsync;
  uv_async_t *mWatcherCallbackHandle;
  Callback *mErrorCallback;
  Callback *mEventCallback;
  NativeInterface *mInterface;
  uv_mutex_t mInterfaceLock;
  bool mInterfaceLockValid;
  std::string mPath;
  bool mRunning;
private:
  NSFW(uint32_t debounceMS, std::string path, Callback *eventCallback, Callback *errorCallback);
  ~NSFW();

  struct ErrorBaton {
    NSFW *nsfw;
    std::string error;
  };

  struct EventBaton {
    NSFW *nsfw;
    std::vector<Event *> *events;
  };

  static NAN_METHOD(JSNew);

  static NAN_METHOD(Start);
  class StartWorker : public AsyncWorker {
  public:
    StartWorker(NSFW *nsfw, Callback *callback);
    void Execute();
    void HandleOKCallback();
  private:
    NSFW *mNSFW;
  };

  static NAN_METHOD(Stop);
  class StopWorker : public AsyncWorker {
  public:
    StopWorker(NSFW *nsfw, Callback *callback);
    void Execute();
    void HandleOKCallback();
  private:
    NSFW *mNSFW;
  };

  static Persistent<v8::Function> constructor;
};

#endif
