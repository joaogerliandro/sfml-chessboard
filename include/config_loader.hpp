#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>

#include <SFML/Window/ContextSettings.hpp>

void load_config(const char* file_path = "config.cfg");

sf::ContextSettings get_settings();