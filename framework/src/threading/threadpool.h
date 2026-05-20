#ifndef PROJECTLABORATORY_THREADPOOL_H
#define PROJECTLABORATORY_THREADPOOL_H

#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>
#include "jobpool.h"
#include "timer.h"

namespace Framework {
    class  Job;
    struct JobVisitor;

    class ThreadPool {
        static inline std::vector<std::jthread> pool;
        static inline std::condition_variable cv;

        static inline std::mutex activeGuard;
        static inline std::mutex completedGuard;
        static inline std::queue<Job*> activeJobs;
        static inline std::deque<Job*> completedJobs;

        static inline JobPool jobPool;
        static inline Timer timer;

        static void working(const std::stop_token& flag);
        static void dequeueJob(const std::stop_token &flag);

        ThreadPool() = default;
    public:
        static void start(unsigned int nThread,unsigned int availableJobs, unsigned int timeLimit);
        static void stop();
        static void enqueueJob(JobVisitor* visitor);
        static void processCompletedJobs();
    };
}

#endif //PROJECTLABORATORY_THREADPOOL_H