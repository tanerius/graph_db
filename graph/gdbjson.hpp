/*config.h*/
#ifndef _GDBJSON_HPP_
#define _GDBJSON_HPP_

#ifndef _GDBSTRING_H_
#include "gdbstring.hpp"
#endif

#include "boost/config/include/boost/config/user.hpp"
#include "boost/config/include/boost/config.hpp"
#include "boost/property_tree/include/boost/property_tree/ptree_fwd.hpp"
#include "boost/property_tree/include/boost/property_tree/ptree.hpp"
#include "boost/property_tree/include/boost/property_tree/json_parser.hpp"

// alias the namespace for convenience
namespace boost_pt = boost::property_tree;

class GdbJson {
    private:
      boost_pt::ptree root_element;
    protected:
    public:
      GdbVariant getValueInt(const char* _value);
      GdbVariant getValueFloat(const char* _value);
      GdbString getValueString(const char* _value);
      void readJson(const char* _value, const bool _is_file); 
        
};

#endif