#include "generated/autoconf.h"
#include "generated/version.h"

#include <stdio.h>
#include <math.h>

/* #include <GL/glut.h> */

#include <GLFW/glfw3.h>

#include "vectmath.h"
#include "main.h"

#define ARROW_SHAFT_SIZE	0.0025
#define ARROW_HEAD_SIZE		0.02
#define ARROW_POLYGONS		10

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
	const double delta = 0.05;
	const int steps = 20;

	for (int i = -steps; i <= steps; i++)
	{
		for (int j = -steps; j <= steps; j++)
		{
			struct dvect3 rvect = dvect3_direct(i * delta, j * delta, 0.0);
			struct dvect3 fieldvect;

			/* Calculate field. */
			struct dvect3 rprime = dvect3_sub(rvect, dvect3_direct(1, -1, 0));
			fieldvect = dvect3_add(
				dvect3_smul(1 / (pow(dvect3_length(rvect), 2)), dvect3_normalize(rvect)),
				dvect3_smul(1 / (pow(dvect3_length(rprime), 2)), dvect3_normalize(rprime))
			);

			/* Draw. Mode: Scale with color. */
			double fieldstrength = dvect3_length(fieldvect) /2;
			double red_strength = (atan(fieldstrength) / M_PI) + 0.5;
			double green_strength = exp(-pow(fieldstrength, 2));
			double blue_strength = (atan(-fieldstrength) / M_PI) + 0.5;
			glColor3d(red_strength, green_strength, blue_strength);

			draw_arrow(rvect, dvect3_smul(0.6 * delta, dvect3_normalize(fieldvect)), ARROW_SHAFT_SIZE, ARROW_HEAD_SIZE, ARROW_POLYGONS);
		}
	}
}

void display(GLFWwindow *window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
	

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

	glFlush();
	
	glfwSwapBuffers(window);
}

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

void glfw_err_callback(int error, const char *desc)
{
	fprintf(stderr, "Error (%d): %s\n", error, desc);
}

int main(int argc, char *argv[])
{
    printf("Simple OpenGL C++ Test.\nVersion %d.%d\n", APP_VERSION_MAJOR, APP_VERSION_MINOR);

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

	while (!glfwWindowShouldClose(window))
	{
		reshape(window);

		display(window);

		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	
	/*

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	glutInitWindowPosition(80, 80);
	glutInitWindowSize(400, 400);
	glutCreateWindow("Simple OpenGL C++ Test.");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutSetOption(GLUT_MULTISAMPLE, 8);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	
	

	glutMainLoop();
	*/

	return 0;
}