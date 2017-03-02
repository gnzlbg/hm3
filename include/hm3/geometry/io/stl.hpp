#pragma once
/// \file
///
/// STereoLithography Parser
///
/// Copyright Gonzalo Brito Gadeschi 2016
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE.md or copy at
/// http://boost.org/LICENSE_1_0.txt)
///
#define BOOST_RESULT_OF_USE_DECLTYPE
#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_NO_AUTO_PTR

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wdeprecated"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wshadow"
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#pragma clang diagnostic pop

#include <hm3/utility/range.hpp>
#include <string>
#include <type_traits>

namespace stl {

using hm3::uncvref_t;

struct ascii_t {};
static constexpr ascii_t ascii{};

struct binary_t {};
static constexpr binary_t binary{};

inline namespace v1 {
namespace utility {

template <class T>
struct static_const {
  static constexpr T value{};
};

template <class T>
constexpr T static_const<T>::value;

}  // namespace utility

/// \name Customization points
///@{

namespace customization {

/// Specialize this type trait to allow discovering the associated triangle type
/// of a mesh automatically (it can be still manually specified on the parsers).
template <typename Mesh>
struct triangle_type {
  using type = typename Mesh::triangle_type;
};

template <typename Mesh>
using triangle_t = typename triangle_type<Mesh>::type;

/// How to set a triangle normal vector component
struct set_normal_fn {
  template <typename Tri>
  constexpr auto /* bool */ operator()(Tri&& tri, int component,
                                       float value) const
   -> decltype(set_normal(std::forward<Tri>(tri), component, value), bool()) {
    return set_normal(std::forward<Tri>(tri), component, value);
  }
};

/// How to set a triangle normal vector component
struct get_normal_fn {
  template <typename Tri>
  constexpr auto operator()(Tri const& tri, int component) const
   -> decltype(get_normal(tri, component), float()) {
    return get_normal(tri, component);
  }
};

/// How to set a triangle vertex component
struct set_vertex_fn {
  template <typename Tri>
  constexpr auto /* bool */ operator()(Tri&& tri, int vertex, int component,
                                       float value) const
   -> decltype(set_vertex(std::forward<Tri>(tri), vertex, component, value),
               bool()) {
    return set_vertex(std::forward<Tri>(tri), vertex, component, value);
  }
};

/// How to get a triangle vertex component
struct get_vertex_fn {
  template <typename Tri>
  constexpr auto /* bool */ operator()(Tri const& tri, int vertex,
                                       int component) const
   -> decltype(get_vertex(tri, vertex, component), float()) {
    return get_vertex(tri, vertex, component);
  }
};

/// How to set the mesh name
struct set_mesh_name_fn {
  template <typename Mesh, typename String>
  constexpr auto /* bool */ operator()(Mesh&& mesh, String&& name) const
   -> decltype(set_mesh_name(std::forward<Mesh>(mesh),
                             std::forward<String>(name)),
               bool()) {
    return set_mesh_name(std::forward<Mesh>(mesh), std::forward<String>(name));
  }
};

/// How to append a triangle to the mesh
struct push_triangle_fn {
  template <typename Mesh, typename Tri>
  constexpr auto /* bool */ operator()(Mesh&& mesh, Tri&& tri) const
   -> decltype(push_triangle(std::forward<Mesh>(mesh), std::forward<Tri>(tri)),
               bool()) {
    return push_triangle(std::forward<Mesh>(mesh), std::forward<Tri>(tri));
  }
};

/// Reserve memory for a number of triangles
struct reserve_fn {
  // try to find it via ADL
  template <typename Mesh>
  constexpr auto /* bool */ impl(Mesh&& mesh, std::uint32_t n, int) const
   -> decltype(reserve(std::forward<Mesh>(mesh), n), bool()) {
    return reserve(std::forward<Mesh>(mesh), n);
  }

  // otherwise do nothing
  template <typename Mesh>
  constexpr auto /* bool */ impl(Mesh&&, std::uint32_t, long) const -> bool {
    return true;
  }

  template <typename Mesh>
  constexpr auto /* bool */ operator()(Mesh&& mesh, std::uint32_t n) const
   -> decltype(impl(std::forward<Mesh>(mesh), n, int()), bool()) {
    return impl(std::forward<Mesh>(mesh), n, int());
  }
};

}  // namespace customization

namespace {
constexpr auto const& set_normal
 = utility::static_const<customization::set_normal_fn>::value;
constexpr auto const& set_vertex
 = utility::static_const<customization::set_vertex_fn>::value;
constexpr auto const& set_mesh_name
 = utility::static_const<customization::set_mesh_name_fn>::value;
constexpr auto const& push_triangle
 = utility::static_const<customization::push_triangle_fn>::value;
constexpr auto const& reserve
 = utility::static_const<customization::reserve_fn>::value;
constexpr auto const& get_normal
 = utility::static_const<customization::get_normal_fn>::value;
constexpr auto const& get_vertex
 = utility::static_const<customization::get_vertex_fn>::value;

}  // namespace

///@}  // Customization points

namespace ph = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace ka = boost::spirit::karma;

namespace ascii_parser {

/// Parses a single STL triangle
template <typename It, typename Tri>
struct triangle
 : qi::grammar<It, Tri(), qi::blank_type, qi::locals<size_t, size_t>> {
  triangle() : triangle::base_type(start, "stl_ascii_triangle") {
    auto&& d_ = qi::float_;
    using qi::repeat;
    using qi::lit;
    using qi::eps;
    using qi::eol;
    using qi::on_error;
    using qi::fail;
    using qi::_val;
    using qi::_1;
    using qi::_2;
    using qi::_3;
    using qi::_4;
    using qi::_pass;
    using qi::_a;  // d-th component
    using qi::_b;  // vertex index

    using boost::phoenix::construct;
    using boost::phoenix::val;

    auto facet_normal = boost::proto::deep_copy(
     eps[_a = 0]
     >> (lit("facet normal")
         > repeat(
            3)[d_[_pass = ph::bind(set_normal, _val, _a, _1)] > eps[_a += 1]]
         > eol));

    auto vertex = boost::proto::deep_copy(
     (eps[_a = 0] > lit("vertex"))
     > repeat(
        3)[d_[_pass = ph::bind(set_vertex, _val, _b, _a, _1)] > eps[_a += 1]]
     > eps[_b += 1] > eol);

    auto vertices = boost::proto::deep_copy(eps[_b = 0] > repeat(3)[vertex]);

    start = facet_normal > lit("outer loop") > eol > vertices > lit("endloop")
            > eol > lit("endfacet");

    start.name("triangle");

    on_error<fail>(
     start,
     std::cout << val("Error! Expecting ") << _4 << val(" here: \"")
               << construct<std::string>(_3, _2)  // iterators to error-pos, end
               << val("\"") << std::endl);
  }
  qi::rule<It, Tri(), qi::blank_type, qi::locals<size_t, size_t>> start;
};

/// Parses a STL mesh
template <typename It, typename Mesh,
          typename Tri = customization::triangle_t<Mesh>>
struct mesh : qi::grammar<It, Mesh(), qi::blank_type, qi::locals<std::string>> {
  mesh() : mesh::base_type(start) {
    using qi::lit;
    using qi::eol;
    using qi::on_error;
    using qi::fail;
    using qi::_val;  // mesh
    using qi::_1;    // tri
    using qi::_2;
    using qi::_3;
    using qi::_4;
    using qi::_a;  // mesh name
    using qi::_pass;
    using qi::char_;
    using qi::eps;

    using boost::phoenix::construct;
    using boost::phoenix::val;

    auto header = boost::proto::deep_copy(
     lit("solid") >> +(char_ - eol)[_a += _1] >> eol
     >> eps[_pass = ph::bind(set_mesh_name, _val, _a)]);
    // auto footer = boost::proto::deep_copy(lit("endsolid " + _a));
    auto footer = lit("endsolid");

    auto push_tri = boost::proto::deep_copy(
     tri_parser[_pass = ph::bind(push_triangle, _val, _1)] >> eol);

    start = header >> +(push_tri) > footer;

    start.name("stl");

    on_error<fail>(
     start,
     std::cout << val("Error! Expecting ") << _4 << val(" here: \"")
               << construct<std::string>(_3, _2)  // iterators to error-pos, end
               << val("\"") << std::endl);
  }
  qi::rule<It, Mesh(), qi::blank_type, qi::locals<std::string>> start;
  triangle<It, Tri> tri_parser;
};

}  // namespace ascii_parser

namespace ascii_generator {

/// Parses a single STL triangle
template <typename It, typename Tri>
struct triangle : ka::grammar<It, Tri(), ka::locals<std::size_t, std::size_t>> {
  template <typename Num>
  struct float_policy : ka::real_policies<Num> {
    using base = ka::real_policies<Num>;
    static int floatfield(Num) { return base::fmtflags::scientific; }
    static bool force_sign(Num) { return true; }
    static bool trailing_zeros(Num) { return true; }
    static unsigned precision(Num) { return 16; }
  };

  template <typename Num>
  using num_t = ka::real_generator<Num, float_policy<Num>>;

  triangle() : triangle::base_type(start) {
    auto&& d_ = num_t<double>();
    using ka::repeat;
    using ka::lit;
    using ka::char_;
    using ka::eps;
    using ka::eol;
    using ka::_val;
    using ka::_1;
    using ka::_a;  // d-th component
    using ka::_b;  // vertex index

    auto facet_normal = boost::proto::deep_copy(
     eps[_a = 0] << (lit("facet normal") << repeat(
                      3)[ka::space << d_[_1 = ph::bind(get_normal, _val, _a)]
                                   << eps[_a += 1]])
                 << eol);

    auto vertex = boost::proto::deep_copy(
     eps[_a = 0] << lit("        vertex")
                 << repeat(
                     3)[ka::space << d_[_1 = ph::bind(get_vertex, _val, _b, _a)]
                                  << eps[_a += 1]]
                 << eps[_b += 1] << eol);
    auto vertices = boost::proto::deep_copy(eps[_b = 0] << repeat(3)[vertex]);

    start = facet_normal << lit("    outer loop") << eol << vertices
                         << lit("    endloop") << eol
                         << lit("endfacet");  // facet_normal << vertices;
  }
  ka::rule<It, Tri(), ka::locals<std::size_t, std::size_t>> start;
};

}  // namespace ascii_generator

namespace binary_parser {

/// Parses a single STL triangle
template <typename It, typename Tri>
struct triangle : qi::grammar<It, Tri(), qi::locals<size_t, size_t>> {
  triangle() : triangle::base_type(start) {
    auto&& f_ = qi::bin_float;
    using qi::repeat;
    using qi::lit;
    using qi::eps;
    using qi::eol;
    using qi::_val;
    using qi::_1;
    using qi::_pass;
    using qi::_a;  // d-th component
    using qi::_b;  // vertex index

    auto normal = boost::proto::deep_copy(
     eps[_a = 0]
     >> repeat(
         3)[f_[_pass = ph::bind(set_normal, _val, _a, _1)] >> eps[_a += 1]]);

    auto vertex = boost::proto::deep_copy(
     eps[_a = 0]
     >> repeat(
         3)[f_[_pass = ph::bind(set_vertex, _val, _b, _a, _1)] >> eps[_a += 1]]
     >> eps[_b += 1]);

    auto vertices = boost::proto::deep_copy(eps[_b = 0] >> repeat(3)[vertex]);

    start = normal >> vertices;
  }
  qi::rule<It, Tri(), qi::locals<size_t, size_t>> start;
};

/// Parses a STL mesh
template <typename It, typename Mesh,
          typename Tri = customization::triangle_t<Mesh>>
struct mesh : qi::grammar<It, Mesh(), qi::blank_type, qi::locals<unsigned>> {
  mesh() : mesh::base_type(start) {
    using qi::repeat;

    using qi::byte_;
    using qi::uint_;
    using qi::dword;
    using qi::eps;
    using qi::_val;  // mesh
    using qi::_1;    // tri
    using qi::_a;    // number of triangles
    using qi::_pass;
    using qi::char_;

    auto header = boost::proto::deep_copy(repeat(80)[byte_] >> dword[_a = _1]
                                          >> eps[ph::bind(reserve, _val, _a)]);
    auto push_tri = boost::proto::deep_copy(
     tri_parser[_pass = ph::bind(push_triangle, _val, _1)]);
    start = header >> +(push_tri);
    // start = header;
  }
  qi::rule<It, Mesh(), qi::blank_type, qi::locals<unsigned>> start;
  triangle<It, Tri> tri_parser;
};

}  // namespace binary_parser

/// Parses an ASCII STL mesh in range [\p f, \p l) into the Mesh \p m
template <typename InputIt, typename Mesh>
bool parse_mesh(InputIt f, InputIt l, Mesh& m, ascii_t) {
  using mesh_type = uncvref_t<Mesh>;
  return qi::phrase_parse(f, l, ascii_parser::mesh<InputIt, mesh_type>{},
                          qi::blank, m);
}

/// Parses an ASCII STL InputRange \p rng into the Mesh \p m
template <typename InputRange, typename Mesh>
bool parse_mesh(InputRange& rng, Mesh& m, ascii_t) {
  using std::begin;
  using std::end;
  return parse_mesh(begin(rng), end(rng), m, ascii);
}

/// Parses an ASCII STL triangle in range [\p f, \p l) into the Triangle \p t
template <typename InputIt, typename Triangle>
bool parse_triangle(InputIt f, InputIt l, Triangle& t, ascii_t) {
  using triangle = uncvref_t<Triangle>;
  return qi::phrase_parse(f, l, ascii_parser::triangle<InputIt, triangle>{},
                          qi::blank, t);
}

/// Parses an ASCII STL triangle from an InputRange \p rng into the Triangle \p
/// t
template <typename InputRange, typename Triangle>
bool parse_triangle(InputRange& rng, Triangle& t, ascii_t) {
  using std::begin;
  using std::end;
  return parse_triangle(begin(rng), end(rng), t, ascii);
}

/// Parses an Binary STL triangle in range [\p f, \p l) into the Triangle \p t
template <typename InputIt, typename Triangle>
bool parse_triangle(InputIt f, InputIt l, Triangle& t, binary_t) {
  using triangle = uncvref_t<Triangle>;
  using iterator_value_type =
   typename std::iterator_traits<InputIt>::value_type;
  static_assert(std::is_same<iterator_value_type, char>{},
                "stl binary parsers require a range of bytes (chars)");
  return qi::phrase_parse(f, l, binary_parser::triangle<InputIt, triangle>{},
                          qi::blank, t);
}

/// Parses a Binary STL mesh in range [\p f, \p l) into the Mesh \p m
template <typename InputIt, typename Mesh>
bool parse_mesh(InputIt f, InputIt l, Mesh& m, binary_t) {
  using iterator_value_type =
   typename std::iterator_traits<InputIt>::value_type;
  static_assert(std::is_same<iterator_value_type, char>{},
                "stl binary parsers require a range of bytes (chars)");

  using mesh_type = uncvref_t<Mesh>;
  return qi::phrase_parse(f, l, binary_parser::mesh<InputIt, mesh_type>{},
                          qi::blank, m);
}

/// Write a triangle \p t into an OutputIt \p out in ASCII format
template <typename OutputIt, typename Triangle>
std::pair<OutputIt, bool> write_triangle(OutputIt& out, Triangle& t, ascii_t) {
  using triangle = uncvref_t<Triangle>;
  ascii_generator::triangle<OutputIt, triangle> g{};
  auto b = ka::generate(out, g, t);
  return {out, b};
}

/// Write a triangle \p t into an OutputRange \p rng in ASCII format
// template <typename OutputRange, typename Triangle>
// auto write_triangle(OutputRange& rng, Triangle const& t, ascii_t)
//  -> decltype(std::begin(rng), std::end(rng), bool()) {
//   using std::begin;
//   using std::end;
//   auto r = write_triangle(begin(rng), t, ascii);
//   return r.second;
// }

}  // namespace v1

}  // namespace stl
