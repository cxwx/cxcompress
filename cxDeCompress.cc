// cspell:disable
#include "cxDeCompress.hh"
#include <spdlog/spdlog.h>
#include <boost/filesystem.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/lzma.hpp>
#include <boost/iostreams/filter/zstd.hpp>
#include <boost/iostreams/filter/zlib.hpp>
using namespace std;

namespace cxfunc {
OpenFile::OpenFile(const YAML::Node& _node) : cxObject(_node) {
  if (theYamlNode["filename"]) {
    auto filename = theYamlNode["filename"].as<string>();
    string ext = boost::filesystem::path(filename).extension().string();
    theInputStream.open(filename, ios_base::in | ios_base::binary);
    if (ext == ".gz") {
      theBoostFilter.push(boost::iostreams::gzip_decompressor());
      if (theVerbose > 0)
        spdlog::debug("open gzip file {}", filename);
    } else if (ext == string(".bz2")) {
      theBoostFilter.push(boost::iostreams::bzip2_decompressor());
      if (theVerbose > 0)
        spdlog::debug("open bzip2 file {}", filename);
    } else if (ext == string(".xz")) {
      theBoostFilter.push(boost::iostreams::lzma_decompressor());
      if (theVerbose > 0)
        spdlog::debug("open lzma file {}", filename);
    } else if (ext == string(".zst")) {  // NEEDCHECK(CX):
      theBoostFilter.push(boost::iostreams::zstd_decompressor());
      if (theVerbose > 0)
        spdlog::debug("open lzma file {}", filename);
    } else if (ext == string(".zlib")) {  // NEEDCHECK(CX):
      theBoostFilter.push(boost::iostreams::zlib_decompressor());
      if (theVerbose > 0)
        spdlog::debug("open lzma file {}", filename);
    } else if (ext == string(".dat")) {
      if (theVerbose > 0)
        spdlog::debug("open dat file {}", filename);
      theOutputStream = &theInputStream;  // bug until
      return;
    } else {
      spdlog::warn("working not zip file??? need future design |{}|", ext);
      theOutputStream = &theInputStream;
      return;
    }
    theBoostFilter.push(theInputStream);
    theOutputStream = new std::istream(&theBoostFilter);
  } else {
    theOutputStream = &std::cin;
    return;
  }
}
OpenFile::~OpenFile() {
  if (theOutputStream == &std::cin) {
  } else {
    spdlog::warn("closing file {:08d}", theInputStream.is_open());
    // theBoostFilter.pop(); ??gptbug
    theBoostFilter.reset();  //?? need check
    theInputStream.close();
  }
}
}
