
#include "geometry.hpp"

OpenVFV::GeometryQuantizedObject::GeometryQuantizedObject(glm::vec3 position, glm::vec3 vector)
{
    this->position = position;
    this->vector = vector;
}

OpenVFV::Geometry::Geometry(GeometryQuantizedObject **geometries, size_t geometries_length)
{
    this->geometries = geometries;
    this->geometries_length = geometries_length;
}