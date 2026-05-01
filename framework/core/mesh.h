#ifndef PROJECTLABORATORY_MESH_H
#define PROJECTLABORATORY_MESH_H

#include <stdexcept>
#include "material.h"

namespace Framework {
    class Mesh : public UniformSource {
    protected:
        Material* boundMaterial;
        void initDump() override { }
    public:
        Mesh(ShaderProgram* program, Material* boundMaterial, const std::string& prefix = "mesh")
            : UniformSource(prefix + '.', {program})
        {
            if (!boundMaterial)
                throw std::invalid_argument("Invalid argument; expected material but found none.");
            this->boundMaterial = boundMaterial;
        }

        void draw(const ShaderProgram* provider) override {
            boundMaterial->draw(provider);
            UniformSource::draw(provider);
            this->draw();
        }

        void relink(const std::vector<ShaderProgram *> &programs) override {
            UniformSource::relink(programs);
            boundMaterial->relink(programs);
        }

        virtual void draw() = 0;
        virtual void createMesh() = 0;
    };
}

#endif //PROJECTLABORATORY_MESH_H