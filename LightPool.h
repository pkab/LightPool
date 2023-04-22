#ifndef LIGHTPOOL_H
#define LIGHTPOOL_H

#include <pthread.h>
#include <queue>

// Job Class to Handle Jobs
class Job {
public:
    Job(){}
    Job(void* (*func)(void* arg), void* arg, int priority = 0) {
        this->func = func;
        this->arg = arg;
        this->priority = priority;
    }
    void* (*getFunc())(void*) {
        return func;
    }
    void exec() {
        func(arg);
    }
    int getPriority(){ return priority; }
private:
    void* (*func)(void* arg);
    void* arg;
    int priority;
};

class LightPool {
public:
    LightPool(int nThreads);
    ~LightPool();
    int getActive() { return active; }
    // void popJob() { jobs.pop(); }
    Job getJob();
    void lockPool() { pthread_mutex_lock(&lock); }
    void unlockPool() { pthread_mutex_unlock(&lock); }
    void condWait() { pthread_cond_wait(&sig, &lock); }
    void signal() { pthread_cond_signal(&sig); }
    void addWork(Job j);
    bool EmptyJobs() { return jobs.empty() && highPriorityJobs.empty(); }
    void stop(){ m_stop=true; }
    bool getStop() { return m_stop; }
private:
    int nThreads;
    int active;
    std::queue<Job> jobs;
    std::queue<Job> highPriorityJobs;
    pthread_t* pool;
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t sig = PTHREAD_COND_INITIALIZER;
    bool m_stop = false;
    static void *workHandler(void* args);
};

#endif // LIGHTPOOL_H