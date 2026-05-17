#ifndef PROJECTLABORATORY_APPLICATIONERROR_H
#define PROJECTLABORATORY_APPLICATIONERROR_H

#include <iostream>
#include "../../opengl/application.h"
#include "../applicationmessage.h"

namespace Framework {
    class ApplicationError : public ApplicationMessage {
        static void Fatal(const std::string& message) {
            std::cout << "The program entered into a non-recoverable or corrupted state." << std::endl;
            std::cout << "Reason: " << message << std::endl;
            std::cout << "Exiting..." << std::endl;
            GLApplication::shutdown();
        }
    public:
        ApplicationError() = delete;

        static void FileNotFound(const std::string& what, const std::string& path) {
            const std::string message = "Failed to load " + what + " file at the following path: " + path + ". The file may be missing.";
            send(Message{message, Fatal, HIGH_PRIORITY});
        }

        static void GeneralConfigurationFailure(const std::string& action, const std::string& what, const std::string& where = "", const std::string& details = "") {
            const std::string more = details.empty() ? "not provided" : details;
            const std::string location = where.empty() ? "" : " (" + where + ")";
            const std::string message = "Unable to " + action + " " + what + location + ".\nDetails: " + more;
            send(Message{message, Fatal, HIGH_PRIORITY});
        }

        static void MissingComponent(const std::string& where, const std::string& missing) {
            const std::string message = "Missing component in " + where + "; expected " + missing + ", but it was not provided.";
            send(Message{message, Fatal, HIGH_PRIORITY});
        }

        static void ComponentNotSupported(const std::string& where, const std::string& what) {
            const std::string message = "Unsupported " + where + " type. The following component is not supported: " + what + ".";
            send(Message{message, Fatal, HIGH_PRIORITY});
        }

        static void UniformMismatch(const std::string& name, const std::string& expected, const std::string& actual) {
            const std::string message = "Incorrect parameter type for uniform"
                                    " \"" + name + "\"; expected " + expected +
                                    ", but found " + actual + ".";
            send(Message{message, Fatal, HIGH_PRIORITY});
        }
    };
}

#endif //PROJECTLABORATORY_APPLICATIONERROR_H