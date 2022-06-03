#pragma once

#include <vector>

#include <tile.hpp>
#include <object.hpp>

#include <glm/ext.hpp>


typedef struct
{
    std::vector<Tile>   tiles_map;
    std::vector<Object> object_list;

} Board;