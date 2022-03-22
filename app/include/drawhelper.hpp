#ifndef __OPENVFV_DRAWHELPER_HPP
#define __OPENVFV_DRAWHELPER_HPP

#include <glm/glm.hpp>

namespace OpenVFVDrawHelper
{
    void draw_cylinder(glm::vec3 base, glm::vec3 axis, double radius, unsigned int polygons);
    void draw_cone(glm::vec3 base, glm::vec3 axis, double radius, unsigned int polygons);
    void draw_vector(glm::vec3 base, glm::vec3 vector, double radius, double head_radius, double head_length, unsigned int polygons);
}

#endif