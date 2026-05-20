#ifndef PROJECTLABORATORY_JOBPOOL_H
#define PROJECTLABORATORY_JOBPOOL_H

#include <mutex>
#include <deque>

namespace Framework {
    class  Job;
    struct JobVisitor;

    class JobPool {
        unsigned int size{0};
        std::mutex availableGuard;
        std::deque<Job*> availableJobs;
    public:
        JobPool() = default;
        void init(unsigned int maxJobs);
        Job* allocate(JobVisitor* visitor);
        void free(Job* job);
        void destroy();
    };
}

#endif //PROJECTLABORATORY_JOBPOOL_H