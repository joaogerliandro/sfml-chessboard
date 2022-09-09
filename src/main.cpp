#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/OpenGL.hpp>

#include <glm/ext.hpp>

#include <gl_init.hpp>
#include <camera.hpp>
#include <object.hpp>
#include <tile.hpp>
#include <drawn_context.hpp>
#include <board.hpp>

#include <config_loader.hpp>

#define SIDES  360
#define ANGLE  3.141 * 2.f / SIDES
#define RADIUS 0.35

Camera main_cam;
Object cam_obj, aux_obj;

std::vector<Meshe> loaded_objs;

Board sfml_board;

std::string title_name("OpenGL Chessboard");

glm::vec2 aux_position;

glm::vec3 tile_len, selected_tile,
          focused_object = init_focused;

sf::Vector2i mouse_position, window_size;
sf::Vector2f world_size    , world_position;

sf::RenderWindow window;

void drawn_tiles()
{
    Tile aux_obj;

    for(int32_t i = 0; i < sfml_board.tiles_map.size(); i++)
    {
        if(sfml_board.tiles_map[i].selected)
        {
            sfml_board.has_selected = true;
            selected_tile = glm::vec3(sfml_board.tiles_map[i].world_cords.x, sfml_board.tiles_map[i].world_cords.y, i);
        }

        if(sfml_board.tiles_map[i].focused)
        {
            sfml_board.has_focused = true;
            focused_object = glm::vec3(sfml_board.tiles_map[i].world_cords.x, 1.0, sfml_board.tiles_map[i].world_cords.y);
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

            if(x == focused_object.x && y == focused_object.z && sfml_board.has_focused)
                glColor4fv(glm::value_ptr(focused_color));

            if(x == selected_tile.x && y == selected_tile.y && sfml_board.has_selected)
                glColor4fv(glm::value_ptr(highlight_color));

            if(x == selected_tile.x   && y == selected_tile.y  &&
               x == focused_object.x  && y == focused_object.z && 
               sfml_board.has_focused && sfml_board.has_selected)
                glColor4fv(glm::value_ptr(track_color));

            glBegin(GL_QUADS);
                glVertex3f(x, ortho_value.y - y, ortho_value.z); //A: x, y
                
                glVertex3f(x + 1, ortho_value.y - y, ortho_value.z); //B: x+len, y

                glVertex3f(x + 1, ortho_value.y - (y + 1), ortho_value.z ); //D: x+len, y+len

                glVertex3f(x, ortho_value.y - (y + 1), ortho_value.z); //C: x, y+len
            glEnd();

            if(!sfml_board.tiles_maped)
            {
                aux_obj.side_x = glm::vec2(
                    x , x + 1
                );
                aux_obj.side_y = glm::vec2(
                    ortho_value.y - y , ortho_value.y - (y + 1)
                );

                aux_obj.world_cords = glm::vec2(x, y);
                sfml_board.tiles_map.push_back(aux_obj);
            } 
        }
    }

    sfml_board.tiles_maped = true;
}

void drawn_3d_board()
{
    for(int x = 0; x < ortho_value.x; x++)
    {
        for(int z = 0; z < ortho_value.y; z++)
        {
            if((x + z) % 2 == 0)
                glColor3f(0, 0, 0);
            else
                glColor3f(1, 1, 1);

            if(x == focused_object.x && z == focused_object.z && sfml_board.has_focused)
                glColor4fv(glm::value_ptr(focused_color));

            if(x == selected_tile.x && z == selected_tile.y && sfml_board.has_selected)
                glColor4fv(glm::value_ptr(highlight_color));

            if(x == selected_tile.x   && z == selected_tile.y  &&
               x == focused_object.x  && z == focused_object.z && 
               sfml_board.has_focused && sfml_board.has_selected)
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

void drawn_pins()
{
    glm::vec2 center_point;

    for(int32_t i = 0; i < sfml_board.tiles_map.size(); i++)
    {
        for(Object obj : sfml_board.object_list)
            if(obj.world_cords.x == sfml_board.tiles_map[i].world_cords.x &&
               obj.world_cords.z == sfml_board.tiles_map[i].world_cords.y)
            {
                sfml_board.tiles_map[i].has_object = true;
                center_point = glm::vec2((sfml_board.tiles_map[i].side_x.x + sfml_board.tiles_map[i].side_x.y) / 2,
                                          sfml_board.tiles_map[i].side_y.x - 0.5);

                glTranslatef(center_point.x, center_point.y, 0);

                glColor4fv(glm::value_ptr(obj.obj_color));
                glBegin(GL_POLYGON);
                    for(int32_t i = 0; i <= 360; i++)
                    {
                        glVertex2f(RADIUS * cosf(ANGLE * i),
                                   RADIUS * sinf(ANGLE * i));
                        
                    }
                glEnd();

                glColor4f(0, 0, 0, 1);
                glBegin(GL_LINE_LOOP);
                    for(int32_t i = 0; i <= 360; i++)
                    {
                        glVertex2f(RADIUS * cosf(ANGLE * i),
                                   RADIUS * sinf(ANGLE * i));
                        
                    }
                glEnd();

                glTranslatef(-center_point.x, -center_point.y, 0);
            }
    }
}

void drawn_objs()
{
    if(sfml_board.object_list.empty())
        return;

    int32_t polygon_type;

    for(Tile tile : sfml_board.tiles_map)
    {
        for(Object obj : sfml_board.object_list)
            if(obj.world_cords.x == tile.world_cords.x &&
               obj.world_cords.z == tile.world_cords.y)
            {
                if(obj.obj_type == CUBE)
                    polygon_type = GL_QUADS;
                else
                    polygon_type = GL_POLYGON;
                
                glTranslatef(tile.world_cords.x + 0.5 , 0.5, tile.world_cords.y + 0.5);
                glScalef(0.8, 0.8, 0.8);
                glRotatef(-90, 1.0, 0.0, 0.0);
                
                glColor4fv(glm::value_ptr(obj.obj_color));

                if(obj.meshe_ptr != nullptr)
                {
                    glBegin(polygon_type);
                        for(glm::vec3 vertex : obj.meshe_ptr->polygon_vertexs)
                            glVertex3f(
                                vertex.x,
                                vertex.z,
                                vertex.y
                            );
                    glEnd();

                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glColor4f(0.0, 0.0, 0.0, 1.0);
                    glBegin(polygon_type);
                        for(glm::vec3 vertex : obj.meshe_ptr->polygon_vertexs)
                            glVertex3f(
                                vertex.x,
                                vertex.z,
                                vertex.y
                            );
                    glEnd();
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
                
                glRotatef(90, 1.0, 0.0, 0.0);
                glScalef(1/0.8, 1/0.8, 1/0.8);
                glTranslatef(-obj.world_cords.x - 0.5, -0.5, -obj.world_cords.z - 0.5);
            }
    }
}

void drawn_2d_context()
{
    glMatrixMode(GL_MODELVIEW);

    drawn_tiles();

    ortho_axis(false);

    drawn_pins();
}

void drawn_3d_context()
{
    glMatrixMode(GL_MODELVIEW);

    main_cam.eye = cam_obj.world_cords;
    main_cam.at  = focused_object;

    glm::mat4 view_mat = glm::lookAt(main_cam.eye, main_cam.at, main_cam.up);
    glLoadMatrixf(glm::value_ptr(view_mat));

    drawn_3d_board();

    frustum_axis(false);

    drawn_objs();
}

void get_mouse_position()
{
    glm::vec2 aux_tile_len;
    
    mouse_position = sf::Mouse::getPosition(window);
    world_position = window.mapPixelToCoords(mouse_position);
    world_size     = window.mapPixelToCoords(window_size);

    aux_tile_len = glm::vec2(
                                (world_size.x / (2.f * ortho_value.x)), 
                                (world_size.y / (ortho_value.y))
    ); 
    aux_position = glm::vec2((int32_t) (world_position.x / aux_tile_len.x),
                                                   (int32_t) (world_position.y / aux_tile_len.y)
    );
}

int main()
{
    load_config();

    loaded_objs = load_objs();
    
    window.create(sf::VideoMode::getDesktopMode(), title_name, sf::Style::Default, get_settings());
    window_size = sf::Vector2i(window.getSize().x, window.getSize().y);
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(true);
    window.setActive(true);

    sf::Image sfml_icon;
    sfml_icon.loadFromFile("../../../assets/icon/icon.png");
    window.setIcon(sfml_icon.getSize(), sfml_icon.getPixelsPtr());

    main_cam = {
        .eye =  glm::vec3(0, 1.5, 0),
        .at  =  focused_object,
        .up  =  glm::vec3(0, 1, 0)
    };

    cam_obj.world_cords = main_cam.eye;
    cam_obj.obj_color   = glm::vec4(0.8, 0.8, 0.8, 1);

    sfml_board.object_list.push_back(cam_obj);

    srand(time(NULL));
    bool running = true;

    while(running)
    {   
        opengl_init();
        
        sf::Event event;
        while(window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    running = false;
                    break;
                case sf::Event::Resized:
                    window_size = sf::Vector2i(event.size.width, event.size.height);
                    break;
                case sf::Event::MouseButtonPressed:
                    switch (event.mouseButton.button)
                    {
                        case sf::Mouse::Button::Left:
                            get_mouse_position();

                            for(int32_t i = 0; i < sfml_board.tiles_map.size(); i++)
                            {
                                if(sfml_board.tiles_map[i].world_cords.x == aux_position.x && sfml_board.tiles_map[i].world_cords.y == aux_position.y)
                                {
                                    if(sfml_board.tiles_map[i].selected)
                                    {
                                        sfml_board.tiles_map[i].selected = false;
                                        sfml_board.has_selected = false;
                                    }
                                    else    
                                        sfml_board.tiles_map[i].selected = true;
                                }
                                else
                                    sfml_board.tiles_map[i].selected = false;
                            }

                            if(!(aux_position.x < ortho_value.x))
                                sfml_board.has_selected = false;
                            break;
                    case sf::Mouse::Button::Right:
                            get_mouse_position();

                            for(int32_t i = 0; i < sfml_board.tiles_map.size(); i++)
                            {
                                if(sfml_board.tiles_map[i].world_cords.x == aux_position.x && 
                                   sfml_board.tiles_map[i].world_cords.y == aux_position.y &&
                                   sfml_board.tiles_map[i].has_object)
                                {                            
                                    if(sfml_board.tiles_map[i].focused)
                                    {
                                        sfml_board.tiles_map[i].focused = false;
                                        sfml_board.has_focused = false;
                                    }
                                    else    
                                        sfml_board.tiles_map[i].focused = true;

                                    if(aux_position.x == cam_obj.world_cords.x && aux_position.y == cam_obj.world_cords.z)
                                    {
                                        sfml_board.tiles_map[i].focused = false;
                                        sfml_board.has_focused = false;
                                    }
                                }
                                else
                                    sfml_board.tiles_map[i].focused = false;
                                    
                            }

                            if(!(aux_position.x < ortho_value.x))
                                sfml_board.has_focused = false; 

                            if(!sfml_board.has_focused)
                                focused_object = glm::vec3(8, 1, 8);

                            break;
                    }
                case sf::Event::KeyPressed:
                    switch (event.key.code)
                    {
                        case sf::Keyboard::Escape:
                            running = false;
                            break;
                        case sf::Keyboard::R:
                            aux_obj.world_cords = glm::vec3(
                                rand() % 8,
                                1.5,
                                rand() % 8
                            );

                            aux_obj.obj_color = glm::vec4(
                                (rand() % 255) / 255.f,
                                (rand() % 255) / 255.f,
                                (rand() % 255) / 255.f,
                                1.0
                            );
                            
                            aux_obj.obj_type = (OBJECT_TYPE) (rand() % 4);

                            aux_obj.meshe_ptr = &(loaded_objs[aux_obj.obj_type]);
                            
                            for(int32_t i = 0; i < sfml_board.tiles_map.size(); i++)
                                if( sfml_board.tiles_map[i].world_cords.x == aux_obj.world_cords.x &&
                                    sfml_board.tiles_map[i].world_cords.y == aux_obj.world_cords.z &&
                                   !sfml_board.tiles_map[i].has_object    && sfml_board.object_list.size() < sfml_board.tiles_map.size() - 1
                                )
                                    sfml_board.object_list.push_back(aux_obj);
                            break;
                            case sf::Keyboard::Delete:
                                for(int32_t i = 1; i < sfml_board.object_list.size(); i++)
                                {
                                    if(sfml_board.object_list[i].world_cords.x == selected_tile.x   &&
                                    sfml_board.object_list[i].world_cords.z == selected_tile.y   && 
                                    sfml_board.tiles_map[selected_tile.z].has_object  &&
                                    sfml_board.tiles_map[selected_tile.z].selected)
                                    {
                                        sfml_board.object_list.erase(sfml_board.object_list.begin() + i);
                                        sfml_board.tiles_map[selected_tile.z].has_object = false;

                                        if(sfml_board.tiles_map[selected_tile.z].focused)
                                        {
                                            sfml_board.tiles_map[selected_tile.z].focused = false;
                                            sfml_board.has_focused = false;
                                            focused_object = glm::vec3(8, 1, 8);
                                        }
                                    }
                                }
                                break;
                            case sf::Keyboard::End:
                                while(sfml_board.object_list.size() != 1)
                                    sfml_board.object_list.erase(sfml_board.object_list.begin() + 1);

                                for(int32_t i = 1; i < sfml_board.tiles_map.size(); i++)
                                    if(sfml_board.tiles_map[i].has_object)
                                        sfml_board.tiles_map[i].has_object = false;

                                if(sfml_board.tiles_map[selected_tile.z].focused)
                                {
                                    sfml_board.tiles_map[selected_tile.z].focused = false;
                                    sfml_board.has_focused = false;
                                    focused_object = glm::vec3(8, 1, 8);
                                }
                                break;
                        case sf::Keyboard::Right:
                            for(int32_t i = 0; i < sfml_board.object_list.size(); i++)
                            {
                                if(sfml_board.object_list[i].world_cords.x == selected_tile.x   &&
                                   sfml_board.object_list[i].world_cords.z == selected_tile.y   && 
                                   sfml_board.tiles_map[selected_tile.z].has_object  &&
                                   sfml_board.tiles_map[selected_tile.z].selected)
                                {
                                    if(sfml_board.object_list[i].world_cords.x + 1 <= ortho_value.x)
                                    {
                                        for(int32_t j = 0; j < sfml_board.tiles_map.size(); j++)
                                        {
                                            if( sfml_board.tiles_map[j].world_cords.x == sfml_board.object_list[i].world_cords.x + 1 &&
                                                sfml_board.tiles_map[j].world_cords.y == sfml_board.object_list[i].world_cords.z     &&
                                               !sfml_board.tiles_map[j].has_object)
                                            {   
                                                sfml_board.tiles_map[selected_tile.z].selected   = false;
                                                sfml_board.tiles_map[selected_tile.z].has_object = false;
                                                
                                                sfml_board.tiles_map[j].selected   = true;
                                                sfml_board.tiles_map[j].has_object = true;

                                                if(sfml_board.tiles_map[selected_tile.z].world_cords.x == focused_object.x &&
                                                   sfml_board.tiles_map[selected_tile.z].world_cords.y == focused_object.z
                                                )
                                                {
                                                    sfml_board.tiles_map[selected_tile.z].focused   = false;
                                                    sfml_board.tiles_map[j].focused                 = true;
                                                    focused_object.x++;
                                                }
                                            }
                                        }

                                        if(!sfml_board.tiles_map[selected_tile.z].selected)
                                            sfml_board.object_list[i].world_cords.x++;
                                
                                    }
                                }
                            }
                            break;
                        case sf::Keyboard::Left:
                            for(int32_t i = 0; i < sfml_board.object_list.size(); i++)
                            {
                                if(sfml_board.object_list[i].world_cords.x == selected_tile.x   &&
                                   sfml_board.object_list[i].world_cords.z == selected_tile.y   && 
                                   sfml_board.tiles_map[selected_tile.z].has_object  &&
                                   sfml_board.tiles_map[selected_tile.z].selected)
                                {
                                    if(sfml_board.object_list[i].world_cords.x - 1 >= 0)
                                    {
                                        for(int32_t j = 0; j < sfml_board.tiles_map.size(); j++)
                                        {
                                            if( sfml_board.tiles_map[j].world_cords.x == sfml_board.object_list[i].world_cords.x - 1 &&
                                                sfml_board.tiles_map[j].world_cords.y == sfml_board.object_list[i].world_cords.z     &&
                                               !sfml_board.tiles_map[j].has_object)
                                            {
                                                sfml_board.tiles_map[selected_tile.z].selected   = false;
                                                sfml_board.tiles_map[selected_tile.z].has_object = false;
                                                
                                                sfml_board.tiles_map[j].selected   = true;
                                                sfml_board.tiles_map[j].has_object = true;

                                                if(sfml_board.tiles_map[selected_tile.z].world_cords.x == focused_object.x &&
                                                   sfml_board.tiles_map[selected_tile.z].world_cords.y == focused_object.z
                                                )
                                                {
                                                    sfml_board.tiles_map[selected_tile.z].focused   = false;
                                                    sfml_board.tiles_map[j].focused                 = true;
                                                    focused_object.x--;
                                                }
                                            }
                                        }

                                        if(!sfml_board.tiles_map[selected_tile.z].selected)
                                            sfml_board.object_list[i].world_cords.x--;

                                    }
                                }
                            }
                            break;
                        case sf::Keyboard::Down:
                            for(int32_t i = 0; i < sfml_board.object_list.size(); i++)
                            {
                                if(sfml_board.object_list[i].world_cords.x == selected_tile.x   &&
                                   sfml_board.object_list[i].world_cords.z == selected_tile.y   && 
                                   sfml_board.tiles_map[selected_tile.z].has_object  &&
                                   sfml_board.tiles_map[selected_tile.z].selected)
                                {
                                    if(sfml_board.object_list[i].world_cords.z + 1 <= ortho_value.y)
                                    {
                                        for(int32_t j = 0; j < sfml_board.tiles_map.size(); j++)
                                        {
                                            if( sfml_board.tiles_map[j].world_cords.x == sfml_board.object_list[i].world_cords.x     &&
                                                sfml_board.tiles_map[j].world_cords.y == sfml_board.object_list[i].world_cords.z + 1 &&
                                               !sfml_board.tiles_map[j].has_object)
                                            {
                                                sfml_board.tiles_map[selected_tile.z].selected   = false;
                                                sfml_board.tiles_map[selected_tile.z].has_object = false;
                                                
                                                sfml_board.tiles_map[j].selected   = true;
                                                sfml_board.tiles_map[j].has_object = true;

                                                if(sfml_board.tiles_map[selected_tile.z].world_cords.x == focused_object.x &&
                                                   sfml_board.tiles_map[selected_tile.z].world_cords.y == focused_object.z
                                                )
                                                {
                                                    sfml_board.tiles_map[selected_tile.z].focused   = false;
                                                    sfml_board.tiles_map[j].focused                 = true;
                                                    focused_object.z++;
                                                }
                                            }
                                        }

                                        if(!sfml_board.tiles_map[selected_tile.z].selected)
                                            sfml_board.object_list[i].world_cords.z++;

                                    }
                                }
                            }
                            break;
                        case sf::Keyboard::Up:
                            for(int32_t i = 0; i < sfml_board.object_list.size(); i++)
                            {
                                if(sfml_board.object_list[i].world_cords.x == selected_tile.x   &&
                                   sfml_board.object_list[i].world_cords.z == selected_tile.y   && 
                                   sfml_board.tiles_map[selected_tile.z].has_object  &&
                                   sfml_board.tiles_map[selected_tile.z].selected)
                                {
                                    if(sfml_board.object_list[i].world_cords.z - 1 >= 0)
                                    {
                                        for(int32_t j = 0; j < sfml_board.tiles_map.size(); j++)
                                        {
                                            if(sfml_board.tiles_map[j].world_cords.x == sfml_board.object_list[i].world_cords.x     &&
                                               sfml_board.tiles_map[j].world_cords.y == sfml_board.object_list[i].world_cords.z - 1 &&
                                               !sfml_board.tiles_map[j].has_object)
                                            {
                                                sfml_board.tiles_map[selected_tile.z].selected   = false;
                                                sfml_board.tiles_map[selected_tile.z].has_object = false;
                                                
                                                sfml_board.tiles_map[j].selected   = true;
                                                sfml_board.tiles_map[j].has_object = true;

                                                if(sfml_board.tiles_map[selected_tile.z].world_cords.x == focused_object.x &&
                                                   sfml_board.tiles_map[selected_tile.z].world_cords.y == focused_object.z
                                                )
                                                {
                                                    sfml_board.tiles_map[selected_tile.z].focused   = false;
                                                    sfml_board.tiles_map[j].focused                 = true;
                                                    focused_object.z--;
                                                }
                                            }
                                        }

                                        if(!sfml_board.tiles_map[selected_tile.z].selected)
                                            sfml_board.object_list[i].world_cords.z--;

                                    }
                                }
                            }
                            break;
                        case sf::Keyboard::F11:
                            sfml_board.fullscreen = !(sfml_board.fullscreen);
                            if(sfml_board.fullscreen)
                            {
                                window.create(sf::VideoMode::getDesktopMode(), title_name, sf::Style::Fullscreen, get_settings());
                                window_size = sf::Vector2i(window.getSize().x, window.getSize().y);
                            }
                            else
                            {
                                window.create(sf::VideoMode::getDesktopMode(), title_name, sf::Style::Default, get_settings());
                                window_size = sf::Vector2i(window.getSize().x, window.getSize().y);
                            }
                            break;
                    }
                    break;    
            }
        }

        cam_obj.world_cords = sfml_board.object_list[0].world_cords;

        glViewport(0, 0, window_size.x / 2 , window_size.y);
        opengl_2d_init();
        drawn_2d_context();

        glViewport(window_size.x / 2, 0, window_size.x / 2 , window_size.y);
        opengl_3d_init();
        drawn_3d_context();
        
        window.display();
    }

    return 0;
}
