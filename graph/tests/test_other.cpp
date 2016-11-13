#ifndef _PRIMITIVES_H_
#include "../primitives.hpp"
#endif
#include <cstring>          /* for strlen, strcpy and memset */
#include <cstdlib>          /* for itoa */
#include <climits>
#include <assert.h>
#include <sstream>          /* For to_string */

#include <boost/log/include/boost/log/common.hpp>
#include <boost/log/include/boost/log/expressions.hpp>

#include <boost/log/include/boost/log/utility/setup/file.hpp>
#include <boost/log/include/boost/log/utility/setup/console.hpp>
#include <boost/log/include/boost/log/utility/setup/common_attributes.hpp>

#include <boost/log/include/boost/log/attributes/timer.hpp>
#include <boost/log/include/boost/log/attributes/named_scope.hpp>

#include <boost/log/include/boost/log/sources/logger.hpp>

#include <boost/log/include/boost/log/support/date_time.hpp>


int main(){
    printf("TESTING RANDOM STUFF \n");
    if(nullptr){
        printf("nullptr evaluates true\n");
    }
    else{
        printf("nullptr evaluates false\n");
    }
}