#ifndef PROJECTLABORATORY_SHADERBATCH_H
#define PROJECTLABORATORY_SHADERBATCH_H

#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include "shaderprogram.h"
#include "loader/shaderloader.h"

namespace Framework {
    struct ShaderProperty {
        const char* name;
        const char* vs;
        const char* fs;
    };

    class ShaderBatch {
    protected:
        int nShaders;
        std::vector<std::unique_ptr<ShaderProgram>> batch;
        std::unordered_map<std::string, int> batchIndices;
    public:
        ShaderBatch(const std::vector<ShaderProperty>& properties) : nShaders(0) {
            if (properties.empty())
                throw std::invalid_argument("Invalid argument; expected at least one shader defining property,"
                                            " but none were provided.");

            for (const auto& [name, vs, fs] : properties) {
                batchIndices[name] = nShaders++;

                auto vertexShader   = ShaderLoader::createAndCompileShader(GL_VERTEX_SHADER,   vs);
                auto fragmentShader = ShaderLoader::createAndCompileShader(GL_FRAGMENT_SHADER, fs);
                batch.emplace_back(std::make_unique<ShaderProgram>(vertexShader, fragmentShader));
            }
        }

        void executeOne(const std::string& name) const {
            (*this)[name]->notify();
        }

        void executeAll() const {
            for (const auto& program : batch) {
                program->notify();
            }
        }

        ShaderProgram* operator[](const std::string& name) const {
            if (const auto it = batchIndices.find(name); it != batchIndices.end()) {
                return batch[it->second].get();
            }
            throw std::invalid_argument("Invalid argument; shader program \"" + name + "\" does not exist.");
        }
    };
}

#endif //PROJECTLABORATORY_SHADERBATCH_H