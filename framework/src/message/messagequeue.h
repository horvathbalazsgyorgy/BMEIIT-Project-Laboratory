#ifndef PROJECTLABORATORY_MESSAGEQUEUE_H
#define PROJECTLABORATORY_MESSAGEQUEUE_H

#include <mutex>
#include <deque>
#include "../threading/timer.h"

namespace Framework {
    enum MessagePriority {
        HIGH_PRIORITY,
        LOW_PRIORITY
    };

    struct Message {
        std::string message;
        void(*context)(const std::string&){};
        MessagePriority priority;

        void emit() const {
            context(message);
        }
    };

    class MessageQueue {
        int priorityCounter{0};
        std::mutex mutex;
        std::deque<Message> messages;
        MessageQueue() = default;
    public:
        void enqueue(const Message& message) {
            if (message.priority == HIGH_PRIORITY) {
                {
                    std::scoped_lock lock(mutex);
                    messages.insert(messages.begin() + priorityCounter, message);
                }
                priorityCounter++;
            }else {
                {
                    std::scoped_lock lock(mutex);
                    messages.push_back(message);
                }
            }
        }

        void dequeue() {
            Message message;
            {
                std::scoped_lock lock(mutex);
                if (messages.empty())
                    return;

                message = messages.front();
                messages.pop_front();
            }
            message.emit();
        }

        static MessageQueue& getMessageQueue() {
            static MessageQueue queue;
            return queue;
        }
    };
}

#endif //PROJECTLABORATORY_MESSAGEQUEUE_H