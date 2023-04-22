#include <LightPool.h>

/**
* @param nThreads Initialize the Construct for Dynamic thread initialization
*/
LightPool::LightPool(int nThreads) {
    this->nThreads = nThreads;
    this->active = 1;
    this->pool = new pthread_t[nThreads];
    for (int i = 0; i < nThreads; i++) {
        pthread_create(&pool[i], NULL, workHandler, (void*)this);
    }
}
/**
* @param j add the Job function you want to add
*/
void LightPool::addWork(Job j) { 
    this->lockPool();
    if (j.getPriority() > 0) 
        highPriorityJobs.push(j);
    else
        jobs.push(j);
    
    this->signal();
    this->unlockPool();
}
/**
* Handle Obtain the Job Data
*/
Job LightPool::getJob(){ 
    //this->lockPool();
    Job job;
    if (!highPriorityJobs.empty()) {
        job = highPriorityJobs.front();
        highPriorityJobs.pop();
    }
    else {
        job = jobs.front();
        jobs.pop();
    }
    //this->unlockPool();
    return job;
}
LightPool::~LightPool() {
    this->active = 0;
    for(int i = 0; i < nThreads ; i++)
        signal();
    for (int i = 0; i < nThreads; i++) {
        pthread_join(pool[i], NULL);
    }
    delete pool;
}
/**
* Function that handles the work assigned
* @param arg arguments to be passed to.
*/
void* LightPool::workHandler(void* arg) {
    LightPool* lpool = (LightPool*)arg;
    while (lpool->getActive() == 1) {
        lpool->lockPool();
        while (!lpool->m_stop && lpool->EmptyJobs()) {
            lpool->condWait();
        }
        Job job = lpool->getJob();
        // lpool->popJob(); Popping is handled on getJob() itself.
        lpool->unlockPool();
        if (job.getFunc()) {
            job.exec();
        }
    }
    return NULL;
}
