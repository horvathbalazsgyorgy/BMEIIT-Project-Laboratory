#ifndef PROJECTLABORATORY_MATERIAL_H
#define PROJECTLABORATORY_MATERIAL_H

#include <string>
#include "../uniform/uniformsource.h"
#include "../opengl/shaderprogram.h"

namespace Framework {
    class Uniform;

    class Material : public UniformSource {
        ShaderProgram* program;

        void initDump() override { }
    public:
        Material(ShaderProgram* program, const std::string& prefix = "material")
            : UniformSource(prefix, {program}), program(program) { }

        virtual void draw() {
            program->useShaderProgram(this);
        }

        ~Material() override = default;
    };
}

#endif //PROJECTLABORATORY_MATERIAL_H