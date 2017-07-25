#pragma once
/// \file
///
/// STereoLithography Parser. This replaces the old Boost.Spirit-based parser
/// because it took forever to compile.
///
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/io/file_system.hpp>
#include <hm3/utility/string_parsing.hpp>

namespace hm3::geometry::io {

/// STereoLithography I/O
namespace stl {

/// Vector type:
using vec_t = array<float, 3>;

/// STL triangle layout for serialization.
///
/// \note STL format works with 32-bit floating-point numbers only.
struct triangle {
  /// Number of bytes in the raw storage:
  static constexpr suint_t no_bytes =  //
   1 * sizeof(vec_t) +                 // normal vector
   3 * sizeof(vec_t) +                 // 3 vertices
   sizeof(std::uint16_t);              // attribute
  static_assert(no_bytes == 50);

  /// Raw storage for direct serialization of the triangle:
  char data_[no_bytes]{};

  /// Triangle normal vector
  vec_t& normal() noexcept { return *reinterpret_cast<vec_t*>(data_); };
  /// Triangle normal vector
  vec_t const& normal() const noexcept {
    return const_cast<triangle&>(*this).normal();
  };
  /// Triangle vertices
  array<vec_t, 3>& vertices() noexcept {
    return *reinterpret_cast<array<vec_t, 3>*>(data_ + 12);
  }
  /// Triangle vertices
  array<vec_t, 3> const& vertices() const noexcept {
    return const_cast<triangle&>(*this).vertices();
  }
  /// Triangle attribute
  std::uint16_t& attribute() noexcept {
    return *reinterpret_cast<std::uint16_t*>(data_ + 48);
  };
  /// Triangle attribute
  std::uint16_t const& attribute() const noexcept {
    return const_cast<triangle&>(*this).attribute();
  }

  constexpr triangle()                = default;
  constexpr triangle(triangle const&) = default;
  constexpr triangle(triangle&&)      = default;
  ~triangle()                         = default;

  constexpr triangle& operator=(triangle const& other) noexcept {
    for (suint_t i = 0; i < no_bytes; ++i) { data_[i] = other.data_[i]; }
    return *this;
  }
  constexpr triangle& operator=(triangle&& other) noexcept {
    for (suint_t i = 0; i < no_bytes; ++i) {
      data_[i] = std::move(other.data_[i]);
    }
    return *this;
  }

  triangle(vec_t n, array<vec_t, 3> vxs, std::uint16_t a = 0) noexcept {
    normal()    = n;
    vertices()  = vxs;
    attribute() = a;
  }
};

template <typename OStream>
OStream& operator<<(OStream& os, triangle const& t) {
  os << "{ stl::triangle | n: " << view::all(t.normal())
     << ", vx[0]: " << view::all(t.vertices()[0])  //
     << ", vx[1]: " << view::all(t.vertices()[1])  //
     << ", vx[2]: " << view::all(t.vertices()[2])  //
     << "}";
  return os;
}

/// Two STL triangles are equal if their normal vectors and vertices are equal.
///
/// The triangle attributes are ignored from equality. In general they contain
/// garbage, depending on the source of the STL file.
inline bool operator==(triangle const& a, triangle const& b) noexcept {
  num_t rel_float_tol = 1e-7;
  num_t abs_float_tol = 1e-6;

  auto eq = [&](auto&& i, auto&& j) {
    return approx_number(num_t{i}, num_t{j}, abs_float_tol, rel_float_tol);
  };

  return ranges::equal(a.normal(), b.normal(), eq)
         && ranges::equal(
             a.vertices(), b.vertices(),
             [&](auto&& vi, auto&& vj) { return ranges::equal(vi, vj, eq); });
}
inline bool operator!=(triangle const& a, triangle const& b) noexcept {
  return !(a == b);
}

/// STL concepts
namespace concept {

/// These are used for optimizations. If a source is bulk writable/readable we
/// can write/read a sequence of triangles in one go. Otherwise we need to
/// write/read triangles one by one.

/// TODO: Bulk... should refine Single... since if a source can be written in
/// bulk this means that triangles can also be written one by one.

/// A source is BulkWritable if it is a ContiguousRange of triangles.
struct BulkWritable {
  template <typename T>
  static auto requires_(T&& t) -> decltype(  //
   rc::valid_expr(                           //
    ranges::size(t),                         //
    (triangle const*)((T &&) t).data()       //
    )                                        //
  );
};

/// A source is BulkReadable if it is a resizable ContiguousRange of triangles.
struct BulkReadable {
  template <typename T>
  static auto requires_(T&& t) -> decltype(          //
   rc::valid_expr(                                   //
    ((void)(((T &&) t).resize(std::size_t{})), 42),  //
    ((void)((triangle*)((T &&) t).data()), 42)       //
    )                                                //
  );
};

struct SingleWritable {
  template <typename T>
  static auto requires_(T&& t) -> decltype(          //
   rc::valid_expr(                                   //
    ranges::begin(t),                                //
    ranges::end(t),                                  //
    rc::is_true(Same<range_value_t<T>, triangle>{})  //
    )                                                //
  );
};

struct SingleReadable {
  template <typename T>
  static auto requires_(T&& t) -> decltype(          //
   rc::valid_expr(                                   //
    ((void)(((T &&) t).push_back(triangle{})), 42),  //
    ((void)(((T &&) t).reserve(std::size_t{})), 42)  //
    )                                                //
  );
};

}  // namespace concept

/// T can be written in bulk to a file
template <typename T>
using BulkWritable = rc::models<concept::BulkWritable, T>;

/// T can be read in bulk from a file
template <typename T>
using BulkReadable = rc::models<concept::BulkReadable, T>;

/// T can be written a triangle at a time to a file
template <typename T>
using SingleWritable = rc::models<concept::SingleWritable, T>;

/// T can be read a triangle at a time from a file
template <typename T>
using SingleReadable = rc::models<concept::SingleReadable, T>;

/// STL ASCII I/O
namespace ascii {

using namespace parsing;

// TODO: workaround range-v3 recent lack of support for ranges::front on
// InputRanges.
template <typename Rng, CONCEPT_REQUIRES_(InputRange<Rng>{})>
auto front_(Rng&& rng) {
  HM3_ASSERT(!ranges::empty(rng), "The range is empty!");
  return *ranges::begin(rng);
};

/// Parses an ascii STL triangle from \p lines.
///
/// The STL ASCII format for a triangle is:
///
///     facet normal nx ny nz
///       outer loop
///         vertex v0x v0y v0z
///         vertex v1x v1y v1z
///         vertex v2x v2y v2z
///       endloop
///     endfacet
///
/// If the range of lines represents an ASCII STL triangle, returns a triangle.
/// Otherwise returns nothing.
template <typename Lines>
[[nodiscard]] optional<triangle> parse_triangle(Lines&& lines_) {
  auto lines
   = ranges::make_iterator_range(ranges::begin(lines_), ranges::end(lines_));
  if (ranges::empty(lines)) { return {}; }
  string line_chars = front_(lines);  // TODO: use string_view

  // Reports a parsing error with context information: what the parser attempted
  // to parse, and what was the input that failed:
  auto fail = [&](string expected) {
    throw parsing::failure{
     "parsing ASCII STL triangle",  // context
     expected,                      // expected
     string(line_chars)             // actual
    };
  };

  // Parses the current line if it contains a name + a vector of three floating
  // point values:
  auto parse_vec = [&](auto&& name, auto& out) {
    if (auto result = parse_named_array<vec_t>(name, trim(line_chars))) {
      out = result.value();
    } else {
      fail(string(name) + " vx vy vz");
    }
  };

  // Advances the range by one line:
  auto next_line = [&]() {
    if (not try_increment(lines)) {
      fail("another line (but buffer is empty)");
    }
    line_chars = front_(lines);
  };

  triangle tri;

  // First line, parse: facet normal nx ny nz
  // On failure return nothing (we are not parsing a triangle).
  try {
    parse_vec("facet normal", tri.normal());
  } catch (parsing::failure const&) { return {}; }

  // Success: from now on we are parsing a triangle, parsing errors are hard
  // errors:
  try {
    // Second line: 'outer loop'
    next_line();
    if ("outer loop"_sv != trim(line_chars)
        and "outerloop"_sv != trim(line_chars)) {
      fail("outer loop/outerloop");
    }

    // Third-Fifth lines: vertex vx vy vz
    for (auto&& i : view::indices(3)) {
      next_line();
      parse_vec("vertex", tri.vertices()[i]);
    }

    // Sixth line: endloop
    next_line();
    if ("endloop"_sv != trim(line_chars)) { fail("endloop"); }

    // Seventh line: endfacet
    next_line();
    if ("endfacet"_sv != trim(line_chars)) { fail("endfacet"); }

  } catch (parsing::failure const& f) {
    string buf_;

    while (not ranges::empty(lines)) {
      buf_ += *lines.begin() + '\n';
      ++lines.begin();
    }
    HM3_FATAL_ERROR("{}\n\nbuffer:\n{}", f, buf_);
  }
  return tri;
}

/// Reads \p lines into the triangle \p tris
///
/// Returns the solid name ("" if empty) if parsing succeeds.
///
/// If parsing fails, returns nothing, and the state of \p tris is unspecified.
/// (it might be that some triangles have been updated to it)
///
template <typename Lines, typename Tris>
[[nodiscard]] optional<string> read(Lines&& lines_, Tris& tris) {
  static_assert(SingleReadable<Tris&&>(), "tris does not model SingleReadable");
  auto lines
   = ranges::make_iterator_range(ranges::begin(lines_), ranges::end(lines_));
  if (ranges::empty(lines)) { return {}; }
  optional<string> solid_name;

  // File header: solid [name] \n (where name is optional)
  string line_chars = front_(lines);  // string_view ?
  if (auto name = remove_prefix("solid", trim(line_chars))) {
    solid_name = string(trim(name.value()));
  } else {
    return {};
  }

  // File body: parse the triangles
  while (try_increment(lines)) {
    if (auto next_tri = parse_triangle(lines)) {
      tris.push_back(next_tri.value());
    } else {
      break;
    }
  }

  // File footer: endsolid [name] \n (where name must match the header name)
  line_chars = front_(lines);
  if (auto name = remove_prefix("endsolid", trim(line_chars))) {
    if (solid_name.value() != trim(name.value())) {
      ascii_fmt::out("[warning] stl::ascii::read: solid names do not match! "
                     "header: \"{}\", footer: \"{}\"\n",
                     solid_name, trim(name.value()));
    }
  } else {
    return {};
  }

  return solid_name;
}

template <typename Stream>
void write_triangle_normal(triangle const& t, Stream& buf) {
  buf << "  facet normal " << t.normal()[0] << " " << t.normal()[1] << " "
      << t.normal()[2] << "\n";
}

template <typename Stream>
void write_triangle_vertex(triangle const& t, suint_t vidx, Stream& buf) {
  HM3_ASSERT(vidx < 3, "vertex index {} out-of-bounds [0, 3)", vidx);
  buf << "      vertex " << t.vertices()[vidx][0] << " "
      << t.vertices()[vidx][1] << " " << t.vertices()[vidx][2] << "\n";
}

template <typename Stream>
void write_triangle(triangle const& t, Stream& buf) {
  write_triangle_normal(t, buf);
  buf << "    outerloop\n";
  for (auto&& v : view::indices(3)) { write_triangle_vertex(t, v, buf); }
  buf << "    endloop\n  endfacet\n";
}

template <typename Tris, typename Stream>
auto write(Stream& buf, Tris&& tris, string_view solid_name = string_view{}) {
  static_assert(SingleWritable<Tris&&>(), "tris does not model SingleWritable");
  buf << "solid " << solid_name << "\n";
  for (auto&& t : tris) { write_triangle(t, buf); }
  buf << "endsolid " << solid_name;
}

}  // namespace ascii

/// STL Binary I/O
namespace binary {

struct bulk_t {};
static constexpr bulk_t bulk;

struct single_t {};
static constexpr single_t single;

/// Returns the size in bytes of a binary STL file containing \p no_triangles.
constexpr std::size_t file_size(std::size_t no_triangles) {
  // header + no_triangles * 50bytes + attribute
  static_assert(sizeof(triangle) == 50, "");
  return 80 +                              // header (80 bytes)
         sizeof(std::uint32_t) +           // 4bytes storing no_triangles
         no_triangles * sizeof(triangle);  // triangles
}

/// Parses a binary STL file from \p stream.
///
/// Read type:
/// - bulk: all triangles are read directly into \p tris.
/// - single: each triangle is read individually and push_back'ed to \p tris.
///
/// The STL binary format is:
///
/// std::uint8_t[80] – Header (80 bytes)
/// std::uint32_t – Number of triangles (4 bytes)
/// foreach triangle (52 bytes)
///     float[3]      – Normal vector (12 bytes)
///     float[3]      – Vertex 1 (12 bytes)
///     float[3]      – Vertex 2 (12 bytes)
///     float[3]      – Vertex 3 (12 bytes)
///     std::uint16_t – Attribute (2 bytes)
/// end
///
template <typename Stream, typename Tris, typename ReadType>
void read(Stream& stream, Tris& tris, ReadType) {
  // Skip header and read number of triangles:
  stream.seekg(80, std::ios_base::beg);
  std::uint32_t no_triangles;
  stream.read(reinterpret_cast<char*>(&no_triangles), 4);

  // Check the number of bytes in the file:
  std::uint32_t file_should_byte_size = file_size(no_triangles);
  stream.seekg(0, std::ios_base::end);
  const auto file_byte_size = stream.tellg();
  if (file_byte_size != file_should_byte_size) {
    HM3_FATAL_ERROR(
     "Reading STL binary file: the file has {} bytes but should have {} bytes",
     file_byte_size, file_should_byte_size);
  }

  // Skip the header and the number of triangles -> go to the first triangle
  stream.seekg(80 + 4, std::ios_base::beg);

  // Read all triangles:
  if constexpr (Same<ReadType, bulk_t>{}) {
    // Reads all triangles of the file in one go:
    static_assert(BulkReadable<Tris&>(), "tris does not model BulkReadable");
    tris.resize(no_triangles);
    stream.read(reinterpret_cast<char*>(tris.data()),
                no_triangles * sizeof(triangle));
  } else if constexpr (Same<ReadType, single_t>{}) {
    static_assert(SingleReadable<Tris&>(),
                  "tris does not model SingleReadable");
    tris.reserve(no_triangles);
    // Read each triangle individually:
    while (no_triangles != 0) {
      triangle tri;
      stream.read(reinterpret_cast<char*>(&tri), sizeof(triangle));
      tris.push_back(tri);
      --no_triangles;
    }
  } else {
    static_assert(always_false<ReadType>{}, "unknown ReadType");
  }
}

/// Writes \p tris to the output \p stream in bulk.
///
/// See `read` for the specification of the binary STL format.
template <typename Stream, typename Tris>
void write(Stream& stream, Tris&& tris, bulk_t) {
  static_assert(BulkWritable<Tris&&>(), "tris does not model BulkWritable");
  char header[80] = {' '};
  stream.write(header, sizeof(header));
  std::uint32_t no_tris = ranges::size(tris);
  stream.write(reinterpret_cast<char*>(&no_tris), sizeof(no_tris));

  stream.write(reinterpret_cast<char*>(tris.data()),
               sizeof(triangle) * no_tris);
}

/// Writes \p tris to the output \p stream one-by-one.
///
/// See `read` for the specification of the binary STL format.
template <typename Stream, typename Tris>
void write(Stream& stream, Tris&& tris, single_t) {
  static_assert(SingleWritable<Tris&&>(), "tris does not model SingleWritable");
  char header[80] = {' '};
  stream.write(header, sizeof(header));
  std::uint32_t no_tris = 0;  // correct once we know the #of triangles
  stream.write(reinterpret_cast<char*>(&no_tris), sizeof(no_tris));

  for (triangle t : tris) {
    stream.write(reinterpret_cast<char*>(&t), sizeof(t));
    ++no_tris;
  }

  stream.seekp(80, std::ios_base::beg);
  stream.write(reinterpret_cast<char*>(&no_tris), sizeof(no_tris));
}

template <typename Stream, typename Tris>
void read(Stream& s, Tris& tris) {
  if constexpr (BulkReadable<Tris&>()) {
    read(s, tris, binary::bulk);
  } else if constexpr (SingleReadable<Tris&>()) {
    read(s, tris, binary::single);
  } else {
    static_assert(always_false<Tris>(),
                  "tris does not model BulkReadable or SingleReadable");
  }
}

template <typename Stream, typename Tris>
void write(Stream& s, Tris&& tris) {
  if constexpr (BulkWritable<Tris&&>()) {
    write(s, tris, binary::bulk);
  } else if constexpr (SingleWritable<Tris&&>()) {
    write(s, tris, binary::single);
  } else {
    static_assert(always_false<Tris>(),
                  "tris does not model BulkWritable or SingleWritable");
  }
}

}  // namespace binary

struct ascii_format_t {};
struct binary_format_t {};

static constexpr ascii_format_t ascii_format{};
static constexpr binary_format_t binary_format{};

template <typename Format, typename Tris>
void write(Format, string filepath, Tris&& tris, mpi::comm const& comm) {
  static_assert(SingleWritable<Tris&>() or BulkWritable<Tris&>());
  auto file_stream = fs::open_output_stream(filepath, comm);
  if constexpr (Same<Format, ascii_format_t>()) {
    ascii::write(file_stream, std::forward<Tris>(tris));
  } else if constexpr (Same<Format, binary_format_t>()) {
    binary::write(file_stream, std::forward<Tris>(tris));
  } else {
    static_assert(always_false<Format>(), "unknown format (ascii/binary)");
  }
}

template <typename Format, typename Tris>
void read(Format, string filepath, Tris& tris, mpi::comm const& comm) {
  static_assert(SingleReadable<Tris&>() or BulkReadable<Tris&>());
  auto file_stream = fs::open_input_stream(filepath, comm);
  if constexpr (Same<Format, ascii_format_t>()) {
    if (not ascii::read(ranges::getlines(file_stream), tris)) {
      HM3_FATAL_ERROR("failed to parse ascii STL file \"{}\"\n", filepath);
    }
  } else if constexpr (Same<Format, binary_format_t>()) {
    binary::read(file_stream, tris);
  } else {
    static_assert(always_false<Format>(), "unknown format (ascii/binary)");
  }
}

/// TODO: detect whether the STL file is a binary file or a ascii file
/// automatically. It turns out that the STL file format makes this extremely
/// hard to do correctly. Basically one should attempt to parse an STL file as
/// an ASCII file, and if that fails, then re-attempt to parse it as a binary
/// file. If that fails, you still don't know in which format the file is
/// supposed to be. It is also trivial to construct Binary STL files that do
/// look like ASCII files for a while.

}  // namespace stl

}  // namespace hm3::geometry::io
