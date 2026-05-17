#ifndef PROJECTLABORATORY_APPLICATIONWARNING_H
#define PROJECTLABORATORY_APPLICATIONWARNING_H

#include <iostream>
#include "../applicationmessage.h"

namespace Framework {
    class ApplicationWarning : public ApplicationMessage {
        static void Warning(const std::string& message) {
            std::cout << "The program ran into a minor error."
                         " Normal execution may or may not be disturbed." << std::endl;
            std::cout << "Reason: " << message << std::endl;
            std::cout << "Continue..." << std::endl;
        }
    public:
        ApplicationWarning() = delete;

        static void MissingValue(const std::string& where, const std::string& missing) {
            const std::string message = "Missing value in " + where + "; the following is missing: " + missing + ". Fallback value is used.";
            send(Message{message, Warning, LOW_PRIORITY});
        }

        static void DispatchFailure(const GLuint where) {
            const std::string message = "Unable to dispatch shader (program ID: " + std::to_string(where) + "). It may not be a compute shader.";
            send(Message{message, Warning, LOW_PRIORITY});
        }

        static void ComponentMismatch(const std::string& where, const std::string& one, const std::string& other) {
            const std::string message = "Mismatched components in " + where + ". The following do not go well together: " + one + " and " + other + ".";
            send(Message{message, Warning, LOW_PRIORITY});
        }
    };
}

#endif //PROJECTLABORATORY_APPLICATIONWARNING_H