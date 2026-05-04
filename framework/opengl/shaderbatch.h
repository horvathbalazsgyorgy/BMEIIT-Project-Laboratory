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
        const char* vcs;
        const char* fs;
        const char* gs;
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

            for (const auto& [name, vcs, fs, gs] : properties) {
                batchIndices[name] = nShaders++;

                if (vcs && !fs) {
                    auto computeShader  = ShaderLoader::createAndCompileShader(GL_COMPUTE_SHADER, vcs);
                    batch.emplace_back(std::make_unique<ShaderProgram>(computeShader));
                }else {
                    auto vertexShader   = ShaderLoader::createAndCompileShader(GL_VERTEX_SHADER,  vcs);
                    auto fragmentShader = ShaderLoader::createAndCompileShader(GL_FRAGMENT_SHADER, fs);
                    auto geometryShader = 0;
                    if (gs != nullptr) {
                        geometryShader = ShaderLoader::createAndCompileShader(GL_GEOMETRY_SHADER, gs);
                    }
                    batch.emplace_back(std::make_unique<ShaderProgram>(vertexShader, fragmentShader, geometryShader));
                }
            }
        }

        void executeOne(const std::string& name) const {
            (*this)[name]->execute();
        }

        void executeAll() const {
            for (const auto& program : batch) {
                program->execute();
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