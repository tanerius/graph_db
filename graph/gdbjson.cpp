
#include "gdbjson.hpp"



void GdbJson::readJson(const char* _value){
    std::string json = _value;
    std::istringstream is (json);
    read_json (is, root_element);
    
    // Write json.
  //ptree pt;
  //pt.put ("foo", "bar");
  //std::ostringstream buf; 
  //write_json (buf, pt, false);
  //std::string json = buf.str(); // {"foo":"bar"}
  //printf("The entire json string %s\n", json.c_str());
  
} 

GdbString GdbJson::getValueString(const char* _value){
     GdbString json_string = root_element.get<std::string>(_value).c_str();
     return json_string;
}

GdbVariant GdbJson::getValueFloat(const char* _value){
    GdbVariant x;
    return x;
}

GdbVariant GdbJson::getValueInt(const char* _value){
    GdbVariant x;
    return x;
}
