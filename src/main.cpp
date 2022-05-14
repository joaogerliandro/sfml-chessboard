#include <iostream>
#include <vector>
#include <string>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/OpenGL.hpp>

#include <glm/ext.hpp>

typedef struct tile
{
    glm::vec2 side_x;
    glm::vec2 side_y;

    glm::vec2 world_cords;

    bool      selected   = false,
              has_object = false;
}Tile;

typedef struct cam
{
    glm::vec3 eye,
              at,
              up;
}Camera;

Camera main_cam;

std::vector<Tile> tiles_map;

bool fullscreen    = false,
     tiles_maped   = false,
     has_selected  = false;
std::string title_name("OpenGL Chessboard");

glm::vec2 aux_tile_len, aux_position, selected_tile;
glm::vec3 ortho_value,
          tile_len;
glm::vec4 highlight_color(0.0, 0.8, 0.0 , 0.2);

sf::Vector2i mouse_position, window_size;
sf::Vector2f world_size, world_position;

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

void fustrum_axis(bool lever)
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

void drawn_tiles()
{
    Tile aux_obj;

    for(Tile tile : tiles_map)
        if(tile.selected)
        {
            has_selected = true;
            selected_tile = glm::vec2(tile.world_cords.x, tile.world_cords.y);
        }

    for(int x = 0; x < ortho_value.x; x++)
    {
        for(int y = 0; y < ortho_value.y; y++)
        {
            if((x + y) % 2 == 0)
                glColor3f(0, 0, 0);
            else
                glColor3f(1, 1, 1);

            if(x == selected_tile.x && y == selected_tile.y && has_selected)
                glColor4fv(glm::value_ptr(highlight_color));

            glBegin(GL_QUADS);
                glVertex3f(x, ortho_value.y - y, ortho_value.z); //A: x, y
                
                glVertex3f(x + 1, ortho_value.y - y, ortho_value.z); //B: x+len, y

                glVertex3f(x + 1, ortho_value.y - (y + 1), ortho_value.z ); //D: x+len, y+len

                glVertex3f(x, ortho_value.y - (y + 1), ortho_value.z); //C: x, y+len
            glEnd();

            if(!tiles_maped)
            {
                aux_obj.side_x = glm::vec2(x, x + 1);
                aux_obj.side_y = glm::vec2(ortho_value.y - y, ortho_value.y - (y + 1));

                aux_obj.world_cords = glm::vec2(x, y);

                tiles_map.push_back(aux_obj);
            } 
        }
    }

    tiles_maped = true;
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

            if(x == selected_tile.x && z == selected_tile.y && has_selected)
                glColor4fv(glm::value_ptr(highlight_color));

            glBegin(GL_QUADS);
                glVertex3f(x    , 0, z); //A: x, y
                glVertex3f(x + 1, 0, z); //B: x+len, y
                glVertex3f(x + 1, 0, z + 1); //D: x+len, y+len
                glVertex3f(x    , 0, z + 1); //C: x, y+len
            glEnd();
        }
    }
}

void opengl_init()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPointSize(10.f);
    glLineWidth(5.f);
}

void opengl_2d_init()
{
    glm::mat4 idt_mat = glm::mat4(1.0);
    glLoadMatrixf(glm::value_ptr(idt_mat));

    glMatrixMode(GL_PROJECTION);
    glm::mat4 proj_mat = glm::ortho(
        0.f, ortho_value.x,
        0.f, ortho_value.y,
        -ortho_value.z, ortho_value.z
    );
    glLoadMatrixf(glm::value_ptr(proj_mat));
}

void opengl_3d_init()
{
    glm::mat4 idt_mat = glm::mat4(1.0);
    glLoadMatrixf(glm::value_ptr(idt_mat));

    glMatrixMode(GL_PROJECTION);
    glm::mat4 proj_mat = glm::frustum(
        -1.f, 1.f,
        -1.f, 1.f,
         1.f, 100.f
    );

    /* int32_t aspect_ratio = (float) window_size.x / (float) window_size.y;
    glm::mat4 proj_mat = glm::perspective(60, aspect_ratio, 2, 100); */

    glLoadMatrixf(glm::value_ptr(proj_mat));
}; 

void drawn_2d_context()
{
    glMatrixMode(GL_MODELVIEW);

    drawn_tiles();

    ortho_axis(true);
}

void drawn_3d_context(int z)
{
    glMatrixMode(GL_MODELVIEW);
    
    main_cam = {
        .eye = glm::vec3(4, 4, z),
        .at =  glm::vec3(0, 0, 0),
        .up =  glm::vec3(0, 1, 0)
    };

    glm::mat4 view_mat = glm::lookAt(main_cam.eye, main_cam.at, main_cam.up);
    glLoadMatrixf(glm::value_ptr(view_mat));

    drawn_3d_board();

    fustrum_axis(true);
}

void print_tiles_map()
{
    for (Tile tile : tiles_map)
    {
        std::cout << "Tile Sides -> X : ["      << tile.side_x.x 
                  << ", "                       << tile.side_x.y
                  << "] ,";
        
        std::cout << " Y : ["  << tile.side_y.x 
                  << ", "      << tile.side_y.y
                  << "] \t"; 

        std::cout << "World Cords -> X : ["     << tile.world_cords.x 
                  << ", "                       << tile.world_cords.y
                  << "] \t";
    }

    std::cout << "TilesMap Len: "  << tiles_map.size() 
              << std::endl;
}

int z = 4;

int main()
{
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), title_name, sf::Style::Default, gl_settings());
    window_size = sf::Vector2i(window.getSize().x, window.getSize().y);
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    window.setActive(true);

    ortho_value = glm::vec3(8, 8, 1);

    bool run = true;
    //while(window.isOpen())
    while(run)
    {   
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.25, 0.25, 0.25, 1);
        opengl_init();
        
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
                    window_size = sf::Vector2i(event.size.width, event.size.height);
                    break;
                case sf::Event::MouseButtonPressed:
                    switch (event.mouseButton.button)
                    {
                        case sf::Mouse::Button::Left:
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

                            std::cout << "["  << aux_position.x
                                      << ", " << aux_position.y
                                      << "]"  << std::endl;

                            for(int32_t i = 0; i < tiles_map.size(); i++)
                            {
                                if(tiles_map[i].world_cords.x == aux_position.x && tiles_map[i].world_cords.y == aux_position.y)
                                {
                                    if(tiles_map[i].selected)
                                    {
                                        tiles_map[i].selected = false;
                                        has_selected = false;
                                    }
                                    else    
                                        tiles_map[i].selected = true;
                                }
                                else
                                    tiles_map[i].selected = false;
                            }

                            if(!(aux_position.x < ortho_value.x))
                                has_selected = false;
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
                        case sf::Keyboard::E:
                            z++;
                            break;
                        case sf::Keyboard::Q:
                            z--;
                            break;
                        case sf::Keyboard::F11:
                            fullscreen = !(fullscreen);
                            if(fullscreen)
                                window.create(sf::VideoMode::getDesktopMode(), title_name, sf::Style::Fullscreen);
                            else
                                window.create(sf::VideoMode::getDesktopMode(), title_name, sf::Style::Default);
                            break;
                    }
                    break;    
            }
        }

        glViewport(0, 0, window_size.x / 2 , window_size.y);
        opengl_2d_init();
        drawn_2d_context();

        glViewport(window_size.x / 2, 0, window_size.x / 2 , window_size.y);
        opengl_3d_init();
        drawn_3d_context(z);
        
        window.display();
    }

    return 0;
}
