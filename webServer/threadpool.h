
#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "locker.h"

/*半同步/半反应堆模式的线程池：
 * 主线程（异步现成）来监听ＩＯ事件，同步线程来处理客户请求。
 * 主线程采用的事件驱动模式是Ｒｅａｃｔｏｒ模式。
 * 主线程往工作队列插入任务，工作线程通过竞争来取得任务并处理。*/

template< typename T >
class threadpool
{
public:
    /*thread_number是线程池中线程的数量，max_requests是请求队列中最多允许的请求数量*/
    threadpool( int thread_number = 4, int max_requests = 1000 );
    ~threadpool();

    /*往请求队列中添加任务*/
    bool append( T* request );

private:
    /*工作线程函数*/
    static void* worker( void* arg );


    void run();

private:
    int m_thread_number;	/*线程池中的线程数*/
    int m_max_requests;    /*请求队列中允许的最大请求数*/
    bool m_stop;	/*是否结束线程*/
    pthread_t* m_threads;	/*描述线程池的数组，其大小为m_thread_number*/
    std::list< T* > m_workqueue;	/*请求队列*/
    locker m_queuelocker;    /*请求队列的互斥锁*/
    sem m_queuestat;	/*是否有任务需要处理*/
};

template< typename T >
threadpool< T >::threadpool( int thread_number, int max_requests ) :
        m_thread_number( thread_number ), m_max_requests( max_requests ), m_stop( false ), m_threads( NULL )
{
    if( ( thread_number <= 0 ) || ( max_requests <= 0 ) )
    {
        throw std::exception();
    }

    m_threads = new pthread_t[ m_thread_number ];
    if( ! m_threads )
    {
        throw std::exception();
    }

    for ( int i = 0; i < thread_number; ++i )
    {
        printf( "create the %dth thread\n", i );
        if( pthread_create( m_threads + i, NULL, worker, this ) != 0 )
        {
            delete [] m_threads;
            throw std::exception();
        }
        if( pthread_detach( m_threads[i] ) )
        {
            delete [] m_threads;
            throw std::exception();
        }
    }
}

template< typename T >
threadpool< T >::~threadpool()
{
    delete [] m_threads;
    m_stop = true;
}

template< typename T >
bool threadpool< T >::append( T* request )
{
    m_queuelocker.lock();
    if ( m_workqueue.size() > (unsigned int)m_max_requests )
    {
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back( request );
    m_queuelocker.unlock();
    m_queuestat.post();   /*通知工作线程*/
    return true;
}

template< typename T >
void* threadpool< T >::worker( void* arg )
{
    threadpool* pool = ( threadpool* )arg;
    pool->run();
    return pool;
}

template< typename T >
void threadpool< T >::run()
{
    while ( !m_stop )
    {
        m_queuestat.wait();
        m_queuelocker.lock();
        if ( m_workqueue.empty() )
        {
            m_queuelocker.unlock();
            continue;
        }
        T* request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();
        if ( !request )
            continue;

        /*任务接口*/
        request->process();
    }
}



#endif /* THREADPOOL_H_ */
