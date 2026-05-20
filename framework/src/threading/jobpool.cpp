#include "jobpool.h"

#include "job.h"

namespace Framework {
    void JobPool::init(const unsigned int maxJobs) {
        destroy();
        size = maxJobs;
        availableJobs.resize(size);
        for (unsigned int i = 0; i < size; i++) {
            availableJobs[i] = new Job();
        }
    }

    Job* JobPool::allocate(JobVisitor* visitor) {
        Job* p = nullptr;
        std::scoped_lock lock(availableGuard);
        if (availableJobs.empty()) {
            p = new Job();
        }else {
            p = availableJobs.back();
            availableJobs.pop_back();
        }
        p->init(visitor);
        return p;
    }

    void JobPool::free(Job* job) {
        job->reset();
        std::scoped_lock lock(availableGuard);
        if (availableJobs.size() < size) {
            availableJobs.push_back(job);
            return;
        }
        delete job;
    }

    void JobPool::destroy() {
        std::scoped_lock lock(availableGuard);
        availableJobs.clear();
    }
}