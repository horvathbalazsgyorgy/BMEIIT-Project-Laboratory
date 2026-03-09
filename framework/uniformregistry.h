#ifndef PROJECTLABORATORY_UNIFORMREGISTRY_H
#define PROJECTLABORATORY_UNIFORMREGISTRY_H

#include <memory>
#include <unordered_map>
#include <string>
#include "uniform.h"

using namespace std;

namespace Framework {
    class UniformFactory {
    public:
        static unique_ptr<Uniform> create(string& name, GLint location, GLenum type) {
            if (type == GL_FLOAT)
                return make_unique<UniformFloat<1>>(name, location, type);
            if (type == GL_FLOAT_VEC2)
                return make_unique<UniformFloat<2>>(name, location, type);
            if (type == GL_FLOAT_VEC3)
                return make_unique<UniformFloat<3>>(name, location, type);
            if (type == GL_FLOAT_VEC4)
                return make_unique<UniformFloat<4>>(name, location, type);
            if (type == GL_FLOAT_MAT2)
                return make_unique<UniformMatrixFloat<2>>(name, location, type);
            if (type == GL_FLOAT_MAT3)
                return make_unique<UniformMatrixFloat<3>>(name, location, type);
            if (type == GL_FLOAT_MAT4)
                return make_unique<UniformMatrixFloat<4>>(name, location, type);
            if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE)
                return make_unique<UniformSampler>(name, location, type);
            throw runtime_error("Unsupported uniform type: " + to_string(type));
        }
    };

    class UniformRegistry {
        std::unordered_map<string, unique_ptr<Uniform>> uniforms;
    public:
        void gatherUniforms(GLuint program) {
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

                std::vector<char> nameBuffer(values[0]);
                glGetProgramResourceName(
                    program,
                    GL_UNIFORM,
                    i,
                    values[0],
                    nullptr,
                    nameBuffer.data());

                string name(nameBuffer.data());
                uniforms[name] = UniformFactory::create(name, values[1], values[2]);
            }
        }

        Uniform* query(const string& name) {
            auto it = uniforms.find(name);
            if (it != uniforms.end()) {
                Uniform* uniform = it->second.get();
                return uniform;
            }
            throw runtime_error("Uniform \"" + name + "\" not found or is not in use.");
        }
    };
}

#endif //PROJECTLABORATORY_UNIFORMREGISTRY_H