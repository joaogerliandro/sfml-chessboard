#pragma once

#include <glm/ext.hpp>

typedef struct tile
{
    glm::vec2 side_x, side_y;

    glm::vec2 world_cords;

    bool      selected   = false,
              focused    = false,
              has_object = false;
} Tile;