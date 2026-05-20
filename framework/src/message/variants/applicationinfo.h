#ifndef PROJECTLABORATORY_APPLICATIONINFO_H
#define PROJECTLABORATORY_APPLICATIONINFO_H

#include <iostream>
#include "../applicationmessage.h"

namespace Framework {
    class ApplicationInfo : public ApplicationMessage {
        static void Info(const std::string& message) {
            std::cout << message << std::endl;
        }
    public:
        ApplicationInfo() = delete;

        static void GeneralInformation(const std::string& message) {
            send(Message{message, Info, LOW_PRIORITY});
        }
    };
}

#endif //PROJECTLABORATORY_APPLICATIONINFO_H