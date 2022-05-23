#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/OpenGL.hpp>

#include <glm/ext.hpp>

#define SIDES  360
#define ANGLE  3.141 * 2.f / SIDES
#define RADIUS 0.35

enum OBJECT_TYPE : int32_t
{
    CUBE,
    PYRAMID,
    ETHER,
    CONE
};
typedef struct tile
{
    glm::vec2 side_x, side_y;

    glm::vec2 world_cords;

    bool      selected   = false,
              focused    = false,
              has_object = false;
}Tile;

typedef struct obj
{
    glm::vec3 world_cords;
    glm::vec4 obj_color;

    std::vector<glm::vec3> polygon_vertexs;

    OBJECT_TYPE obj_type;
}Object;

typedef struct cam
{   
    glm::vec3 eye,
              at,
              up;            
}Camera;

Camera main_cam;
Object cam_obj, aux_obj;

std::vector<Tile>   tiles_map;
std::vector<Object> object_list;

bool fullscreen    = false,
     tiles_maped   = false,
     has_selected  = false,
     has_focused   = false;
std::string title_name("OpenGL Chessboard");

glm::vec2 aux_position;

glm::vec3 ortho_value  , tile_len, selected_tile,
          focused_object = glm::vec3(8, 1, 8);

glm::vec4 highlight_color(0.0, 0.8, 0.0 , 0.2),
          focused_color  (1.0, 0.8, 0.0,  0.2),
          track_color    (0.0, 0.0, 0.8, 0.2);

sf::Vector2i mouse_position, window_size;
sf::Vector2f world_size    , world_position;

sf::RenderWindow window;

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

std::vector<glm::vec3> obj_loader(const char* obj_path)
{
    //Vertexs
    std::vector<glm::fvec3> vertexs_positions;
    //Faces
    std::vector<GLint>      vertexs_indicies;
    
    //Vertexs e temp array
    std::vector<glm::vec3>  polygon_vertexs;

    std::stringstream str_stream;
    std::ifstream     in_file(obj_path);

    std::string       line        = "",
                      line_prefix = "";

    //Aux atributes 
    GLint     aux_glint = 0;
    glm::vec2 aux_vec2; 
    glm::vec3 aux_vec3;

    if(!in_file.is_open())
        throw "ERRO: Can't open the file !";

    while(std::getline(in_file, line))
    {
        str_stream.clear();
        str_stream.str(line);
        str_stream >> line_prefix;

        if(line_prefix == "v")
        {
            str_stream >> aux_vec3.x >> aux_vec3.y >> aux_vec3.z;
            vertexs_positions.push_back(aux_vec3);
        }
        else if(line_prefix == "f")
        {
            int32_t counter = 0;
            while(str_stream >> aux_glint)
            {
                if(counter == 0)
                    vertexs_indicies.push_back(aux_glint);

                if(str_stream.peek() == '/')
                {
                    counter++;
                    str_stream.ignore(1, '/');
                }
                else if(str_stream.peek() == ' ')
                {
                    counter++;
                    str_stream.ignore(1, ' ');
                }

                if(counter > 2)
                    counter = 0;
            }
        }

        polygon_vertexs.resize(
            vertexs_indicies.size(),
            glm::vec3()                    
        );

        for(size_t i = 0; i < polygon_vertexs.size(); i++)
            polygon_vertexs[i] = vertexs_positions[vertexs_indicies[i] - 1];

    }

    return polygon_vertexs;
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

void drawn_tiles()
{
    Tile aux_obj;

    for(int32_t i = 0; i < tiles_map.size(); i++)
    {
        if(tiles_map[i].selected)
        {
            has_selected = true;
            selected_tile = glm::vec3(tiles_map[i].world_cords.x, tiles_map[i].world_cords.y, i);
        }

        if(tiles_map[i].focused)
        {
            has_focused = true;
            focused_object = glm::vec3(tiles_map[i].world_cords.x, 1.0, tiles_map[i].world_cords.y);
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

            if(x == focused_object.x && y == focused_object.z && has_focused)
                glColor4fv(glm::value_ptr(focused_color));

            if(x == selected_tile.x && y == selected_tile.y && has_selected)
                glColor4fv(glm::value_ptr(highlight_color));

            if(x == selected_tile.x  && y == selected_tile.y  &&
               x == focused_object.x && y == focused_object.z && 
               has_focused           && has_selected)
                glColor4fv(glm::value_ptr(track_color));

            glBegin(GL_QUADS);
                glVertex3f(x, ortho_value.y - y, ortho_value.z); //A: x, y
                
                glVertex3f(x + 1, ortho_value.y - y, ortho_value.z); //B: x+len, y

                glVertex3f(x + 1, ortho_value.y - (y + 1), ortho_value.z ); //D: x+len, y+len

                glVertex3f(x, ortho_value.y - (y + 1), ortho_value.z); //C: x, y+len
            glEnd();

            if(!tiles_maped)
            {
                aux_obj.side_x = glm::vec2(
                    x , x + 1
                );
                aux_obj.side_y = glm::vec2(
                    ortho_value.y - y , ortho_value.y - (y + 1)
                );

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

            if(x == focused_object.x && z == focused_object.z && has_focused)
                glColor4fv(glm::value_ptr(focused_color));

            if(x == selected_tile.x && z == selected_tile.y && has_selected)
                glColor4fv(glm::value_ptr(highlight_color));

            if(x == selected_tile.x  && z == selected_tile.y  &&
               x == focused_object.x && z == focused_object.z && 
               has_focused           && has_selected)
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
    if(object_list.empty())
        return;

    glm::vec2 center_point;

    for(int32_t i = 0; i < tiles_map.size(); i++)
    {
        for(Object obj : object_list)
            if(obj.world_cords.x == tiles_map[i].world_cords.x &&
               obj.world_cords.z == tiles_map[i].world_cords.y)
            {
                tiles_map[i].has_object = true;
                center_point = glm::vec2((tiles_map[i].side_x.x + tiles_map[i].side_x.y) / 2,
                                          tiles_map[i].side_y.x - 0.5);

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
    if(object_list.empty())
        return;

    int32_t polygon_type;

    for(Tile tile : tiles_map)
    {
        for(Object obj : object_list)
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
                glBegin(polygon_type);
                    for(glm::vec3 vertex : obj.polygon_vertexs)
                        glVertex3f(
                            vertex.x,
                            vertex.z,
                            vertex.y
                        );
                glEnd();

                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glColor4f(0.0, 0.0, 0.0, 1.0);
                glBegin(polygon_type);
                    for(glm::vec3 vertex : obj.polygon_vertexs)
                        glVertex3f(
                            vertex.x,
                            vertex.z,
                            vertex.y
                        );
                glEnd();
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                
                glRotatef(90, 1.0, 0.0, 0.0);
                glScalef(1/0.8, 1/0.8, 1/0.8);
                glTranslatef(-obj.world_cords.x - 0.5, -0.5, -obj.world_cords.z - 0.5);
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
    glDisable(GL_DEPTH_TEST);
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
    glEnable(GL_DEPTH_TEST);
    glm::mat4 idt_mat = glm::mat4(1.0);
    glLoadMatrixf(glm::value_ptr(idt_mat));

    glMatrixMode(GL_PROJECTION);
    glm::mat4 proj_mat = glm::frustum(
        -0.5f,  0.5f,
        -0.5f,  0.5f,
         0.75f,  100.f
    );

    glLoadMatrixf(glm::value_ptr(proj_mat));
}; 

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

    frustum_axis(true);

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
    window.create(sf::VideoMode::getDesktopMode(), title_name, sf::Style::Default, gl_settings());
    window_size = sf::Vector2i(window.getSize().x, window.getSize().y);
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(true);
    window.setActive(true);

    ortho_value = glm::vec3(8, 8, 1);
    main_cam = {
        .eye =  glm::vec3(0, 1.5, 0),
        .at  =  focused_object,
        .up  =  glm::vec3(0, 1, 0)
    };

    cam_obj.world_cords = main_cam.eye;
    cam_obj.obj_color   = glm::vec4(0.8, 0.8, 0.8, 1);

    object_list.push_back(cam_obj);

    srand(time(NULL));
    bool running = true;

    while(running)
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
                    case sf::Mouse::Button::Right:
                            get_mouse_position();

                            for(int32_t i = 0; i < tiles_map.size(); i++)
                            {
                                if(tiles_map[i].world_cords.x == aux_position.x && 
                                   tiles_map[i].world_cords.y == aux_position.y &&
                                   tiles_map[i].has_object)
                                {                            
                                    if(tiles_map[i].focused)
                                    {
                                        tiles_map[i].focused = false;
                                        has_focused = false;
                                    }
                                    else    
                                        tiles_map[i].focused = true;

                                    if(aux_position.x == cam_obj.world_cords.x && aux_position.y == cam_obj.world_cords.z)
                                    {
                                        tiles_map[i].focused = false;
                                        has_focused = false;
                                    }
                                }
                                else
                                    tiles_map[i].focused = false;
                                    
                            }

                            if(!(aux_position.x < ortho_value.x))
                                has_focused = false; 

                            if(!has_focused)
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

                            switch (aux_obj.obj_type)
                            {
                            case CUBE:
                                aux_obj.polygon_vertexs = obj_loader("../obj/cube.obj");
                                break;
                            case PYRAMID:
                                aux_obj.polygon_vertexs = obj_loader("../obj/pyramid.obj");
                                break;
                            case ETHER:
                                aux_obj.polygon_vertexs = obj_loader("../obj/ether.obj");
                                break;
                            case CONE:
                                aux_obj.polygon_vertexs = obj_loader("../obj/cone.obj");
                                break;
                            }
                            
                            for(int32_t i = 0; i < tiles_map.size(); i++)
                                if(tiles_map[i].world_cords.x == aux_obj.world_cords.x &&
                                   tiles_map[i].world_cords.y == aux_obj.world_cords.z &&
                                   !tiles_map[i].has_object   && object_list.size() < tiles_map.size() - 1
                                )
                                    object_list.push_back(aux_obj);
                            break;
                            case sf::Keyboard::Delete:
                                for(int32_t i = 1; i < object_list.size(); i++)
                                {
                                    if(object_list[i].world_cords.x == selected_tile.x   &&
                                    object_list[i].world_cords.z == selected_tile.y   && 
                                    tiles_map[selected_tile.z].has_object  &&
                                    tiles_map[selected_tile.z].selected)
                                        object_list.erase(object_list.begin() + i);
                                }
                                break;
                        case sf::Keyboard::Right:
                            for(int32_t i = 0; i < object_list.size(); i++)
                            {
                                if(object_list[i].world_cords.x == selected_tile.x   &&
                                   object_list[i].world_cords.z == selected_tile.y   && 
                                   tiles_map[selected_tile.z].has_object  &&
                                   tiles_map[selected_tile.z].selected)
                                {
                                    if(object_list[i].world_cords.x + 1 <= ortho_value.x)
                                    {
                                        for(int32_t j = 0; j < tiles_map.size(); j++)
                                        {
                                            if(tiles_map[j].world_cords.x == object_list[i].world_cords.x + 1 &&
                                               tiles_map[j].world_cords.y == object_list[i].world_cords.z     &&
                                               !tiles_map[j].has_object)
                                            {   
                                                tiles_map[selected_tile.z].selected   = false;
                                                tiles_map[selected_tile.z].has_object = false;
                                                
                                                tiles_map[j].selected   = true;
                                                tiles_map[j].has_object = true;

                                                if(tiles_map[selected_tile.z].world_cords.x == focused_object.x &&
                                                   tiles_map[selected_tile.z].world_cords.y == focused_object.z
                                                )
                                                {
                                                    tiles_map[selected_tile.z].focused   = false;
                                                    tiles_map[j].focused                 = true;
                                                    focused_object.x++;
                                                }
                                            }
                                        }

                                        if(!tiles_map[selected_tile.z].selected)
                                            object_list[i].world_cords.x++;
                                
                                    }
                                }
                            }
                            break;
                        case sf::Keyboard::Left:
                            for(int32_t i = 0; i < object_list.size(); i++)
                            {
                                if(object_list[i].world_cords.x == selected_tile.x   &&
                                   object_list[i].world_cords.z == selected_tile.y   && 
                                   tiles_map[selected_tile.z].has_object  &&
                                   tiles_map[selected_tile.z].selected)
                                {
                                    if(object_list[i].world_cords.x - 1 >= 0)
                                    {
                                        for(int32_t j = 0; j < tiles_map.size(); j++)
                                        {
                                            if(tiles_map[j].world_cords.x == object_list[i].world_cords.x - 1 &&
                                               tiles_map[j].world_cords.y == object_list[i].world_cords.z     &&
                                               !tiles_map[j].has_object)
                                            {
                                                tiles_map[selected_tile.z].selected   = false;
                                                tiles_map[selected_tile.z].has_object = false;
                                                
                                                tiles_map[j].selected   = true;
                                                tiles_map[j].has_object = true;

                                                if(tiles_map[selected_tile.z].world_cords.x == focused_object.x &&
                                                   tiles_map[selected_tile.z].world_cords.y == focused_object.z
                                                )
                                                {
                                                    tiles_map[selected_tile.z].focused   = false;
                                                    tiles_map[j].focused                 = true;
                                                    focused_object.x--;
                                                }
                                            }
                                        }

                                        if(!tiles_map[selected_tile.z].selected)
                                            object_list[i].world_cords.x--;

                                    }
                                }
                            }
                            break;
                        case sf::Keyboard::Down:
                            for(int32_t i = 0; i < object_list.size(); i++)
                            {
                                if(object_list[i].world_cords.x == selected_tile.x   &&
                                   object_list[i].world_cords.z == selected_tile.y   && 
                                   tiles_map[selected_tile.z].has_object  &&
                                   tiles_map[selected_tile.z].selected)
                                {
                                    if(object_list[i].world_cords.z + 1 <= ortho_value.y)
                                    {
                                        for(int32_t j = 0; j < tiles_map.size(); j++)
                                        {
                                            if( tiles_map[j].world_cords.x == object_list[i].world_cords.x     &&
                                                tiles_map[j].world_cords.y == object_list[i].world_cords.z + 1 &&
                                               !tiles_map[j].has_object)
                                            {
                                                tiles_map[selected_tile.z].selected   = false;
                                                tiles_map[selected_tile.z].has_object = false;
                                                
                                                tiles_map[j].selected   = true;
                                                tiles_map[j].has_object = true;

                                                if(tiles_map[selected_tile.z].world_cords.x == focused_object.x &&
                                                   tiles_map[selected_tile.z].world_cords.y == focused_object.z
                                                )
                                                {
                                                    tiles_map[selected_tile.z].focused   = false;
                                                    tiles_map[j].focused                 = true;
                                                    focused_object.z++;
                                                }
                                            }
                                        }

                                        if(!tiles_map[selected_tile.z].selected)
                                            object_list[i].world_cords.z++;

                                    }
                                }
                            }
                            break;
                        case sf::Keyboard::Up:
                            for(int32_t i = 0; i < object_list.size(); i++)
                            {
                                if(object_list[i].world_cords.x == selected_tile.x   &&
                                   object_list[i].world_cords.z == selected_tile.y   && 
                                   tiles_map[selected_tile.z].has_object  &&
                                   tiles_map[selected_tile.z].selected)
                                {
                                    if(object_list[i].world_cords.z - 1 >= 0)
                                    {
                                        for(int32_t j = 0; j < tiles_map.size(); j++)
                                        {
                                            if(tiles_map[j].world_cords.x == object_list[i].world_cords.x     &&
                                               tiles_map[j].world_cords.y == object_list[i].world_cords.z - 1 &&
                                               !tiles_map[j].has_object)
                                            {
                                                tiles_map[selected_tile.z].selected   = false;
                                                tiles_map[selected_tile.z].has_object = false;
                                                
                                                tiles_map[j].selected   = true;
                                                tiles_map[j].has_object = true;

                                                if(tiles_map[selected_tile.z].world_cords.x == focused_object.x &&
                                                   tiles_map[selected_tile.z].world_cords.y == focused_object.z
                                                )
                                                {
                                                    tiles_map[selected_tile.z].focused   = false;
                                                    tiles_map[j].focused                 = true;
                                                    focused_object.z--;
                                                }
                                            }
                                        }

                                        if(!tiles_map[selected_tile.z].selected)
                                            object_list[i].world_cords.z--;

                                    }
                                }
                            }
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

        cam_obj.world_cords = object_list[0].world_cords;

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
