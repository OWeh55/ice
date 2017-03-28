#ifndef __ICETHREADCOMMHELPER_H
#define __ICETHREADCOMMHELPER_H

#  include <wx/thread.h>

namespace ice
{
  /**
     A mixin class to provide some synchronisation facilities.
     This class has been designed to compensate for the wxWidgets restriction that
     only the main thread can have a message queue (although I don't know any operating
     system that would make this restriction necessary). This restriction is fine for mose
     projects, since it prevents the programmer from creating non-thread-safe GUI
     objects in different threads and mess up the application. The original intention
     from the wxWidgets guys was, that all threads apart from the main thread should be
     worker threads, only.
     <BR>This makes our code a bit more complicated. Since sometimes the user thread (the thread that
     runs the user's Main() function, see iceApp) must wait for completion of an operation
     inside the main thread (for example the create-image-window function), we have to
     use some synchronisation primitives apart from events. Unfortunately, the wxCondition
     class, that wraps pthread conditions or win32 events, behaves slightly different from
     the win32 event objects: if the condition becomes signalled and no thread is waiting for the
     condition, the signal will be lost.
     <BR>Instead of using wxCondition, I tried to create a solution based on mutexes. After some
     consideration I came to the conclusion that one mutex would not be sufficient, so I
     had to use two mutexes. Whenever the user thread has to wait for some operation inside
     the main thread to finish, it can wait for one mutex. This mutex has been locked inside
     the main thread, and will be released when the critical operation has finished, so that
     the user thread can continue. For a consecutive wait the user thread will have to wait
     for the second mutex, because there is no way to ensure that the first mutex has already
     been locked by the main thread. Thus both mutexes will be used one after another.
     This may look a bit confusing at the first moment, but it works just fine!
     <BR>This class is designed as a mixin-class, that provides the two methods
     ThreadCommHelper::WaitForMainThread () and ThreadCommHelper::WakeUpUserThread ().
     The first can only be called from inside
     the user thread, the latter only from inside the main thread.*/

  class ThreadCommHelper
  {
  public:
    ThreadCommHelper();
    ~ThreadCommHelper();

    /*! Wait until the main thread has finished processing. That could be the case if input
      from the main thread has become available, or the main thread has finished some
      GUI critical operations. This method can only be called from inside the user thread.*/
    void WaitForMainThread();

    /*! Wake up the user thread. This method must be called from inside the main thread.*/
    void WakeUpUserThread();
  private:
    //! The mutexes for communication with the user thread.
    wxMutex MutexA, MutexB;

    //! This index tells us which Mutex should be used for the next communication cycle
    // Each communication partner must have it's own index, else they could get confused
    int CurrMutexMain,
        CurrMutexUser;
  };

  class EventHelper: private ThreadCommHelper
  {
  public:
    // Callable from user thread: wait for event from main thread
    long WaitForEvent();
    // Callable from main thread: send event to user thread
    void SendEvent(long event);

  private:
    //! Event to transfer from main thread to user thread
    long Event;
  };
}
#endif // #ifndef __ICETHREADCOMMHELPER_H
