#ifndef DECOMPRESS_CX___
#define DECOMPRESS_CX___

#include "cxObject.hh"
#include <boost/iostreams/filtering_streambuf.hpp>
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

namespace cxfunc {
class OpenFile : public cxObject {
 public:
  std::ifstream theInputStream;
  boost::iostreams::filtering_streambuf<boost::iostreams::input> theBoostFilter;
  std::istream* theOutputStream;

  explicit OpenFile(const YAML::Node& _node);
  ~OpenFile();
};

}  // namespace cxFunc

#endif
