#ifndef PROJECTLABORATORY_MATERIAL_H
#define PROJECTLABORATORY_MATERIAL_H

#include <string>
#include "../uniform/uniformsource.h"

namespace Framework {
    class Uniform;

    class Material : public UniformSource {
    protected:
        void initDump() override { }
    public:
        Material(ShaderProgram* program, const std::string& prefix = "material")
            : UniformSource(prefix + '.', {program}) { }

        ~Material() override = default;
    };
}

#endif //PROJECTLABORATORY_MATERIAL_H