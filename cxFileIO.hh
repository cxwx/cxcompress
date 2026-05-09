#ifndef CXFILEIO_HH_
#define CXFILEIO_HH_

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/categories.hpp>
#include <fstream>
#include <iostream>
#include <string>

namespace cxfunc {

enum class FileFormat { kAuto, kPlain, kGzip, kBzip2, kLzma, kZstd, kZlib };

// Boost iostreams Source that delegates to a streambuf but never closes it.
// Prevents filtering_streambuf::reset() from closing streams we don't own
// (e.g. std::cin, user-provided istream).
struct non_closing_source {
  using char_type = char;
  using category = boost::iostreams::source_tag;

  std::streambuf* buf;

  explicit non_closing_source(std::streambuf* b) : buf(b) {}

  std::streamsize read(char* s, std::streamsize n) {
    std::streamsize got = buf->sgetn(s, n);
    return got == 0 ? -1 : got;
  }

  void close() {}  // no-op: we don't own the underlying streambuf
};

class cxFileIO {
 public:
  explicit cxFileIO(const std::string& filename, FileFormat fmt = FileFormat::kAuto);
  explicit cxFileIO(std::istream& stream, FileFormat fmt = FileFormat::kPlain);
  cxFileIO();  // reads from stdin
  ~cxFileIO();

  cxFileIO(const cxFileIO&) = delete;
  cxFileIO& operator=(const cxFileIO&) = delete;

  std::istream& stream() { return theStream; }

 private:
  std::ifstream theFileBuf;
  boost::iostreams::filtering_streambuf<boost::iostreams::input> theFilter;
  std::istream theStream{&theFilter};
};

}  // namespace cxfunc

#endif
