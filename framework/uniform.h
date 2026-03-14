#ifndef PROJECTLABORATORY_UNIFORM_H
#define PROJECTLABORATORY_UNIFORM_H

#include <string>
#include <variant>
#include <concepts>

#include "texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glad/glad.h"

using namespace std;
using namespace glm;

namespace Framework {
    using VecType = variant<vec2, vec3, vec4>;
    using PrimitiveType = variant<int, float, double, bool>;
    using MatType = variant<mat2, mat3, mat4>;

    template <typename T>
    concept PrimitiveTypeConcept =
            same_as<T, int> ||
            same_as<T, float> ||
            same_as<T, double> ||
            same_as<T, bool>;

    //NOTE: Prone to changes
    //TODO: Unified error logging
    class Uniform {
    protected:
        string name;
        GLint location;
        GLenum type;
        virtual void process(std::vector<PrimitiveType> primitives) = 0;
    public:
        Uniform(string& name, GLint location, GLenum type) :
            name(name), location(location), type(type) {}
        virtual bool acceptTextureUnit(const int& unit) = 0;
        virtual void set(MatType matrix) = 0;
        virtual void set(VecType vector) = 0;
        virtual void set(Texture* texture) = 0;
        template<PrimitiveTypeConcept... Args>
        requires(
            sizeof...(Args) >= 1 && sizeof...(Args) <= 4 &&
            ((same_as<Args, int> && ...) ||
            (same_as<Args, float> && ...) ||
            (same_as<Args, double> && ...) ||
            (same_as<Args, bool> && ...))
        )
        void set(Args... args) {
            process({args...});
        }
        virtual ~Uniform() = default;
    };


    template<int Size>
    requires(
        Size == 1 ||
        Size == 2 ||
        Size == 3 ||
        Size == 4
    )
    class UniformFloat : public Uniform {
        void commit(const float* pValues) {
            switch (type) {
                case GL_FLOAT:
                    glUniform1fv(location, 1, pValues);
                    break;
                case GL_FLOAT_VEC2:
                    glUniform2fv(location, 1, pValues);
                    break;
                case GL_FLOAT_VEC3:
                    glUniform3fv(location, 1, pValues);
                    break;
                case GL_FLOAT_VEC4:
                    glUniform4fv(location, 1, pValues);
                    break;
                default:
                    break;
            }
        }

        void process(std::vector<PrimitiveType> primitives) override {
            size_t N = primitives.size();
            if (N != Size)
                throw invalid_argument("Incorrect amount of parameters for uniform"
                                    " \"" + name + "\", expected " + to_string(Size) +
                                    " but found " + to_string(N) + ".");
            float values[Size];
            try {
                for (int i = 0; i < N; i++) {
                    values[i] = get<float>(primitives[i]);
                }
            }catch (exception &ex) {
                throw invalid_argument("Incorrect parameter type(s) for uniform"
                                       " \"" + name + "\", expected float(s), but found non-float(s).");
            }
            commit(values);
        }
    public:
        UniformFloat(string& name, GLint location, GLenum type) :
            Uniform(name, location, type) { }

        void set(MatType matrix) override {
            throw runtime_error("Incorrect parameter type for uniform"
                                " \"" + name + "\", expected float(s), but found matrix.");
        }

        void set(VecType vector) override {
            if (const auto v2 = get_if<vec2>(&vector)){
                Size == 2 ? commit(value_ptr(*v2)) :
                throw invalid_argument("Incorrect parameter type for uniform"
                                       " \"" + name + "\", expected vec" + to_string(Size) +
                                       ", but found vec2.");
            } else if (const auto v3 = get_if<vec3>(&vector)) {
                Size == 3 ? commit(value_ptr(*v3)) :
                throw invalid_argument("Incorrect parameter type for uniform"
                                       " \"" + name + "\", expected vec" + to_string(Size) +
                                       ", but found vec3.");
            } else if (const auto v4 = get_if<vec4>(&vector)) {
                Size == 4 ? commit(value_ptr(*v4)) :
                throw invalid_argument("Incorrect parameter type for uniform"
                                       " \"" + name + "\", expected vec" + to_string(Size) +
                                       ", but found vec4.");
            }
        }

        void set(Texture* texture) override {
            throw invalid_argument("Incorrect parameter type for uniform"
                                   " \"" + name + "\", it is a non-texture uniform.");
        }

        bool acceptTextureUnit(const int& unit) override{
            return false;
        }
    };


    //NOTE: Potential extension to matNxM
    template<int Size> /*Serves only error logging purposes for now*/
    requires(
        Size == 2 ||
        Size == 3 ||
        Size == 4
    )
    class UniformMatrixFloat : public Uniform {
        void commit(const float* mValues) {
            switch (type) {
                case GL_FLOAT_MAT2:
                    glUniformMatrix2fv(location, 1, GL_FALSE, mValues);
                    break;
                case GL_FLOAT_MAT3:
                    glUniformMatrix3fv(location, 1, GL_FALSE, mValues);
                    break;
                case GL_FLOAT_MAT4:
                    glUniformMatrix4fv(location, 1, GL_FALSE, mValues);
                    break;
                default:
                    break;
            }
        }

        void process(std::vector<PrimitiveType> primitives) override {
            throw invalid_argument("Incorrect parameter type for uniform"
                                   " \"" + name + "\", expected mat" + to_string(Size) +
                                   ", but found primitive type(s).");
        }
    public:
        UniformMatrixFloat(string& name, GLint location, GLenum type) :
            Uniform(name, location, type) { }

        void set(MatType matrix) override {
            if (const auto m2 = get_if<mat2>(&matrix)) {
                type == GL_FLOAT_MAT2 ? commit(value_ptr(*m2)) :
                throw invalid_argument("Incorrect parameter type for uniform"
                                       " \"" + name + "\", expected mat" + to_string(Size) +
                                       ", but found mat2.");
            } else if (const auto m3 = get_if<mat3>(&matrix)) {
                type == GL_FLOAT_MAT3 ? commit(value_ptr(*m3)) :
                throw invalid_argument("Incorrect parameter type for uniform"
                                       " \"" + name + "\", expected mat" + to_string(Size) +
                                       ", but found mat3.");
            } else if (const auto m4 = get_if<mat4>(&matrix)) {
                type == GL_FLOAT_MAT4 ? commit(value_ptr(*m4)) :
                throw invalid_argument("Incorrect parameter type for uniform"
                                       " \"" + name + "\", expected mat" + to_string(Size) +
                                       ", but found mat4.");
            }
        }

        void set(VecType vector) override {
            throw invalid_argument("Incorrect parameter type for uniform"
                                   " \"" + name + "\", expected mat" + to_string(Size) +
                                   ", but found vector.");
        }

        void set(Texture* texture) override {
            throw invalid_argument("Incorrect parameter type for uniform"
                                   " \"" + name + "\", it is a non-texture uniform.");
        }

        bool acceptTextureUnit(const int& unit) override {
            return false;
        }
    };


    class UniformSampler : public Uniform {
        int textureUnit;

        void process(std::vector<PrimitiveType> primitives) override {
            throw invalid_argument("Incorrect parameter type for uniform"
                                   " \"" + name + "\", expected Texture" +
                                   ", but found primitive type(s).");
        }
    public:
        UniformSampler(string& name, GLint location, GLenum type) :
            Uniform(name, location, type), textureUnit(0) { }

        void set(MatType matrix) override {
            throw invalid_argument("Incorrect parameter type for uniform"
                       " \"" + name + "\", expected Texture" +
                       ", but found matrix.");
        }

        void set(VecType vector) override {
            throw invalid_argument("Incorrect parameter type for uniform"
                       " \"" + name + "\", expected Texture" +
                       ", but found vector.");
        }

        void set(Texture* texture) override {
            glUniform1iv(location, 1, &textureUnit);
            texture->bindTexture(textureUnit);
        }

        bool acceptTextureUnit(const int& unit) override {
            textureUnit = unit;
            return true;
        }
    };
}

#endif //PROJECTLABORATORY_UNIFORM_H