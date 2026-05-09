#ifndef CXFILEIO_HH_
#define CXFILEIO_HH_

#include <boost/iostreams/filtering_streambuf.hpp>
#include <fstream>
#include <string>

namespace cxfunc {

enum class FileFormat { kAuto, kPlain, kGzip, kBzip2, kLzma, kZstd, kZlib };

class cxFileIO {
 public:
  explicit cxFileIO(const std::string& filename, FileFormat fmt = FileFormat::kAuto);
  explicit cxFileIO(std::istream& stream, FileFormat fmt = FileFormat::kPlain);
  cxFileIO();  // reads from standard input
  ~cxFileIO();

  cxFileIO(const cxFileIO&) = delete;
  auto operator=(const cxFileIO&) -> cxFileIO& = delete;

  auto stream() -> std::istream& { return theStream; }

 private:
  std::ifstream theFileBuf;
  boost::iostreams::filtering_streambuf<boost::iostreams::input> theFilter;
  std::istream theStream{&theFilter};
};

}  // namespace cxfunc

#endif
