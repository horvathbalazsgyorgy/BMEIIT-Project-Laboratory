#ifndef PROJECTLABORATORY_UNIFORMSOURCE_H
#define PROJECTLABORATORY_UNIFORMSOURCE_H

#include <string>

namespace Framework {
    class Uniform;

    class UniformSource {
    protected:
        std::string glslPrefix;
    public:
        UniformSource(const std::string &prefix) : glslPrefix(prefix) { }
        virtual Uniform* operator[](const std::string& name) const = 0;
        virtual ~UniformSource() = default;
    };
}

#endif //PROJECTLABORATORY_UNIFORMSOURCE_H