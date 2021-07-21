/*
 * @Author: czw
 * @Date: 2021-07-21 08:40:49
 * @LastEditors: czw
 * @LastEditTime: 2021-07-21 11:33:12
 */

#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

//信号量类封装
class sem
{
public:
    //默认构造
    sem()
    {
        if (sem_init(&m_sem, 0, 0) != 0)
        {
            throw std::exception();
        }
    }
    //构造 参数数量
    sem(int num)
    {
        if (sem_init(&m_sem, 0, num) != 0)
        {
            throw std::exception();
        }
    }
    //析构
    ~sem()
    {
        //清除信号量
        sem_destroy(&m_sem);
    }
    bool wait()
    {
        //获取一个资源
        return sem_wait(&m_sem) == 0;
    }
    bool post()
    {   
        //释放一个资源
        return sem_post(&m_sem) == 0;
    }

private:
    //信号量
    sem_t m_sem;
};

//互斥锁类
class locker
{
public:
    //默认构造
    locker()
    {
        if (pthread_mutex_init(&m_mutex, NULL) != 0)
        {
            throw std::exception();
        }
    }
    //析构
    ~locker()
    {
        pthread_mutex_destroy(&m_mutex);
    }
    //枷锁
    bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;
    }
    //解锁
    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }
    //获取当前锁
    pthread_mutex_t *get()
    {
        return &m_mutex;
    }

private:
    pthread_mutex_t m_mutex;
};

//条件变量
class cond
{
public:
    cond()
    {
        if (pthread_cond_init(&m_cond, NULL) != 0)
        {
            //pthread_mutex_destroy(&m_mutex);
            throw std::exception();
        }
    }
    ~cond()
    {
        pthread_cond_destroy(&m_cond);
    }
    bool wait(pthread_mutex_t *m_mutex)
    {
        int ret = 0;
        //pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond, m_mutex);
        //pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }
    bool timewait(pthread_mutex_t *m_mutex, struct timespec t)
    {
        int ret = 0;
        //pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_timedwait(&m_cond, m_mutex, &t);
        //pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }
    bool signal()
    {
        return pthread_cond_signal(&m_cond) == 0;
    }
    bool broadcast()
    {
        return pthread_cond_broadcast(&m_cond) == 0;
    }

private:
    //static pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};

#endif