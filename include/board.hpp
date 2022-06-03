#pragma once

#include <vector>

#include <tile.hpp>
#include <object.hpp>

#include <glm/ext.hpp>

#include <gl_init.hpp>

typedef struct
{
    std::vector<Tile>   tiles_map;
    std::vector<Object> object_list;

    glm::vec3 selected_tile,
              focused_object = init_focused;

    bool fullscreen    = false,
         tiles_maped   = false,
         has_selected  = false,
         has_focused   = false;

} Board;