#include <iostream>
#include <GLFW/glfw3.h>

#include "drawhelper.hpp"

static void _glvertex_vec3(glm::vec3 vector)
{
    glVertex3f(vector.x, vector.y, vector.z);
}

void OpenVFVDrawHelper::draw_cylinder(glm::vec3 base, glm::vec3 axis, double radius, unsigned int polygons)
{
    double delta_phi, theta;
    glm::vec3 rotaxis;
	double height;

    // If length == 0, nothing to draw.
	if (glm::length(axis) == 0)
	{
		return;
	}

    // Minimum 4 polygons.
    if (polygons < 4)
    {
        return;
    }

	delta_phi = 2 * M_PI / polygons; /* Polygonalized slice angle */
	height = glm::length(axis);
    theta = -acos(axis.y / height);
    rotaxis = glm::normalize(glm::cross(axis, glm::vec3(0.0f, 1.0f, 0.0f)));

	glPushMatrix();

	glTranslatef(base.x, base.y, base.z);
	if (!(axis.x == 0 && axis.z == 0)) /* Only rotate if cylvect !|| y axis */
		glRotatef(theta * (180.0 / M_PI), rotaxis.x, rotaxis.y, rotaxis.z);
	else if (axis.y < 0)
	{
		height = -height;
	}
	
	/* Draw cylinder oriented parallel to y-axis */

	/* Draw vertices for sides. */
	glBegin(GL_TRIANGLE_STRIP);
	for (unsigned int i = 0; i < polygons; i++)
	{
		double phi = delta_phi * i;
        glm::vec3 rvect = (float)radius * glm::vec3(cos(phi), 0, sin(phi));
        _glvertex_vec3(rvect);
        _glvertex_vec3(rvect + glm::vec3(0, height, 0));
	}
	glEnd();

	/* Draw two end circles. */
	glBegin(GL_POLYGON);
	for (unsigned int i = 0; i < polygons; i++)
	{
		double phi = delta_phi * i;
        glm::vec3 rvect = (float)radius * glm::vec3(cos(phi), 0, sin(phi));
        _glvertex_vec3(rvect);
	}
	glEnd();

	glBegin(GL_POLYGON);
	for (unsigned int i = 0; i < polygons; i++)
	{
		double phi = delta_phi * i;
        glm::vec3 rvect = (float)radius * glm::vec3(cos(phi), height, sin(phi));
        _glvertex_vec3(rvect);
	}
	glEnd();

	glPopMatrix();
}

void OpenVFVDrawHelper::draw_cone(glm::vec3 base, glm::vec3 axis, double radius, unsigned int polygons)
{
    double delta_phi, theta;
    glm::vec3 rotaxis;
	double height;

	// If length == 0, nothing to draw.
	if (glm::length(axis) == 0)
	{
		return;
	}

    // Minimum 4 polygons.
    if (polygons < 4)
    {
        return;
    }

	delta_phi = 2 * M_PI / polygons; /* Polygonalized slice angle */
	height = glm::length(axis);
	theta = -acos(axis.y / height);
    rotaxis = glm::normalize(glm::cross(axis, glm::vec3(0.0f, 1.0f, 0.0f)));

	glPushMatrix();

	glTranslated(base.x, base.y, base.z);
	if (!(axis.x == 0 && axis.z == 0)) /* Only rotate if vect !|| y axis */
		glRotated(theta * (180.0 / M_PI), rotaxis.x, rotaxis.y, rotaxis.z);
	else if (axis.y < 0)
	{
		height = -height;
	}

	/* Draw cone */
	glBegin(GL_TRIANGLE_STRIP);
	for (unsigned int i = 0; i < polygons; i++)
	{
		double phi = delta_phi * i;
        glm::vec3 rvect = (float)radius * glm::vec3(cos(phi), 0, sin(phi));
        _glvertex_vec3(rvect);
        _glvertex_vec3(glm::vec3(0, height, 0));
	}
	glEnd();

	/* Draw base circle */
	glBegin(GL_POLYGON);
	for (unsigned int i = 0; i < polygons; i++)
	{
		double phi = delta_phi * i;
        glm::vec3 rvect = (float)radius * glm::vec3(cos(phi), 0, sin(phi));
        _glvertex_vec3(rvect);
	}
	glEnd();

	glPopMatrix();
}

void OpenVFVDrawHelper::draw_vector(glm::vec3 base, glm::vec3 vector, double radius, double head_radius, double head_length, unsigned int polygons)
{
    glm::vec3 head_axis = ((float)head_length * glm::normalize(vector));
    glm::vec3 shaft_axis = vector - head_axis;

    OpenVFVDrawHelper::draw_cylinder(base, shaft_axis, radius, polygons);
    OpenVFVDrawHelper::draw_cone(base + shaft_axis, head_axis, head_radius, polygons);
}