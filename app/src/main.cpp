#include "generated/autoconf.h"
#include "generated/version.h"

#include <stdio.h>
#include <math.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vectmath.h"
#include "renderer.hpp"
#include "main.h"

#define ARROW_SHAFT_SIZE	0.0025
#define ARROW_HEAD_SIZE		0.02
#define ARROW_POLYGONS		10
#define WIRE_DIAMETER		0.001

#define MU_0				4 * M_PI * 1e-7

void draw_vertex3(struct dvect3 vect);
void draw_cylinder(struct dvect3 v1, struct dvect3 v2, double radius, unsigned int poly);
void draw_cone(struct dvect3 base, struct dvect3 vect, double radius, unsigned int poly);
void draw_arrow(struct dvect3 base, struct dvect3 vect, double shaft_radius, double head_size, unsigned int poly);

void draw_vectorfield(void);

void draw_vertex3(struct dvect3 vect)
{
	glVertex3d(vect.x, vect.y, vect.z);
}

void draw_cylinder(struct dvect3 v1, struct dvect3 v2, double radius, unsigned int poly)
{
	double delta_phi, theta;
	struct dvect3 cylvect, rotaxis;
	double height;

	/* Minimum of poly is 6. */
	if (poly < 6) poly = 6;
	
	/* Check if start == end (if so, nothing to draw.) */
	if (dvect3_isequal(v1, v2))
	{
		return;
	}

	delta_phi = 2 * M_PI / poly; /* Polygonalized slice angle */
	cylvect = dvect3_sub(v2, v1);
	height = dvect3_length(cylvect);

	theta = -acos(cylvect.y / height);
	rotaxis = dvect3_normalize(dvect3_exmul(cylvect, dvect3_direct(0, 1, 0)));

	glPushMatrix();

	glTranslated(v1.x, v1.y, v1.z);
	if (!(cylvect.x == 0 && cylvect.z == 0)) /* Only rotate if cylvect !|| y axis */
		glRotated(theta * (180.0 / M_PI), rotaxis.x, rotaxis.y, rotaxis.z);
	else if (cylvect.y < 0)
	{
		height = -height;
	}
	
	/* Draw cylinder oriented parallel to y-axis */

	/* Draw vertices for sides. */
	glBegin(GL_TRIANGLE_STRIP);
	for (unsigned int i = 0; i < poly; i++)
	{
		double phi = delta_phi * i;
		struct dvect3 rvect = dvect3_smul(radius, dvect3_direct(cos(phi), 0, sin(phi)));
		draw_vertex3(rvect);
		draw_vertex3(dvect3_add(rvect, dvect3_direct(0, height, 0)));
	}
	glEnd();

	/* Draw two end circles. */
	glBegin(GL_POLYGON);
	for (unsigned int i = 0; i < poly; i++)
	{
		double phi = delta_phi * i;
		struct dvect3 rvect = dvect3_smul(radius, dvect3_direct(cos(phi), 0, sin(phi)));
		draw_vertex3(rvect);
	}
	glEnd();

	glBegin(GL_POLYGON);
	for (unsigned int i = 0; i < poly; i++)
	{
		double phi = delta_phi * i;
		struct dvect3 rvect = dvect3_smul(radius, dvect3_direct(cos(phi), height, sin(phi)));
		draw_vertex3(rvect);
	}
	glEnd();

	glPopMatrix();
}

void draw_cone(struct dvect3 base, struct dvect3 vect, double radius, unsigned int poly)
{
	double delta_phi, theta;
	struct dvect3 rotaxis;
	double height;

	if (dvect3_isnull(vect))
	{
		return;
	}

	/* Minimum of poly is 6. */
	if (poly < 6) poly = 6;

	delta_phi = 2 * M_PI / poly; /* Polygonalized slice angle */
	height = dvect3_length(vect);

	theta = -acos(vect.y / height);
	rotaxis = dvect3_normalize(dvect3_exmul(vect, dvect3_direct(0, 1, 0)));

	glPushMatrix();

	glTranslated(base.x, base.y, base.z);
	if (!(vect.x == 0 && vect.z == 0)) /* Only rotate if vect !|| y axis */
		glRotated(theta * (180.0 / M_PI), rotaxis.x, rotaxis.y, rotaxis.z);
	else if (vect.y < 0)
	{
		height = -height;
	}

	/* Draw cone */
	glBegin(GL_TRIANGLE_STRIP);
	for (unsigned int i = 0; i < poly; i++)
	{
		double phi = delta_phi * i;
		struct dvect3 rvect = dvect3_smul(radius, dvect3_direct(cos(phi), 0, sin(phi)));
		draw_vertex3(rvect);
		draw_vertex3(dvect3_direct(0, height, 0));
	}
	glEnd();

	/* Draw base circle */
	glBegin(GL_POLYGON);
	for (unsigned int i = 0; i < poly; i++)
	{
		double phi = delta_phi * i;
		struct dvect3 rvect = dvect3_smul(radius, dvect3_direct(cos(phi), 0, sin(phi)));
		draw_vertex3(rvect);
	}
	glEnd();

	glPopMatrix();
}

void draw_arrow(struct dvect3 base, struct dvect3 vect, double shaft_radius, double head_size, unsigned int poly)
{
	struct dvect3 shaft_vect, head_vect;

	shaft_vect = dvect3_sub(vect, dvect3_smul(head_size, dvect3_normalize(vect)));
	head_vect = dvect3_sub(vect, shaft_vect);
	
	draw_cylinder(base, dvect3_add(base, shaft_vect), shaft_radius, poly);
	draw_cone(dvect3_add(base, shaft_vect), head_vect, shaft_radius * 2, poly);
}

void draw_vectorfield(void)
{
	const double delta = 0.5;
	const int steps = 10;

	const unsigned int source_poly = 20;
	const double source_radius = 0.25;
	const double source_current = 1.0;

	struct dvect3 source_org_vect[source_poly];
	struct dvect3 source_wire_vect[source_poly];
	double delta_phi = 2 * M_PI / source_poly;

	/* Generate source circle. */
	for (unsigned int i = 0; i < source_poly; i++)
	{
		double phi = i * delta_phi;

		struct dvect3 source_end_vect = dvect3_smul(source_radius, dvect3_direct(cos(phi + delta_phi), sin(phi + delta_phi), 0));

		source_org_vect[i] = dvect3_smul(source_radius, dvect3_direct(cos(phi), sin(phi), 0));
		source_wire_vect[i] = dvect3_sub(source_end_vect, source_org_vect[i]);

		/* Draw the source via cylindrical wires. */
		
		//glColor3d(0.8, 0, 0);
		//draw_arrow(source_org_vect[i], source_wire_vect[i], WIRE_DIAMETER, ARROW_HEAD_SIZE, 15);
		glColor3d(0.8, 0.8, 0.8);
		draw_cylinder(source_org_vect[i], source_end_vect, WIRE_DIAMETER, 15);
	}

	struct dvect3 rvect = dvect3_direct(0.6, 0.4, 0);
	struct dvect3 fieldvect = dvect3_direct(0, 0, 0);

	/* Calculate field. */

	/* B-field */
	/*
		B = [mu_0 * I / (4 * pi * rho)] * [sin(alpha_2) - sin(alpha_1)] e_B
	*/
	for (unsigned int src_i = 0; src_i < /*source_poly*/ 1; src_i++)
	{
		/* Sum over all source components. */
		
		struct dvect3 vect_base = dvect3_sub(source_org_vect[src_i], rvect);
		struct dvect3 vect_end = dvect3_add(vect_base, source_wire_vect[src_i]);
		struct dvect3 vect_e_B = dvect3_normalize(dvect3_exmul(source_wire_vect[src_i], dvect3_smul(-1, vect_base)));
		
		double n_scalar = -dvect3_inmul(vect_base, source_wire_vect[src_i]) / pow(dvect3_length(source_wire_vect[src_i]), 2);
		struct dvect3 vect_n = dvect3_add(vect_base, dvect3_smul(n_scalar, source_wire_vect[src_i]));

		double alpha_1 = acos(( dvect3_inmul(vect_n, vect_base) ) / ( dvect3_length(vect_n) * dvect3_length(vect_base) ));
		double alpha_2 = acos(( dvect3_inmul(vect_n, vect_end) ) / ( dvect3_length(vect_n) * dvect3_length(vect_end) ));

		/* If the n-scalar is positive, the angles have to be counted negatively,
		as they are antiparallel to the current. */
		if (n_scalar > 0)
		{
			alpha_1 *= -1;
			alpha_2 *= -1;
		}

		//printf("alpha_1: %lf, alpha_2: %lf\n", alpha_1 * M_1_PI * 180, alpha_2 * M_1_PI * 180);

		double scalar_B = ( /*MU_0*/1 * source_current / (4 * M_PI * dvect3_length(vect_base)) ) * ( sin(alpha_2) - sin(alpha_1) );
		fieldvect = dvect3_add(fieldvect, dvect3_smul(scalar_B, vect_e_B));

		glColor3d(0, 1, 0);
		draw_arrow(rvect, vect_end, ARROW_SHAFT_SIZE, ARROW_HEAD_SIZE, ARROW_POLYGONS);
		glColor3d(1, 0, 0);
		draw_arrow(rvect, vect_base, ARROW_SHAFT_SIZE, ARROW_HEAD_SIZE, ARROW_POLYGONS);
		draw_arrow(dvect3_add(rvect, vect_base), source_wire_vect[src_i], ARROW_SHAFT_SIZE, ARROW_HEAD_SIZE, ARROW_POLYGONS);
		glColor3d(0, 0.6, 0.6);
		draw_arrow(rvect, vect_n, ARROW_SHAFT_SIZE, ARROW_HEAD_SIZE, ARROW_POLYGONS);
		glColor3d(0, 0, 1);
		draw_arrow(rvect, fieldvect, ARROW_SHAFT_SIZE, ARROW_HEAD_SIZE, ARROW_POLYGONS);
	}

	/*
	for (int i = -steps; i <= steps; i++)
	{
		for (int j = -steps; j <= steps; j++)
		{
			for (int k = -steps; k <= steps; k++)
			{
				struct dvect3 rvect = dvect3_direct(i * delta, j * delta, 0.0);
				struct dvect3 fieldvect;

				
				

				struct dvect3 rprime = dvect3_sub(rvect, dvect3_direct(1, -1, 0));
				fieldvect = dvect3_add(
					dvect3_smul(1 / (pow(dvect3_length(rvect), 2)), dvect3_normalize(rvect)),
					dvect3_smul(1 / (pow(dvect3_length(rprime), 2)), dvect3_normalize(rprime))
				);

				// Draw. Mode: Scale with color.
				double fieldstrength = dvect3_length(fieldvect) /2;
				double red_strength = (atan(fieldstrength) / M_PI) + 0.5;
				double green_strength = exp(-pow(fieldstrength, 2));
				double blue_strength = (atan(-fieldstrength) / M_PI) + 0.5;
				glColor3d(red_strength, green_strength, blue_strength);

				draw_arrow(rvect, dvect3_smul(0.6 * delta, dvect3_normalize(fieldvect)), ARROW_SHAFT_SIZE, ARROW_HEAD_SIZE, ARROW_POLYGONS);
			}
		}
	}*/
}

void display(GLFWwindow *window)
{
	/*
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	double radius = 10.0;
    double cam_x = sin(glfwGetTime() / 4) * radius;
    double cam_z = cos(glfwGetTime() / 4) * radius;

    glm::mat4 matrix_view = glm::lookAt(glm::vec3(cam_x, 1, cam_z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    
    double glmat_view[16];
    for (int col = 0; col < 4; col++)
    {
        for (int row = 0; row < 4; row++)
        {
            glmat_view[(col * 4) + row] = matrix_view[col][row];
        }
    }

	//glLoadIdentity();
    glLoadMatrixd(glmat_view);
	//glRotated(glfwGetTime()*30, 0, 0, 1);

	glPushMatrix();*/
	//glMatrixMode(GL_MODELVIEW);

	//glTranslatef(0.0f, 0.0f, -5.0f);
	

	/* Draw axis. */

	/*
	glColor3f(1, 0, 0);
	draw_arrow(dvect3_direct(0, 0, 0), dvect3_direct(1, 0, 0), ARROW_SHAFT_SIZE, ARROW_HEAD_SIZE, ARROW_POLYGONS);
	glColor3f(0, 1, 0);
	draw_arrow(dvect3_direct(0, 0, 0), dvect3_direct(0, 1, 0), ARROW_SHAFT_SIZE, ARROW_HEAD_SIZE, ARROW_POLYGONS);
	glColor3f(0, 0, 1);
	draw_arrow(dvect3_direct(0, 0, 0), dvect3_direct(0, 0, 1), ARROW_SHAFT_SIZE, ARROW_HEAD_SIZE, ARROW_POLYGONS);
	*/

	/* Draw vectorfield */
	draw_vectorfield();

	//glPopMatrix();

	//glFlush();
	
	//glfwSwapBuffers(window);
}

/*
void reshape(GLFWwindow *window)
{
	int width, height;

	glfwGetFramebufferSize(window, &width, &height);

	GLdouble window_aspect = (GLdouble)width / (GLdouble)height;

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-window_aspect, window_aspect, -1.0, 1.0, 1.0, 100.0);
}
*/

void glfw_err_callback(int error, const char *desc)
{
	fprintf(stderr, "Error (%d): %s\n", error, desc);
}

int main(int argc, char *argv[])
{
    printf("Simple OpenGL C++ Test.\nVersion %d.%d\n", APP_VERSION_MAJOR, APP_VERSION_MINOR);

	OpenVFVRenderer *renderer = new OpenVFVRenderer();

	if (renderer->open_window("Open Vectorfield Visualizer") != 0)
	{
		exit(-1);
	}

	renderer->set_draw_poly_resolution(20);

	/*
	if (!glfwInit())
	{
		fprintf(stderr, "GLFW init failed.\n");
		exit(-1);
	}

	glfwSetErrorCallback(glfw_err_callback);

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow *window = glfwCreateWindow(800, 800, "VectFieldView C++", NULL, NULL);
	if (!window)
	{
		fprintf(stderr, "GLFW window init failed.\n");
		exit(-1);
	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(0);

	glEnable(GL_MULTISAMPLE);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

	GLint iMultiSample = 0;
	GLint iNumSamples = 0;
	glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
	glGetIntegerv(GL_SAMPLES, &iNumSamples);
	printf("MSAA on, GL_SAMPLE_BUFFERS = %d, GL_SAMPLES = %d\n", iMultiSample, iNumSamples);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	*/

	while (!renderer->should_window_close())
	{
		/*
		reshape(window);

		display(window);

		glfwPollEvents();*/
		renderer->do_render();
	}

	/*
	glfwDestroyWindow(window);
	glfwTerminate();
	*/

	delete renderer;

	return 0;
}