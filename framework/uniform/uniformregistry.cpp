#include "uniformregistry.h"

#include <vector>
#include "uniform.h"

namespace {
    struct UniformFactory {
        static std::unique_ptr<Framework::Uniform> create(std::string& name, GLint location, GLenum type) {
            if (type == GL_FLOAT)
                return std::make_unique<Framework::UniformFloat<1>>(name, location, type);
            if (type == GL_FLOAT_VEC2)
                return std::make_unique<Framework::UniformFloat<2>>(name, location, type);
            if (type == GL_FLOAT_VEC3)
                return std::make_unique<Framework::UniformFloat<3>>(name, location, type);
            if (type == GL_FLOAT_VEC4)
                return std::make_unique<Framework::UniformFloat<4>>(name, location, type);
            if (type == GL_FLOAT_MAT2)
                return std::make_unique<Framework::UniformMatrixFloat<2>>(name, location, type);
            if (type == GL_FLOAT_MAT3)
                return std::make_unique<Framework::UniformMatrixFloat<3>>(name, location, type);
            if (type == GL_FLOAT_MAT4)
                return std::make_unique<Framework::UniformMatrixFloat<4>>(name, location, type);
            if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE)
                return std::make_unique<Framework::UniformSampler>(name, location, type);
            throw std::runtime_error("Unsupported uniform type: " + std::to_string(type));
        }
    };
}

namespace Framework {
    void UniformRegistry::gatherUniforms(GLuint program) {
        GLint count = 0;
        glGetProgramInterfaceiv(
            program,
            GL_UNIFORM,
            GL_ACTIVE_RESOURCES,
            &count);

        GLenum props[] = {
            GL_NAME_LENGTH,
            GL_LOCATION,
            GL_TYPE
        };

        for (GLuint i = 0; i < (GLuint)count; i++) {
            GLint values[3];
            glGetProgramResourceiv(
                program,
                GL_UNIFORM,
                i, 3, props, 3,
                nullptr,
                values);

            if (values[1] == -1) continue;

            std::vector<char> nameBuffer(values[0]);
            glGetProgramResourceName(
                program,
                GL_UNIFORM,
                i,
                values[0],
                nullptr,
                nameBuffer.data());

            std::string name(nameBuffer.data());
            uniforms[name] = UniformFactory::create(name, values[1], values[2]);
        }
    }

    Uniform* UniformRegistry::query(const std::string& name) {
        auto it = uniforms.find(name);
        if (it != uniforms.end()) {
            Uniform* uniform = it->second.get();
            return uniform;
        }
        return nullptr;
    }
}