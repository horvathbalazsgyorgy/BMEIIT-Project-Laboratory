#ifndef PROJECTLABORATORY_APPLICATIONMESSAGE_H
#define PROJECTLABORATORY_APPLICATIONMESSAGE_H

#include "messagequeue.h"

namespace Framework {
    class ApplicationMessage {
        ApplicationMessage() = default;
        static inline MessageQueue& messages = MessageQueue::getMessageQueue();
    protected:
        static void send(const Message& message) {
            messages.enqueue(message);
        }
    };
}

#endif //PROJECTLABORATORY_APPLICATIONMESSAGE_H