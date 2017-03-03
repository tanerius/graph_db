/*graphutils.cpp*/
#include "graphutils.hpp"
#ifndef _GDBSTRING_H_
#include "gdbstring.hpp"
#endif


/* ********************************************************
    GdbObject
******************************************************* **/

GdbVariant* GDBUtils::GdbObject::GetTimer() 
{
    GdbVariant* TimerResult = new GdbVariant(event_diff.count());
    return TimerResult; 
}

void GDBUtils::GdbObject::TimerStart() 
{
    event_start = std::chrono::high_resolution_clock::now();
}

void GDBUtils::GdbObject::TimerLap()
{
    event_end = std::chrono::high_resolution_clock::now();
    event_diff = event_end - event_start;
}


/* ********************************************************
    GdbBinFile
******************************************************* **/
bool GDBUtils::GdbBinFile::CheckOpen()
{
    return (m_pFile || m_fileName);
}

const char* GDBUtils::GdbBinFile::ObjDisplay()
{
    return "ObjDisplay displaying the object";
}

const char* GDBUtils::GdbBinFile::ObjGetID()
{
    return "ObjGetID: 1";
}
