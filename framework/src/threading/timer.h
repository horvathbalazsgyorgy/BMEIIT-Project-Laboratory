#ifndef PROJECTLABORATORY_SCHEDULER_H
#define PROJECTLABORATORY_SCHEDULER_H

#include <chrono>

namespace Framework {
    class Timer {
        std::chrono::microseconds limit{0};
        std::chrono::steady_clock::time_point start;
    public:
        Timer() = default;
        void launch() {
            start = std::chrono::steady_clock::now();
        }

        void setLimitInMicros(const unsigned int microseconds) {
            limit = std::chrono::microseconds(microseconds);
        }

        [[nodiscard]] bool outOfTime() const {
            const auto end = std::chrono::steady_clock::now();
            const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            return elapsed > limit;
        }
    };
}

#endif //PROJECTLABORATORY_SCHEDULER_H