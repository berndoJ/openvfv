#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include "main.h"
#include "drawhelper.hpp"
#include "geometry.hpp"
#include "vectorfield.hpp"
#include "renderer.hpp"

static void glfw_err_callback(int error, const char *desc);

OpenVFVRenderer::OpenVFVRenderer()
{
    this->window_opended = false;

    this->draw_polygons = 10;
    this->draw_vector_radius = 0.0025;
    this->draw_vector_head_radius = this->draw_vector_radius * 2;
    this->draw_vector_head_size = 0.02;

    std::cout << "VFV renderer created." << std::endl;
}

OpenVFVRenderer::~OpenVFVRenderer()
{
    if (this->window_opended)
    {
        glfwDestroyWindow(this->window_handle);
	    glfwTerminate();
    }

    std::cout << "VFV renderer closed." << std::endl;
}

int OpenVFVRenderer::open_window(const char *title)
{
    // Cannot open window if already opened...
    if (this->window_opended)
    {
        return -1;
    }

    if (!glfwInit())
	{
		std::cerr << "GLFW init failed." << std::endl;
		return -1;
	}

	glfwSetErrorCallback(glfw_err_callback);

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_SAMPLES, 4);

	this->window_handle = glfwCreateWindow(800, 800, title, NULL, NULL);
	if (!window_handle)
	{
        std::cerr << "GLFW window init failed." << std::endl;
        return -1;
	}

	glfwMakeContextCurrent(window_handle);

	glfwSwapInterval(0);

    GLenum glew_init_rc = glewInit();

    if (glew_init_rc != GLEW_OK)
    {
        std::cerr << "Failed to init GLEW.\nError: " << glewGetErrorString(glew_init_rc) << std::endl;
        return -1;
    }

	this->_gl_set_global_opts();

    this->window_opended = true;

    std::cout << "VFV renderer opened window \"" << title << "\"." << std::endl;
    std::cout << "Using OpenGL " << glGetString(GL_VERSION)
        << ", GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION)
        << ", GLEW " << glewGetString(GLEW_VERSION) << std::endl;

    return 0;
}

void OpenVFVRenderer::do_render(void)
{
    if (!this->window_opended) return;

    // Debug:
    this->_handle_window_reshape();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    this->camera.apply_transform();
    glPushMatrix();    
    //display(this->window_handle);

    // Draw axis.
    /*glColor3f(1, 0, 0);
    OpenVFVDrawHelper::draw_vector(glm::vec3(0, 0, 0), glm::vec3(0.1, 0, 0),
        this->draw_vector_radius, this->draw_vector_head_radius, this->draw_vector_head_size, this->draw_polygons);
    glColor3f(0, 1, 0);
    OpenVFVDrawHelper::draw_vector(glm::vec3(0, 0, 0), glm::vec3(0, 0.1, 0),
        this->draw_vector_radius, this->draw_vector_head_radius, this->draw_vector_head_size, this->draw_polygons);
    glColor3f(0, 0, 1);
    OpenVFVDrawHelper::draw_vector(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0.1),
        this->draw_vector_radius, this->draw_vector_head_radius, this->draw_vector_head_size, this->draw_polygons);*/

    // DBG START

    // Generate geometry.
    const int geometry_polys = 30;
    const double radius = 0.3;
    const double delta_theta = 2 * M_PI / geometry_polys;
    OpenVFV::GeometryQuantizedObject **geometry_objs = new OpenVFV::GeometryQuantizedObject*[geometry_polys];
    for (int i = 0; i < geometry_polys; i++)
    {
        double theta = i * delta_theta;
        glm::vec3 base = (float)radius * glm::vec3(cos(theta), sin(theta), 0);
        glm::vec3 vector = ((float) radius * glm::vec3(cos(theta + delta_theta), sin(theta + delta_theta), 0)) - base;
        geometry_objs[i] = new OpenVFV::GeometryQuantizedObject(base, vector);
    }
    OpenVFV::Geometry *geometry = new OpenVFV::Geometry(geometry_objs, geometry_polys);

    // Render geometry.
    for (int i = 0; i < geometry->geometries_length; i++)
    {
        OpenVFV::GeometryQuantizedObject *gqobj = geometry->geometries[i];
        
        glColor3d(0.8, 0.8, 0.8);
        OpenVFVDrawHelper::draw_cylinder(gqobj->position, gqobj->vector, 0.001, 10);
    }

    OpenVFV::VFMagneticVectorPotential v_A_field;
    OpenVFV::VFMagneticFluxDensity v_B_field;

    const int steps = 10;
    const double delta = 0.07;

    /*
    glm::vec3 pos_vect = glm::vec3(0.5, 0.2, 0.6);
    glm::vec3 field_vect = vfield.compute(geometry, pos_vect);
    */

    
    for (int i = -steps; i <= steps; i++) // x
	{
		for (int j = -0; j <= 0; j++) // z
		{
			for (int k = -steps; k <= steps; k++) // y
			{
                glm::vec3 pos_vect = glm::vec3(i * delta, k * delta, j * delta);
                glm::vec3 field_vect = v_A_field.compute(geometry, pos_vect);

				double fieldstrength = glm::length(field_vect);
				double red_strength = (atan(fieldstrength) / M_PI) + 0.5;
				double green_strength = exp(-pow(fieldstrength, 2));
				double blue_strength = (atan(-fieldstrength) / M_PI) + 0.5;
				glColor3d(red_strength, green_strength, blue_strength);

				OpenVFVDrawHelper::draw_vector(pos_vect, 0.6f * (float)delta * glm::normalize(field_vect),
                    this->draw_vector_radius, this->draw_vector_head_radius, this->draw_vector_head_size, this->draw_polygons);
			}
		}
	}

    for (int i = -steps; i <= steps; i++) // x
	{
		for (int j = -steps; j <= steps; j++) // z
		{
			for (int k = -0; k <= 0; k++) // y
			{
                glm::vec3 pos_vect = glm::vec3(i * delta, k * delta, j * delta);
                glm::vec3 field_vect = v_B_field.compute(geometry, pos_vect);

				double fieldstrength = glm::length(field_vect);
				double red_strength = (atan(fieldstrength) / M_PI) + 0.5;
				double green_strength = exp(-pow(fieldstrength, 2));
				double blue_strength = (atan(-fieldstrength) / M_PI) + 0.5;
				glColor3d(blue_strength, green_strength, red_strength);

				OpenVFVDrawHelper::draw_vector(pos_vect, 0.6f * (float)delta * glm::normalize(field_vect),
                    this->draw_vector_radius, this->draw_vector_head_radius, this->draw_vector_head_size, this->draw_polygons);
			}
		}
	}

    // DBG END

    glPopMatrix();
    glFlush();
    

    glfwSwapBuffers(this->window_handle);
    glfwPollEvents();
}

bool OpenVFVRenderer::should_window_close(void)
{
    return glfwWindowShouldClose(this->window_handle);
}

void OpenVFVRenderer::_handle_window_reshape(void)
{
    int width, height;

	glfwGetFramebufferSize(this->window_handle, &width, &height);

	GLdouble window_aspect = (GLdouble)width / (GLdouble)height;

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-window_aspect, window_aspect, -1.0, 1.0, 1.0, 100.0);
}

void OpenVFVRenderer::_gl_set_global_opts(void)
{
    // Enable Multisampling.
    glEnable(GL_MULTISAMPLE);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

	GLint iMultiSample = 0;
	GLint iNumSamples = 0;
	glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
	glGetIntegerv(GL_SAMPLES, &iNumSamples);
    std::cout << "MSAA on, GL_SAMPLE_BUFFERS = " << iMultiSample << ", GL_SAMPLES = " << iNumSamples << std::endl;

    // Set clearing parameters.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0);

    // Enable Depth Test.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void OpenVFVRenderer::set_draw_poly_resolution(unsigned int polygons)
{
    if (polygons < 6) return;
    this->draw_polygons = polygons;
}

void glfw_err_callback(int error, const char *desc)
{
    std::cerr << "Error: (" << error << "): " << desc << std::endl;
}