PicoJSON - a C++ JSON parser / serializer
Copyright © 2009-2010 Cybozu Labs, Inc. Copyright © 2011-2015 Kazuho Oku

Licensed under 2-clause BSD license

Version

1.3.1-dev Build Status

Introduction

PicoJSON is a tiny JSON parser / serializer for C++ with following properties:

header-file only
no external dependencies (only uses standard C++ libraries)
STL-frendly (arrays are represented by using std::vector, objects are std::map)
provides both pull interface and streaming (event-based) interface
Reading JSON using the pull interface

There are several ways to use the pull (DOM-like) interface of picojson.

The easiest way is to use the two-argument parse function.

std::string json = "[ \"hello JSON\" ]";
picojson::value v;
std::string err = picojson::parse(v, json);
if (! err.empty()) {
  std:cerr << err << std::endl;
}
Four-argument parse function accepts a pair of iterators, and returns the end position of the input.

const char* json = "{\"a\":1}";
picojson::value v;
std::string err;
const char* json_end = picojson::parse(v, json, json + strlen(json), &err);
if (! err.empty()) {
  std::cerr << err << std::endl;
}
std::istream_iterator input(std::cin);
picojson::value v;
std::string err;
input = picojson::parse(v, input, std::istream_iterator(), &err);
if (! err.empty()) {
  std::cerr << err << std::endl;
}
It is also possible to use the >> operator to parse the input, however this interface is not thread-safe.

picosjon::value v;
std::cin >> v;
std::string err = picojson::get_last_error();
Accessing the values

Values of a JSON object is represented as instances of picojson::value class.

namespace picojson {

  class value {
    ...

  public:

    typedef std::vector<value> array;
    typedef std::map<std::string, value> object;

    value();                               // create a null object
    explicit value(bool b);                // create a boolean object
    explicit value(double n);              // create a number object
    explicit value(const std::string& s);  // create a string object
    explicit value(const array& a);        // create an array object
    explicit value(const object& o);       // create an "object"

    bool is<picojson::null>() const;       // check if the object is "null"

    bool is<bool>() const;                 // check if the object is a boolean
    const bool& get<bool>() const;         // const accessor (usable only if the object is a boolean)
    bool& get<bool>();                     // non-const accessor (usable only if the object is a boolean)

    bool is<double>() const;               // check if the object is a number
    const double& get<double>() const;     // const accessor (usable only if the object is a number)
    double& get<double>();                 // non-const accessor (usable only if the object is a number)

    bool is<std::string>() const;          // check if the object is a string
    const std::string& get<std::string>() const;
                                           // const accessor (usable only if the object is a string)
    std::string& get<std::string>();       // non-const accessor (usable only if the object is a string)

    bool is<array>() const;                // check if the object is an array
    const array& get<array>() const;       // const accessor (usable only if the object is an array)
    array& get<array>();                   // non-const accessor (usable only if the object is an array)

    bool is<object>() const;               // check if the object is an "object"
    const object& get<object>() const;     // const accessor (usable only if the object is an object)
    object& get<object>();                 // non-const accessor (usable only if the object is an array)

    bool evaluate_as_boolean() const;      // evaluates the object as a boolean

    std::string serialize() const;         // returns the object in JSON representation
    template void serialize(Iter os) const;
                                           // serializes the object in JSON representation through an output iterator

    std::string to_str() const;            // returns the object in string (for casual use)

  };

}
The code below parses a JSON string and prints the contents of the object.

picojson::value v;

// parse the input
std::cin >> v;
std::string err = picojson::get_last_error();
if (! err.empty()) {
  std::cerr << err << std::endl;
  exit(1);
}

// check if the type of the value is "object"
if (! v.is<picojson::object>()) {
  std::cerr << "JSON is not an object" << std::endl;
  exit(2);
}

// obtain a const reference to the map, and print the contents
const picojson::value::object& obj = v.get<picojson::object>();
for (picojson::value::object::const_iterator i = obj.begin();
     i != obj.end();
     ++i) {
  std::cout << i->first << ': ' << i->second.to_str() << std::endl;
}
Please note that the type check is mandatory; do not forget to check the type of the object by calling is<type>() before accessing the value by calling get<type>().

Reading JSON using the streaming (event-driven) interface

Please refer to the implementation of picojson::default_parse_context and picojson::null_parse_context. There is also an example (examples/streaming.cc) .

Serializing to JSON

Instances of the picojson::value class can be serialized in three ways, to ostream, to std::string, or to an output iterator.

picojson::value v;
...
std::cout << v;
picojson::value v;
...
std::string json = v.serialize();
picojson::value v;
...
v.serialize(std::ostream_iterator(std::cout));
Experimental support for int64_t

Experimental suport for int64_t becomes available if the code is compiled with preprocessor macro PICOJSON_USE_INT64.

Turning on the feature will cause following changes to picojson:

new constructor picojson::value(int64_t) is defined
is<int64_t>() and get<int64_t>() become available
numerics in JSON within the bounds of int64_t and not using . nor e/E are considered as int64 type
the values are also avaliable as doubles as well (i.e. all values which are .is<int64_t>() == true are also .is<double>() == true)
int64 values are converted to double once get<double>() is called
Enabling the feature should not cause compatibility problem with code that do not use the feature.