#pragma once

#include <SFML/OpenGL.hpp>

#include <gl_init.hpp>
#include <board.hpp>

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

void drawn_tiles(Board board)
{
    Tile aux_obj;

    for(int32_t i = 0; i < board.tiles_map.size(); i++)
    {
        if(board.tiles_map[i].selected)
        {
            board.has_selected = true;
            board.selected_tile = glm::vec3(board.tiles_map[i].world_cords.x, board.tiles_map[i].world_cords.y, i);
        }

        if(board.tiles_map[i].focused)
        {
            board.has_focused = true;
            board.focused_object = glm::vec3(board.tiles_map[i].world_cords.x, 1.0, board.tiles_map[i].world_cords.y);
        }
    }

    for(int x = 0; x < ortho_value.x; x++)
    {
        for(int y = 0; y < ortho_value.y; y++)
        {
            if((x + y) % 2 == 0)
                glColor3f(0, 0, 0);
            else
                glColor3f(1, 1, 1);

            if(x == board.focused_object.x && y == board.focused_object.z && board.has_focused)
                glColor4fv(glm::value_ptr(focused_color));

            if(x == board.selected_tile.x && y == board.selected_tile.y && board.has_selected)
                glColor4fv(glm::value_ptr(highlight_color));

            if(x == board.selected_tile.x  && y == board.selected_tile.y  &&
               x == board.focused_object.x && y == board.focused_object.z && 
               board.has_focused           && board.has_selected)
                glColor4fv(glm::value_ptr(track_color));

            glBegin(GL_QUADS);
                glVertex3f(x, ortho_value.y - y, ortho_value.z); //A: x, y
                
                glVertex3f(x + 1, ortho_value.y - y, ortho_value.z); //B: x+len, y

                glVertex3f(x + 1, ortho_value.y - (y + 1), ortho_value.z ); //D: x+len, y+len

                glVertex3f(x, ortho_value.y - (y + 1), ortho_value.z); //C: x, y+len
            glEnd();

            if(!board.tiles_maped)
            {
                aux_obj.side_x = glm::vec2(
                    x , x + 1
                );
                aux_obj.side_y = glm::vec2(
                    ortho_value.y - y , ortho_value.y - (y + 1)
                );

                aux_obj.world_cords = glm::vec2(x, y);
                board.tiles_map.push_back(aux_obj);
            } 
        }
    }

    board.tiles_maped = true;
}

void drawn_3d_board(Board board)
{
    for(int x = 0; x < ortho_value.x; x++)
    {
        for(int z = 0; z < ortho_value.y; z++)
        {
            if((x + z) % 2 == 0)
                glColor3f(0, 0, 0);
            else
                glColor3f(1, 1, 1);

            if(x == board.focused_object.x && z == board.focused_object.z && board.has_focused)
                glColor4fv(glm::value_ptr(focused_color));

            if(x == board.selected_tile.x && z == board.selected_tile.y && board.has_selected)
                glColor4fv(glm::value_ptr(highlight_color));

            if(x == board.selected_tile.x  && z == board.selected_tile.y  &&
               x == board.focused_object.x && z == board.focused_object.z && 
               board.has_focused           && board.has_selected)
                glColor4fv(glm::value_ptr(track_color));

            glBegin(GL_QUADS);
                glVertex3f(x    , 0, z); //A: x, y
                glVertex3f(x + 1, 0, z); //B: x+len, y
                glVertex3f(x + 1, 0, z + 1); //D: x+len, y+len
                glVertex3f(x    , 0, z + 1); //C: x, y+len
            glEnd();
        }
    }
}