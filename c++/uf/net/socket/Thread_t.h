// ======================================================================================
// File         : Thread.h
// Author       : Tory Lin 
// Last Change  : 04/26/2011 | 16:44:16 PM | Tuesday,April
// Description  : 简单封装的跨平台线程类
// ======================================================================================

#ifndef __THREAD_T_H__
#define __THREAD_T_H__


#ifdef WIN32
#include <winsock2.h>
#include <process.h>
#else
#include <stdio.h>
#include <pthread.h>
#endif


#ifdef WIN32
#define SLEEP_MS(t) Sleep(t)
#else
#define SLEEP_MS(t) usleep((t)*1000)
#endif

#ifdef _WIN32
// to be
//typedef DWORD  threadfunc_t;
//typedef LPVOID threadparam_t;
//#define STDPREFIX WINAPI
typedef unsigned threadfunc_ex_t;
typedef void * threadparam_ex_t;
#define STDPREFIX_EX __stdcall
#else

typedef void * threadfunc_ex_t;
typedef void * threadparam_ex_t;
#define STDPREFIX_EX
#endif

class thread_ex
{
protected:
    //-- 子类需要实现此线程函数 --//
    virtual void Run() = 0;     
public:
    //-- 用户代码开始执行线程时调用此函数 --//
    virtual bool RunThread()
    {
#ifdef _WIN32
        //	m_thread = ::CreateThread(NULL, 0, StartThread, this, 0, &m_dwThreadId);
        m_thread = (HANDLE)_beginthreadex(NULL, 0, &StartThread, this, 0, &m_dwThreadId);
#else
        pthread_attr_init(&m_attr);
        pthread_attr_setdetachstate(&m_attr,PTHREAD_CREATE_DETACHED);
        if (pthread_create(&m_thread,&m_attr, StartThread,this) == -1)
        {
            perror("Thread: create failed");
            //SetRunning(false);
        }
#endif
        return true;
//#ifdef WIN32
//        DWORD dwID;
//        m_hHandle = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StaticRun, this, 0, &dwID);
//        return true;
//#else
//        return pthread_create(&thread_ex::m_thread,NULL,thread_function,this);
//#endif
    }
  
    void stopThread()
    {
#ifdef _WIN32
        if (m_thread)
        {
            ::CloseHandle(m_thread);
            m_thread = NULL;
        }
#else
        pthread_attr_destroy(&m_attr);
#endif
//#ifdef WIN32
//        if (m_hHandle)
//        {
//            CloseHandle(m_hHandle);
//            m_hHandle =NULL;
//        }
//#else
//        // ... TODO
//#endif
    }

public:
    thread_ex(void)
#ifdef _WIN32
        : m_thread(NULL)
        , m_dwThreadId(0)
#else
     //   : m_attr(NULL)
#endif
    {};
    virtual ~thread_ex(void)
    { 
        stopThread(); 
    }


private:

    static threadfunc_ex_t STDPREFIX_EX StartThread(threadparam_ex_t pParam)
    {
        thread_ex* pThread = (thread_ex*)pParam;

        if(pThread != NULL)
        {
            pThread->Run();
        }

#ifdef _WIN32
        _endthreadex(0);
#endif

        return (threadfunc_ex_t)NULL;
    }

//#ifdef WIN32
//    static DWORD StaticRun(LPVOID pParam)
//    {
//        thread_ex* pThread = (thread_ex*)pParam;
//        if(pThread != NULL)
//        {
//            pThread->Run();
//        }
//        return true;
//    }
//#else
//    static void* thread_function(void* arg)
//    {
//        thread_ex* pThread = (thread_ex*)arg;
//        if(pThread != NULL)
//        {
//            pThread->Run();
//        }
//        return NULL;
//    }
//#endif

private:
#ifdef _WIN32
    HANDLE m_thread;
    unsigned m_dwThreadId;
#else
    pthread_t m_thread;
    pthread_attr_t m_attr;
#endif

//#ifdef WIN32
//    HANDLE m_hHandle;
//#else
//    pthread_t m_thread;
//#endif
};

#endif
