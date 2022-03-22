#ifndef __OPENVFV_GEOMETRY_HPP
#define __OPENVFV_GEOMETRY_HPP

#include <glm/glm.hpp>

namespace OpenVFV
{

class GeometryQuantizedObject
{
    public:
        glm::vec3 position;
        glm::vec3 vector;

        GeometryQuantizedObject(glm::vec3 position, glm::vec3 vector);
};

class Geometry
{
    public:
        GeometryQuantizedObject **geometries;
        size_t geometries_length;

        Geometry(GeometryQuantizedObject **geometries, size_t geometries_length);
};


}

#endif