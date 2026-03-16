#ifndef PROJECTLABORATORY_MATERIAL_H
#define PROJECTLABORATORY_MATERIAL_H

#include <string>
#include "../uniform/uniformsource.h"

namespace Framework {
    class ShaderProgram;
    class Uniform;

    class Material : public UniformSource {
        ShaderProgram* program;
    public:
        Material(ShaderProgram* program, const std::string& prefix = "material");
        virtual void draw();
        Uniform* operator[](const std::string& name) const override;
        ~Material() override = default;
    };
}

#endif //PROJECTLABORATORY_MATERIAL_H