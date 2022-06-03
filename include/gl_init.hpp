#pragma once 

#include <SFML/OpenGL.hpp>
#include <glm/ext.hpp>

void opengl_init()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.25, 0.25, 0.25, 1);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPointSize(10.f);
    glLineWidth(5.f);
}

void opengl_2d_init(glm::vec3 ortho_vector)
{
    glDisable(GL_DEPTH_TEST);
    glm::mat4 idt_mat = glm::mat4(1.0);
    glLoadMatrixf(glm::value_ptr(idt_mat));

    glMatrixMode(GL_PROJECTION);
    glm::mat4 proj_mat = glm::ortho(
        0.f, ortho_vector.x,
        0.f, ortho_vector.y,
        -ortho_vector.z, ortho_vector.z
    );
    glLoadMatrixf(glm::value_ptr(proj_mat));
}

void opengl_3d_init()
{
    glEnable(GL_DEPTH_TEST);

    glm::mat4 idt_mat = glm::mat4(1.0);
    glLoadMatrixf(glm::value_ptr(idt_mat));

    glMatrixMode(GL_PROJECTION);
    glm::mat4 proj_mat = glm::frustum(
        -0.5f,    0.5f,
        -0.5f,    0.5f,
        0.75f,  1000.f
    );

    glLoadMatrixf(glm::value_ptr(proj_mat));
}; 

sf::ContextSettings gl_settings()
{
    sf::ContextSettings settings;
    settings.depthBits         = 24;
    settings.stencilBits       = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion      = 3;
    settings.minorVersion      = 0;
    
    return settings;
}