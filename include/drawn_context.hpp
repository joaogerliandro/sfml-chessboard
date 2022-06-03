#pragma once

#include <SFML/OpenGL.hpp>

#include <gl_init.hpp>

void ortho_axis(bool lever)
{
    if(!lever)
        return;

    glBegin(GL_LINES);
        //Eixo x em vermelho
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(0, ortho_value.y / 2, 0);
        glVertex3f(ortho_value.x, ortho_value.y / 2, 0); 
        
        //Eixo y em verde
        glColor3f(0.0, 1.0, 0.0);  
        glVertex3f(ortho_value.x / 2,  ortho_value.y, 0); 
        glVertex3f(ortho_value.x / 2, 0, 0); 
    glEnd();
}

void frustum_axis(bool lever)
{
    if(!lever)
        return;
   
    glBegin(GL_LINES);
        //Eixo x em vermelho
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f( 0.0, 0.0, 0.0);
        glVertex3f(10.0, 0.0, 0.0);

        //Eixo y em verde
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f( 0.0, 0.0, 0.0);
        glVertex3f( 0.0,10.0, 0.0);

        //Eixo z em azul
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f( 0.0, 0.0, 0.0);
        glVertex3f( 0.0, 0.0,10.0);
    glEnd();
}