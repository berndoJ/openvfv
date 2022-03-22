#include "vectorfield.hpp"
#include <math.h>

#include "drawhelper.hpp"
#include <GLFW/glfw3.h>

glm::vec3 OpenVFV::VFMagneticFluxDensity::compute(OpenVFV::Geometry *geometry, glm::vec3 vect)
{
    const double source_current = 15.0;
    glm::vec3 ret = glm::vec3(0, 0, 0);

    /* B-field */
	/*
		B = [mu_0 * I / (4 * pi * rho)] * [sin(alpha_2) - sin(alpha_1)] e_B
	*/
    for (int i = 0; i < geometry->geometries_length; i++)
    {
        OpenVFV::GeometryQuantizedObject *gobj = geometry->geometries[i];

        glm::vec3 vect_base = gobj->position - vect;
        glm::vec3 vect_end = vect_base + gobj->vector;
        glm::vec3 vect_e_B = glm::normalize(glm::cross(gobj->vector, -1.0f * vect_base));

        double scalar_n = - glm::dot(vect_base, gobj->vector) / pow(glm::length(gobj->vector), 2);
        glm::vec3 vect_n = vect_base + ((float)scalar_n * gobj->vector);

        double alpha_1 = acos(( glm::dot(vect_n, vect_base) ) / ( glm::length(vect_n) * glm::length(vect_base) ));
        double alpha_2 = acos(( glm::dot(vect_n, vect_end) ) / ( glm::length(vect_n) * glm::length(vect_end) ));

        if (scalar_n > 0)
        {
            alpha_1 *= -1;
            alpha_2 *= -1;
        }

        double scalar_B = ( source_current / (4 * M_PI * glm::length(vect_base)) ) * ( sin(alpha_2) - sin(alpha_1) );
        ret = ret + ((float)scalar_B * vect_e_B);

    /*
        glColor3d(0, 1, 0);
        OpenVFVDrawHelper::draw_vector(vect, vect_end, 0.0025, 0.005, 0.02, 10);
        glColor3d(1, 0, 0);
        OpenVFVDrawHelper::draw_vector(vect, vect_base, 0.0025, 0.005, 0.02, 10);
        OpenVFVDrawHelper::draw_vector(vect + vect_base, gobj->vector, 0.0025, 0.005, 0.02, 10);
        glColor3d(0, 0.6, 0.6);
        OpenVFVDrawHelper::draw_vector(vect, vect_n, 0.0025, 0.005, 0.02, 10);
        glColor3d(0, 0, 1);
        OpenVFVDrawHelper::draw_vector(vect, ret, 0.0025, 0.005, 0.02, 10);
    */
    }

    return ret;
}

glm::vec3 OpenVFV::VFMagneticVectorPotential::compute(OpenVFV::Geometry *geometry, glm::vec3 vect)
{
    const double source_current = 15.0;
    glm::vec3 ret = glm::vec3(0, 0, 0);

    /* A-Field */
    /*
        A = mu_0 * I / (4 * pi) * ln sqrt( (1-sin(alpha_1))/(1+sin(alpha_1)) * (1+sin(alpha_2))/(1-sin(alpha_2)) ) e_I
    */
    for (int i = 0; i < geometry->geometries_length; i++)
    {
        OpenVFV::GeometryQuantizedObject *gobj = geometry->geometries[i];

        glm::vec3 vect_base = gobj->position - vect;
        glm::vec3 vect_end = vect_base + gobj->vector;
        glm::vec3 vect_e_I = glm::normalize(gobj->vector);

        double scalar_n = - glm::dot(vect_base, gobj->vector) / pow(glm::length(gobj->vector), 2);
        glm::vec3 vect_n = vect_base + ((float)scalar_n * gobj->vector);

        double alpha_1 = acos(( glm::dot(vect_n, vect_base) ) / ( glm::length(vect_n) * glm::length(vect_base) ));
        double alpha_2 = acos(( glm::dot(vect_n, vect_end) ) / ( glm::length(vect_n) * glm::length(vect_end) ));

        if (scalar_n > 0)
        {
            alpha_1 *= -1;
            alpha_2 *= -1;
        }

        double scalar_A = ( source_current / (4 * M_PI) ) * log(sqrt( ( (1-sin(alpha_1))/(1+sin(alpha_2)) ) * ( (1+sin(alpha_2))/(1-sin(alpha_1)) ) ));


        double scalar_B = ( source_current / (4 * M_PI * glm::length(vect_base)) ) * ( sin(alpha_2) - sin(alpha_1) );
        ret = ret + ((float)scalar_B * vect_e_I);
    }

    return ret;
}