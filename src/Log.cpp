#include "../include/Log.hpp"

Log::Log()
{
}

bool Log::open(std::string const& filename)
{
    mFile.open(filename);
    return mFile.is_open();
}

bool Log::write(std::string const& line)
{
    if (mFile.is_open())
    {
        mFile << line << std::endl;
        return true;
    }
    else
    {
        return false;
    }
}

bool Log::isOpen() const
{
    return mFile.is_open();
}
