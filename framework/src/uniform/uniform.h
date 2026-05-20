#ifndef PROJECTLABORATORY_UNIFORM_H
#define PROJECTLABORATORY_UNIFORM_H

#include <string>
#include <utility>
#include <vector>
#include <variant>
#include <concepts>

#include "../message/variants/applicationerror.h"
#include "../opengl/loader/texture.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Framework {
    using VecType = std::variant<glm::vec2, glm::vec3, glm::vec4, glm::ivec2, glm::ivec3, glm::ivec4>;
    using PrimitiveType = std::variant<int, float, double, bool>;
    using MatType = std::variant<glm::mat2, glm::mat3, glm::mat4>;

    template <typename T>
    concept PrimitiveTypeConcept =
            std::same_as<T, int> ||
            std::same_as<T, float> ||
            std::same_as<T, double> ||
            std::same_as<T, bool>;

    //NOTE: Prone to changes
    //TODO: Unified error logging
    class Uniform {
    protected:
        std::string name;
        GLint location;
        GLenum type;
        virtual void process(std::vector<PrimitiveType> primitives) = 0;
    public:
        Uniform(std::string  name, const GLint location, const GLenum type) :
            name(std::move(name)), location(location), type(type) {}
        virtual void set(MatType matrix) = 0;
        virtual void set(VecType vector) = 0;
        virtual void set(Texture* texture) = 0;
        template<PrimitiveTypeConcept... Args>
        requires(
            sizeof...(Args) >= 1 && sizeof...(Args) <= 4 &&
            ((std::same_as<Args, int> && ...) ||
            (std::same_as<Args, float> && ...) ||
            (std::same_as<Args, double> && ...) ||
            (std::same_as<Args, bool> && ...))
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
        void commit(const float* pValues) const {
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
            if (N != Size) {
                ApplicationError::UniformMismatch(name, std::to_string(Size) + " primitives", std::to_string(N));
                return;
            }

            float values[Size];
            try {
                for (int i = 0; i < N; i++) {
                    values[i] = std::get<float>(primitives[i]);
                }
            }catch (...) {
                ApplicationError::UniformMismatch(name, "float(s)", "non-float(s)");
            }
            commit(values);
        }
    public:
        UniformFloat(const std::string& name, const GLint location, const GLenum type) :
            Uniform(name, location, type) { }

        void set(MatType matrix) override {
            ApplicationError::UniformMismatch(name, "float(s)", "matrix");
        }

        void set(VecType vector) override {
            if (const auto v2 = std::get_if<glm::vec2>(&vector)){
                Size == 2 ? commit(glm::value_ptr(*v2)) :
                ApplicationError::UniformMismatch(name, "vec" + std::to_string(Size), "vec2");
            } else if (const auto v3 = std::get_if<glm::vec3>(&vector)) {
                Size == 3 ? commit(glm::value_ptr(*v3)) :
                ApplicationError::UniformMismatch(name, "vec" + std::to_string(Size), "vec3");
            } else if (const auto v4 = std::get_if<glm::vec4>(&vector)) {
                Size == 4 ? commit(glm::value_ptr(*v4)) :
                ApplicationError::UniformMismatch(name, "vec" + std::to_string(Size), "vec4");
            } else {
                ApplicationError::UniformMismatch(name, "a vector of floats", "otherwise");
            }
        }

        void set(Texture* texture) override {
            ApplicationError::UniformMismatch(name, "non-texture", "texture");
        }
    };


    template<int Size>
    requires(
        Size == 1 ||
        Size == 2 ||
        Size == 3 ||
        Size == 4
    )
    class UniformInt : public Uniform {
        void commit(const int* pValues) const {
            switch (type) {
                case GL_INT:
                    glUniform1iv(location, 1, pValues);
                    break;
                case GL_INT_VEC2:
                    glUniform2iv(location, 1, pValues);
                    break;
                case GL_INT_VEC3:
                    glUniform3iv(location, 1, pValues);
                    break;
                case GL_INT_VEC4:
                    glUniform4iv(location, 1, pValues);
                    break;
                default:
                    break;
            }
        }

        void process(std::vector<PrimitiveType> primitives) override {
            size_t N = primitives.size();
            if (N != Size) {
                ApplicationError::UniformMismatch(name, std::to_string(Size) + " primitives", std::to_string(N));
                return;
            }

            int values[Size];
            try {
                for (int i = 0; i < N; i++) {
                    values[i] = std::get<int>(primitives[i]);
                }
            }catch (...) {
                ApplicationError::UniformMismatch(name, "int(s)", "non-int(s)");
            }
            commit(values);
        }
    public:
        UniformInt(const std::string& name, const GLint location, const GLenum type) :
            Uniform(name, location, type) { }

        void set(MatType matrix) override {
            ApplicationError::UniformMismatch(name, "int(s)", "matrix");
        }

        void set(VecType vector) override {
            if (const auto v2 = std::get_if<glm::ivec2>(&vector)){
                Size == 2 ? commit(glm::value_ptr(*v2)) :
                ApplicationError::UniformMismatch(name, "ivec" + std::to_string(Size), "ivec2");
            } else if (const auto v3 = std::get_if<glm::ivec3>(&vector)) {
                Size == 3 ? commit(glm::value_ptr(*v3)) :
                ApplicationError::UniformMismatch(name, "ivec" + std::to_string(Size), "ivec3");
            } else if (const auto v4 = std::get_if<glm::ivec4>(&vector)) {
                Size == 4 ? commit(glm::value_ptr(*v4)) :
                ApplicationError::UniformMismatch(name, "ivec" + std::to_string(Size), "ivec4");
            } else {
                ApplicationError::UniformMismatch(name, "a vector of ints", "otherwise");
            }
        }

        void set(Texture* texture) override {
            ApplicationError::UniformMismatch(name, "non-texture", "texture");
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
        void commit(const float* mValues) const {
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
            ApplicationError::UniformMismatch(name, "mat" + std::to_string(Size), "primitive type(s)");
        }
    public:
        UniformMatrixFloat(const std::string& name, const GLint location, const GLenum type) :
            Uniform(name, location, type) { }

        void set(MatType matrix) override {
            if (const auto m2 = std::get_if<glm::mat2>(&matrix)) {
                type == GL_FLOAT_MAT2 ? commit(glm::value_ptr(*m2)) :
                ApplicationError::UniformMismatch(name, "mat" + std::to_string(Size), "mat2");
            } else if (const auto m3 = std::get_if<glm::mat3>(&matrix)) {
                type == GL_FLOAT_MAT3 ? commit(glm::value_ptr(*m3)) :
                ApplicationError::UniformMismatch(name, "mat" + std::to_string(Size), "mat3");
            } else if (const auto m4 = std::get_if<glm::mat4>(&matrix)) {
                type == GL_FLOAT_MAT4 ? commit(glm::value_ptr(*m4)) :
                ApplicationError::UniformMismatch(name, "mat" + std::to_string(Size), "mat4");
            }
        }

        void set(VecType vector) override {
            ApplicationError::UniformMismatch(name, "mat" + std::to_string(Size), "vector");
        }

        void set(Texture* texture) override {
            ApplicationError::UniformMismatch(name, "mat" + std::to_string(Size), "texture");
        }
    };


    class UniformSampler : public Uniform {
        int textureUnit;

        void process(std::vector<PrimitiveType> primitives) override {
            ApplicationError::UniformMismatch(name, "Texture", "primitive type(s)");
        }
    public:
        UniformSampler(const std::string& name, const GLint location, const GLenum type, const int textureUnit) :
            Uniform(name, location, type), textureUnit(textureUnit) { }

        void set(MatType matrix) override {
            ApplicationError::UniformMismatch(name, "Texture", "matrix");
        }

        void set(VecType vector) override {
            ApplicationError::UniformMismatch(name, "Texture", "vector");
        }

        void set(Texture* texture) override {
            glUniform1iv(location, 1, &textureUnit);
            texture->bindTexture(textureUnit);
        }
    };
}

#endif //PROJECTLABORATORY_UNIFORM_H