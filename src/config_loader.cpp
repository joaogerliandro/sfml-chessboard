#include <config_loader.hpp>

sf::ContextSettings settings;

void write_cfg_template(std::fstream& cfg_file)
{
    cfg_file << "DepthBits = " << settings.depthBits << "\n";
    cfg_file << "StencilBits = " << settings.stencilBits << "\n";
    cfg_file << "AntialiasingLevel = " << settings.antialiasingLevel << "\n";
    cfg_file << "GlMajorVersion = " << settings.majorVersion << "\n";
    cfg_file << "GlMinorVersion = " << settings.minorVersion << "\n";
    cfg_file << "SRgbCapable = " << settings.sRgbCapable << "\n";
}

bool is_number(const std::string& str)
{
    std::string::const_iterator str_iterator = str.begin();

    while (str_iterator != str.end() && std::isdigit(*str_iterator)) 
        ++str_iterator;

    return !str.empty() && str_iterator == str.end();
}

uint32_t read_value(std::stringstream& str_stream, std::string option)
{
    std::string line_content;
    str_stream >> line_content;

    if(line_content == "=")
    {
        str_stream >> line_content;
        if(is_number(line_content))
            return std::atoi(line_content.c_str());
    }
    else
        std::cout << "WARNING: Could not load " << option << " configuration ! The default value will be kept." << std::endl;
}

void read_config(std::fstream& cfg_file)
{
    std::string file_line;
    std::string line_content;

    std::stringstream str_stream;

    while(std::getline(cfg_file, file_line))
    {
        str_stream.clear();
        str_stream.str(file_line);

        str_stream >> line_content;

        if(line_content == "DepthBits")
            settings.depthBits = read_value(str_stream, line_content);
        else if(line_content == "StencilBits")
            settings.stencilBits = read_value(str_stream, line_content);
        else if(line_content == "AntialiasingLevel")
            settings.antialiasingLevel = read_value(str_stream, line_content);
        else if(line_content == "GlMajorVersion")
            settings.majorVersion = read_value(str_stream, line_content);
        else if(line_content == "GlMinorVersion")
            settings.minorVersion = read_value(str_stream, line_content);
        else if(line_content == "SRgbCapable")
            settings.sRgbCapable = read_value(str_stream, line_content);
    }
}

void load_default_setting()
{
    settings.depthBits         = 24;
    settings.stencilBits       =  8;
    settings.antialiasingLevel =  4;
    settings.majorVersion      =  3;
    settings.minorVersion      =  0;

    settings.sRgbCapable = false;
}

void load_config(const char* file_path)
{
    load_default_setting();

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
    return settings;
}