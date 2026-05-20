#include "threadpool.h"

#include "job.h"

namespace Framework {
    void ThreadPool::working(const std::stop_token& flag) {
        while (!flag.stop_requested()) {
            dequeueJob(flag);
        }
    }

    void ThreadPool::dequeueJob(const std::stop_token &flag) {
        Job* job;

        {
            std::unique_lock lock(activeGuard);

            cv.wait(lock, [&] {
                return flag.stop_requested() || !activeJobs.empty();
            });

            if (flag.stop_requested() || activeJobs.empty()) {
                return;
            }

            job = activeJobs.front();
            activeJobs.pop();
        }

        job->execute();
        {
            std::scoped_lock lock(completedGuard);
            completedJobs.push_back(job);
        }
    }

    void ThreadPool::start(const unsigned int nThread, const unsigned int availableJobs, const unsigned int timeLimit) {
        if (!pool.empty()) return;

        timer.setLimitInMicros(timeLimit);
        jobPool.init(availableJobs);

        for (unsigned int i = 0; i < (nThread == 0 ? 4 : nThread); i++) {
            pool.emplace_back(working);
        }
    }

    void ThreadPool::stop() {
        for (auto& thread : pool) {
            thread.request_stop();
        }

        cv.notify_all();
        pool.clear();

        {
            std::scoped_lock lock(activeGuard);
            activeJobs = {};
        }
        {
            std::scoped_lock lock(completedGuard);
            completedJobs.clear();
        }
        jobPool.destroy();
    }

    void ThreadPool::enqueueJob(JobVisitor* visitor) {
        {
            std::scoped_lock lock(activeGuard);
            activeJobs.push(jobPool.allocate(visitor));
        }
        cv.notify_one();
    }

    void ThreadPool::processCompletedJobs() {
        {
            std::scoped_lock lock(completedGuard);
            if (completedJobs.empty())
                return;
        }

        int maxJobs;
        {
            std::scoped_lock lock(completedGuard);
            maxJobs = completedJobs.size();
        }

        timer.launch();
        for (unsigned int i = 0; i < maxJobs; i++) {
            Job* job;

            {
                std::scoped_lock lock(completedGuard);
                if (completedJobs.empty())
                    return;

                job = completedJobs.front();
                completedJobs.pop_front();
            }

            if (job->ready()) {
                jobPool.free(job);
            }else {
                std::scoped_lock lock(completedGuard);
                completedJobs.push_back(job);
            }

            if (timer.outOfTime())
                return;
        }
    }
}