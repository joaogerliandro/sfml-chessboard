#pragma once

#include <vector>

#include <tile.hpp>
#include <object.hpp>

#include <glm/ext.hpp>


typedef struct
{
    std::vector<Tile>   tiles_map;
    std::vector<Object> object_list;

    std::vector<Meshe> loaded_objs;

    bool fullscreen    = false,
         tiles_maped   = false,
         has_selected  = false,
         has_focused   = false;

} Board;