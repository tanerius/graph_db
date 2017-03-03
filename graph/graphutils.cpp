/*graphutils.cpp*/
#include "graphutils.hpp"

using namespace GDBUtils;


/* ********************************************************
    GdbObject
******************************************************* **/

GdbVariant* GdbObject::GetTimer() 
{
    GdbVariant* TimerResult = new GdbVariant(event_diff.count());
    return TimerResult; 
}

void GdbObject::TimerStart() 
{
    event_start = std::chrono::high_resolution_clock::now();
}

void GdbObject::TimerLap()
{
    event_end = std::chrono::high_resolution_clock::now();
    event_diff = event_end - event_start;
}


/* ********************************************************
    GdbBinFile
******************************************************* **/
const char* GdbBinFile::ObjDisplay()
{
    return "ObjDisplay displaying the object";
}

const char* GdbBinFile::ObjGetID()
{
    return "ObjGetID: 1";
}
