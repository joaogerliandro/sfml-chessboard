#include <config_loader.hpp>

sf::ContextSettings settings;

enum CONFIG_TYPE : int32_t
{
    CUSTOM,
    DEFAULT
};

void load_config(const char* custom_path = "config.ini", CONFIG_TYPE config_type = CUSTOM)
{
    std::fstream config_file;

    switch (config_type)
    {
        case CUSTOM:
            config_file.open(custom_path);

            if(!config_file.is_open())
                //create file and auto-config
                throw "ERRO: Can't open the file !";

            break;

        case DEFAULT:
            settings.depthBits         = 24;
            settings.stencilBits       =  8;
            settings.antialiasingLevel =  4;
            settings.majorVersion      =  3;
            settings.minorVersion      =  0;
            break;

        default:
            throw "ERRO: Invalid Config Type !";
            break;
    }

    config_file.close();
}

sf::ContextSettings get_settings()
{
    return settings;
}