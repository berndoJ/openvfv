

#ifndef __OPENVFV_CAMERA
#define __OPENVFV_CAMERA

#include <glm/glm.hpp>

class OpenVFVCamera
{
    private:
        glm::vec3 position;
        glm::vec3 target;
    public:
        OpenVFVCamera();
        ~OpenVFVCamera();
        void apply_transform(void);
}; 

#endif