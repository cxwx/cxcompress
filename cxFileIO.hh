#ifndef CXFILEIO_HH_
#define CXFILEIO_HH_

#include <boost/iostreams/filtering_streambuf.hpp>
#include <fstream>
#include <string>

namespace cxfunc {

enum class cxFileIOFormat : std::uint8_t { kAuto, kPlain, kGzip, kBzip2, kLzma, kZstd, kZlib };

class cxFileIO {
 public:
  explicit cxFileIO(const std::string& filename, cxFileIOFormat fmt = cxFileIOFormat::kAuto);
  explicit cxFileIO(std::istream& stream, cxFileIOFormat fmt = cxFileIOFormat::kPlain);
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
