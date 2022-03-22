#ifndef __OPENVFV_VECTORFIELD_HPP
#define __OPENVFV_VECTORFIELD_HPP

#include <glm/glm.hpp>
#include "geometry.hpp"

namespace OpenVFV
{

class VectorField
{
    public:
        virtual glm::vec3 compute(OpenVFV::Geometry *geometry, glm::vec3 vect) = 0;
};

class VFMagneticFluxDensity : public OpenVFV::VectorField
{
    public:
        glm::vec3 compute(OpenVFV::Geometry *geometry, glm::vec3 vect);
};

class VFMagneticVectorPotential : public OpenVFV::VectorField
{
    public:
        glm::vec3 compute(OpenVFV::Geometry *geometry, glm::vec3 vect);
};

}

#endif