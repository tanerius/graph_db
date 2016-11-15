/*graphutils.cpp*/
#include "graphutils.hpp"

using namespace GDBUtils;

bool GdbBinFile::setFile(const char* newFile)
{
    if (file_mutex.try_lock())
    {
        if(checkOpen())
        {
            fclose(m_pFile);
            m_pFile = nullptr;
        }
        m_fileName = newFile;
        file_mutex.unlock();
        return true;
    }

    // mutex couldnt be aquired
    return false;
}