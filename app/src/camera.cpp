
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"

OpenVFVCamera::OpenVFVCamera()
{

}

OpenVFVCamera::~OpenVFVCamera()
{

}

void OpenVFVCamera::apply_transform(void)
{
    const glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 direction = glm::normalize(this->position - this->target);
    glm::vec3 cam_right = glm::normalize(glm::cross(up, direction));
    glm::vec3 cam_up = glm::cross(direction, cam_right);

    double cam_x = sin(glfwGetTime() / 4) * 10.0;
    double cam_z = cos(glfwGetTime() / 4) * 10.0;

    glm::mat4 m_view = glm::lookAt(glm::vec3(cam_x, 5, cam_z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    // Convert viewmatrix to OpenGL viewmatrix.  
    double glm_view[16];
    for (int col = 0; col < 4; col++)
    {
        for (int row = 0; row < 4; row++)
        {
            glm_view[(col * 4) + row] = m_view[col][row];
        }
    }

    // Load view matrix.
    glLoadMatrixd(glm_view);
}