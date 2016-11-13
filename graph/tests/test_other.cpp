#ifndef _PRIMITIVES_H_
#include "../primitives.hpp"
#endif

#ifndef _GRAPHUTILS_H_
#include "../graphutils.hpp"
#endif

#include <cstring>          /* for strlen, strcpy and memset */
#include <cstdlib>          /* for itoa */
#include <climits>
#include <assert.h>
#include <sstream>          /* For to_string */



int main(){
    GDBUtils::GdbTester test;
    GdbString x = test.getTimestamp();
    x += "This is my first log entry";
    printf("GdbTester outputs: %s \n",test.objDisplay());
    GDBUtils::GdbTester::restart_log();
    GDBUtils::GdbTester::log(x.cstr());

}