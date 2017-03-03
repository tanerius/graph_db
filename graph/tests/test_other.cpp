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
    GDBUtils::GdbBinFile* BinaryFile = new GDBUtils::GdbBinFile();
    std::cout << BinaryFile->ObjDisplay();
    delete BinaryFile;
    return 0;
}