#ifndef PROJECTLABORATORY_JOB_H
#define PROJECTLABORATORY_JOB_H

namespace Framework {
    struct JobVisitor {
        virtual void run() = 0;
        virtual bool complete() = 0;
        virtual ~JobVisitor() = default;
    };

    class Job {
        JobVisitor* job;
    public:
        Job() = default;

        void reset() {
            job = nullptr;
        }

        void init(JobVisitor* jobToDo) {
            this->job = jobToDo;
        }

        void execute() const {
            if (this->job) {
                job->run();
            }
        }

        [[nodiscard]] bool ready() const {
            return job->complete();
        }
    };
}

#endif //PROJECTLABORATORY_JOB_H