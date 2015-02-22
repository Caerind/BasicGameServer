#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <fstream>
#include <map>
#include <sstream>
#include <string>

#include <iostream>

class Settings
{
    public:
        Settings();

        bool loadFromFile(std::string const& name);
        bool saveToFile(std::string const& name);
        void createDefault();

        std::string getString(std::string const& id);
        int getInt(std::string const& id);
        float getFloat(std::string const& id);
        bool getBool(std::string const& id);

        void set(std::string const& id, std::string const& value);
        void set(std::string const& id, int value);
        void set(std::string const& id, float value);
        void set(std::string const& id, bool value);

    protected:
        std::map<std::string,std::string> mSettings;
};

#endif // SETTINGS_HPP
