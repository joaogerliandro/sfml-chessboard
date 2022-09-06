#include <config_loader.hpp>

sf::ContextSettings settings;

void write_cfg_template(std::fstream& cfg_file)
{
    cfg_file << "DepthBits = 24\n";
    cfg_file << "StencilBits = 8\n";
    cfg_file << "AntialiasingLevel = 4\n";
    cfg_file << "GlMajorVersion = 3\n";
    cfg_file << "GlMinorVersion = 0\n";
}

void read_config(std::fstream& cfg_file)
{
    std::string  file_line;

    while(std::getline(cfg_file, file_line))
    {
        std::cout << file_line << std::endl;
    }
}

void load_config(const char* file_path)
{
    std::fstream config_file(file_path);

    if(!config_file.is_open())
    {
        std::cout << "WARNING: Could not read settings file ! Creating a new ..." << std::endl;

        config_file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::trunc);
        
        if(!config_file.is_open())
            throw "ERROR: Unable to create settings file !";

        write_cfg_template(config_file);
    }
    
    read_config(config_file);

    config_file.close();
}

sf::ContextSettings get_settings()
{
    settings.depthBits         = 24;
    settings.stencilBits       =  8;
    settings.antialiasingLevel =  4;
    settings.majorVersion      =  3;
    settings.minorVersion      =  0;

    return settings;
}