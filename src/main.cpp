#include <iostream>
#include <vector>
#include <cmath>
#include <string>

#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>

typedef struct tile
{
    glm::vec3 coordinates;
    bool      selected   = false,
              has_object = false;
}Tile;

std::vector<Tile> tiles_map;

glm::vec3 ortho_value, viewport1_size,
          tile_len, board_len,
          matrix_len, aux_len;


bool fullscreen    = false,
     tiles_maped   = false;
std::string title_name("OpenGL Chessboard");

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

void ortho_axis()
{
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
        glVertex3f(0,  viewport1_size.y, 0); 
        glVertex3f(0, -viewport1_size.y, 0); 

        glVertex3f(-viewport1_size.x, 0, 0);
        glVertex3f( viewport1_size.x, 0, 0); 
    glEnd();
}

void drawn_tiles(double width, double height)
{
    if(width < 2.0 || height < 2.0)
        return;

    Tile aux_obj;
    matrix_len = glm::vec3(width, height, 1);

    board_len = glm::vec3(
                          viewport1_size.x,
                          viewport1_size.y,
                          viewport1_size.z
    );
    
    tile_len = glm::vec3(board_len.x / width ,
                         board_len.y / height,
                         board_len.z);

    for(int x = 0; x < width; x++)
    {
        for(int y = 1; y <= height; y++)
        {
            if((x + y) % 2 == 0)
                glColor3f(0, 0, 0);
            else
                glColor3f(1, 1, 1);

            /*
                A: x, y       B: x+len, y
                C: x, y+len   D: x+len, y+len 
            */

            glBegin(GL_QUADS);
                glVertex3f(-(board_len.x / 2) + x * tile_len.x,
                            (board_len.y / 2) - y * tile_len.y,
                                                    tile_len.z
                );//A: x, y
                
                glVertex3f(-(board_len.x / 2) + x * tile_len.x + tile_len.x,
                            (board_len.y / 2) - y * tile_len.y,
                                                    tile_len.z
                ); //B: x+len, y

                glVertex3f(-(board_len.x / 2) + x * tile_len.x + tile_len.x,
                            (board_len.y / 2) - y * tile_len.y + tile_len.y,
                                                    tile_len.z    
                ); //D: x+len, y+len

                glVertex3f(-(board_len.x / 2) + x * tile_len.x,
                            (board_len.y / 2) - y * tile_len.y + tile_len.y,
                                                    tile_len.z
                ); //C: x, y+len
            glEnd();

            if(!tiles_maped)
            {
                //XB + XA (Invertemos o sinal devido contarmos do eixo negativo)
                aux_len.x = (-(board_len.x / 2) + x * tile_len.x + tile_len.x) 
                                                + 
                            (-(board_len.x / 2) + x * tile_len.x);            
                
                //YC + YA (Invertemos o sinal devido contarmos do eixo negativo)
                aux_len.y = ((board_len.y / 2) - y * tile_len.y + tile_len.y)
                                               +
                            ((board_len.y / 2) - y * tile_len.y);

                /* aux_x_len = (-ortho_value.x + ((x * tile_len_x)) + tile_len_x) + (-ortho_value.x + (x * tile_len_x)); //XB - XA
                aux_y_len = (ortho_value.y - ((y * tile_len_y) - tile_len_y))  + ortho_value.y - (y * tile_len_y);    //YC - YA */
                
                aux_obj.coordinates.x = aux_len.x;
                aux_obj.coordinates.y = aux_len.y; 
                tiles_map.push_back(aux_obj);
            } 
        }
    }

    tiles_maped = true;
}

void draw_context()
{
    glClearColor(1, 1, 1, 1);

    drawn_tiles(8, 8);

    ortho_axis();
}

void opengl_init()
{
    glEnable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPointSize(10.f);
    glLineWidth(5.f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(
            -(viewport1_size.x / 2),   (viewport1_size.x / 2),
            -(viewport1_size.y / 2),   (viewport1_size.y / 2),
             (viewport1_size.z    ),  -(viewport1_size.z    )
           );
}

void print_tiles_map()
{
    for (Tile tile : tiles_map)
        std::cout << "("  << tile.coordinates.x 
                  << ", " << tile.coordinates.y
                  << ")"  << std::endl; 

    std::cout << "Tiles Len: "  << tiles_map.size() 
              << std::endl;
}

int main()
{
    sf::Window window(sf::VideoMode::getDesktopMode(), title_name, sf::Style::Default, gl_settings());
    sf::Vector2i local_position;
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    window.setActive(true);

    viewport1_size = glm::vec3(window.getPosition().x / 2,
                               window.getPosition().y,
                               1);
    opengl_init();

    bool run = true;
    //while(window.isOpen())
    while(run)
    {  
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        sf::Event event;
        while(window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    //window.close();
                    run = false;
                    break;
                case sf::Event::Resized:
                    glViewport(0, 0, event.size.width / 2, event.size.height); //Viewport 1
                    //Viewport 2
                    break;
                case sf::Event::MouseButtonPressed:
                    switch (event.mouseButton.button)
                    {
                        case sf::Mouse::Button::Left:
                            local_position = sf::Mouse::getPosition(window);
                            std::cout << "("  << (int32_t) ((((local_position.x) / 2) / matrix_len.x) / tile_len.x)
                                      << ", " << (int32_t) (((local_position.y) / matrix_len.y) / tile_len.y)
                                      << ")"  << std::endl;
                            break;
                    }
                case sf::Event::KeyPressed:
                    switch (event.key.code)
                    {
                        case sf::Keyboard::Escape:
                            run = false;
                            break;
                        case sf::Keyboard::D:
                            print_tiles_map();
                            break;
                        case sf::Keyboard::F11:
                            fullscreen = !(fullscreen);
                            if(fullscreen)
                            {
                                window.create(sf::VideoMode::getDesktopMode(), title_name, sf::Style::Fullscreen);
                                opengl_init();
                            }
                            else
                            {
                                window.create(sf::VideoMode::getDesktopMode(), title_name, sf::Style::Default);
                                opengl_init();
                            }
                            break;
                    }
                    break;    
            }
        }

        draw_context();
        window.display();
    }

    return 0;
}
