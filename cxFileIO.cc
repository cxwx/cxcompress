// cspell:disable
#include "cxFileIO.hh"
#include <boost/iostreams/categories.hpp>
#include <iostream>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/lzma.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filter/zstd.hpp>
#include <filesystem>
#include <stdexcept>

namespace cxfunc {

namespace {

// Delegates to a streambuf but never closes it.
// Prevents filtering_streambuf::reset() from closing streams we don't own.
struct non_closing_source {
  using char_type = char;
  using category = boost::iostreams::source_tag;

  std::streambuf* buf;
  explicit non_closing_source(std::streambuf* b) : buf(b) {}

  std::streamsize read(char* s, std::streamsize n) {
    std::streamsize got = buf->sgetn(s, n);
    return got == 0 ? -1 : got;
  }
  void close() {}
};

FileFormat guess_format(const std::string& filename) {
  auto ext = std::filesystem::path(filename).extension().string();
  if (ext == ".gz") return FileFormat::kGzip;
  if (ext == ".bz2") return FileFormat::kBzip2;
  if (ext == ".xz") return FileFormat::kLzma;
  if (ext == ".zst") return FileFormat::kZstd;
  if (ext == ".zlib") return FileFormat::kZlib;
  return FileFormat::kPlain;
}

void push_decompressor(boost::iostreams::filtering_streambuf<boost::iostreams::input>& f,
                       FileFormat fmt) {
  switch (fmt) {
    case FileFormat::kGzip: f.push(boost::iostreams::gzip_decompressor()); break;
    case FileFormat::kBzip2: f.push(boost::iostreams::bzip2_decompressor()); break;
    case FileFormat::kLzma: f.push(boost::iostreams::lzma_decompressor()); break;
    case FileFormat::kZstd: f.push(boost::iostreams::zstd_decompressor()); break;
    case FileFormat::kZlib: f.push(boost::iostreams::zlib_decompressor()); break;
    case FileFormat::kAuto:
    case FileFormat::kPlain: break;
  }
}

}  // namespace

cxFileIO::cxFileIO() {
  theFilter.push(non_closing_source{std::cin.rdbuf()});
}

cxFileIO::cxFileIO(const std::string& filename, FileFormat fmt) {
  theFileBuf.open(filename, std::ios_base::in | std::ios_base::binary);
  if (!theFileBuf.is_open()) {
    throw std::runtime_error("cannot open file: " + filename);
  }

  if (fmt == FileFormat::kAuto) fmt = guess_format(filename);
  push_decompressor(theFilter, fmt);
  theFilter.push(theFileBuf);
}

cxFileIO::cxFileIO(std::istream& stream, FileFormat fmt) {
  push_decompressor(theFilter, fmt);
  theFilter.push(non_closing_source{stream.rdbuf()});
}

cxFileIO::~cxFileIO() {
  theFilter.reset();
}

}  // namespace cxfunc
