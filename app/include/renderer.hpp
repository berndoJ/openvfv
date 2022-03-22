

#ifndef __OPENVFV_RENDERER_HPP
#define __OPENVFV_RENDERER_HPP

#include <GLFW/glfw3.h>
#include "camera.hpp"

class OpenVFVRenderer
{
    private:
        GLFWwindow *window_handle;
        bool window_opended;
        OpenVFVCamera camera;

        unsigned int draw_polygons;
        double draw_vector_radius;
        double draw_vector_head_radius;
        double draw_vector_head_size;

        void _handle_window_reshape(void);
        void _gl_set_global_opts(void);

    public:
        OpenVFVRenderer();
        ~OpenVFVRenderer();
        int open_window(const char *title);
        void do_render(void);
        bool should_window_close(void);

        void set_draw_poly_resolution(unsigned int polygons);
};

#endif