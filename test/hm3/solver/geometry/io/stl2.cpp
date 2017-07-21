#ifdef ABC

/*#include <hm3/grid/surface/serialization.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

struct tri {
  using point_t = geometry::point<3>;
  array<point_t, 3> vertices_;
  point_t normal_;

  tri(array<point_t, 3> vxs, point_t n) : vertices_(vxs), normal_(n) {}

  point_t const& vertex(suint_t i) const { return vertices_[i]; }
  point_t& vertex(suint_t i) { return vertices_[i]; }

  point_t const& normal() const { return normal_; }
  point_t& normal() { return normal_; }
};

struct tri_mesh {
  std::vector<tri> triangles_;
  auto& triangles() { return triangles_; }
  auto const& triangles() const { return triangles_; }
};

int main() {
  tri t0({{
          //
          {0., 0., 0.},  // vx 0
          {1., 0., 0.},  // vx 1
          {1., 1., 0.}   // vx 2
         }},
         {0., 0., 1.});  // n

  tri t1({{
          //
          {1., 1., 0.},  // vx 0
          {0., 1., 0.},  // vx 1
          {0., 0., 0.}   // vx 2
         }},
         {0., 0., 1.});  // n

  {  // test triangle to asscii + write tri
    // clang format off
    char ascii_stl[] =  //
"facet normal +0.00000000000000000e+00 +0.00000000000000000e+00
+1.00000000000000000e+00\n"
"    outer loop\n"
"        vertex +0.00000000000000000e+00 +0.00000000000000000e+00
+0.00000000000000000e+00\n"
"        vertex +1.00000000000000000e+00 +0.00000000000000000e+00
+0.00000000000000000e+00\n"
"        vertex +1.00000000000000000e+00 +1.00000000000000000e+00
+0.00000000000000000e+00\n"
"    endloop\n"
"endfacet\n";
    // clang format on
    static_assert(
     sizeof(ascii_stl) - 1 == grid::surface::stl::ascii_tri_str_size(),
     "if this changes one should update the value of ascii-tri_str_size()");

    auto result = grid::surface::stl::triangle_to_ascii(t0);
    std::cout << ascii_stl << std::endl;
    std::cout << result << std::endl;
    CHECK(result == ascii_stl);
  }

  {
    tri_mesh mesh;
    mesh.triangles_.push_back(t0);
    mesh.triangles_.push_back(t1);
    std::cout << "STRING:" << std::endl;
    std::cout << grid::surface::stl::mesh_to_ascii("tris", mesh) << std::endl;
    std::cout << "STD::COUT" << std::endl;
    grid::surface::stl::stream_mesh_to_ascii(std::cout, "tris", mesh);

    std::ofstream ofs{"two_tris.stl"};
    grid::surface::stl::stream_mesh_to_ascii(ofs, "tris", mesh);
  }

  return 0;
}
*/

#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/phoenix.hpp>
#include <boost/phoenix/function.hpp>
#include <boost/spirit/include/qi.hpp>
#include <hm3/geometry/point.hpp>

using namespace hm3;

struct tri {
  using point_t = geometry::point<3>;
  array<point_t, 3> vertices_;
  point_t normal_;

  tri() = default;
  tri(array<point_t, 3> vxs, point_t n) : vertices_(vxs), normal_(n) {}

  point_t const& vertex(suint_t i) const { return vertices_[i]; }
  point_t& vertex(suint_t i) { return vertices_[i]; }

  point_t const& normal() const { return normal_; }
  point_t& normal() { return normal_; }
};
template <typename OStream>  //
OStream& operator<<(OStream& os, tri const& t) {
  os << t.normal_ << " | ";
  for (int d = 0; d < 3; ++d) os << t.vertices_[d];
  return os;
}

namespace qi    = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

/// Parses STL triangles
template <typename It>  //
struct stl_tri_parser : qi::grammar<It, ascii::space_type> {
  int vx;
  template <typename Tri>  //
  stl_tri_parser(Tri& tri) : stl_tri_parser::base_type(start) {
    // how to read the normal and the vertices:
    auto n
     = [&tri](int p) { return [&, p](auto&& d) { tri.normal()(p) = d; }; };

    auto v = [&tri, this](int p) {
      return [&, p](auto&& d) {
        tri.vertex(vx)(p) = d;
        if (p == 2) { ++vx; }
      };
    };

    auto&& d_ = qi::double_;
    using qi::repeat;
    using qi::phrase_parse;
    using qi::lit;
    using qi::eps;

    facet_normal %= lit("facet normal") >> d_[n(0)] >> d_[n(1)] >> d_[n(2)];
    vertex %= lit("vertex") >> d_[v(0)] >> d_[v(1)] >> d_[v(2)];

    start %=
     // Initialize parser (set vertex count to zero):
     eps[([&]() { vx = 0; })] >>  //
     // Begin STL facet grammar
     (                      //
      facet_normal          // parse facet normal
      >> lit("outer loop")  //
      >> repeat(3)[vertex]  // parse 3 vertices
      >> lit("endloop")     //
      >> lit("endfacet")    // done
      // End STL facet grammar
      );
  }
  qi::rule<It, ascii::space_type> start;
  qi::rule<It, ascii::space_type> facet_normal;
  qi::rule<It, ascii::space_type> vertex;
};

struct stl_mesh {
  std::string name;
  std::vector<tri> tris;
};

namespace boost {
namespace spirit {
namespace traits {
template <> struct is_container<stl_mesh> {};

template <typename Attrib> struct push_back_container<stl_mesh, Attrib> {
  static bool call(stl_mesh& c, Attrib const& val) {
    c.tris.push_back(val);
    return true;
  }
};

template <> struct container_value<stl_mesh> { typedef tri type; };
}
}
}

struct stl_mesh_set_name {
  template <typename, typename> struct result { using type = bool; };
  template <typename Mesh, typename Name>
  bool operator()(Mesh& m, Name& n) const {
    if (m.name.empty()) {
      m.name = n;
      return true;
    }
    return false;
  }
};

BOOST_PHOENIX_ADAPT_CALLABLE(set_name, stl_mesh_set_name, 2);

template <typename It, typename Mesh>
using stl_rule = qi::rule<It, Mesh(), qi::blank_type, qi::locals<size_t>>;

stl_rule<typename std::string::iterator, stl_mesh> stl_parser;

// template <typename It, typename Mesh>  //
// struct stl_parser : qi::grammar<It, Mesh(), ascii::space> {
//   string solid_name;
//   stl_parser() : stl_parser::base_type(start) {
//     using qi::lit;
//     using qi::eps;
//     using qi::char_;
//     using qi::eol;
//     using qi::_val;
//     using qi::_1;
//     using qi::_pass;
//   }

//   qi::rule<It, Mesh(), ascii::space> start;
// };

// Parses STL ASCII facet into a triangle
template <typename Rng, typename Tri>  //
bool parse_ascii_stl_tri(Rng&& rng, Tri& tri) {
  auto&& first = std::begin(rng);
  auto&& last  = std::end(rng);
  using it_t   = decltype(first);

  namespace qi    = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;
  auto&& d_       = qi::double_;
  using qi::repeat;
  using qi::phrase_parse;
  using qi::lit;
  using ascii::space;

  // how to read the normal and the vertices:
  auto n = [&tri](int p) { return [&, p](auto&& d) { tri.normal()(p) = d; }; };
  int vx = 0;
  auto v = [&tri, &vx](int p) {
    return [&, p](auto&& d) {
      tri.vertex(vx)(p) = d;
      if (p == 2) { ++vx; }
    };
  };

  qi::rule<it_t, qi::space_type> vx_rule
   = lit("vertex") >> d_[v(0)] >> d_[v(1)] >> d_[v(2)];

  bool r = phrase_parse(
   first, last,
   (  // Begin STL facet grammar
    lit("facet normal") >> d_[n(0)] >> d_[n(1)] >> d_[n(2)]          //
    >> lit("outer loop")                                             //
    >> repeat(3)[lit("vertex") >> d_[v(0)] >> d_[v(1)] >> d_[v(2)]]  //
    >> lit("endloop")                                                //
    >> lit("endfacet")
    // End STL facet grammar
    ),
   space);

  if (first != last) { return false; }
  return r;
}

template <typename Rng, typename TriMesh>
bool parse_ascii_stl_mesh(Rng&& rng, TriMesh& mesh) {
  auto&& first = std::begin(rng);
  auto&& last  = std::end(rng);

  namespace qi    = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;
  using qi::phrase_parse;
  using qi::lit;
  using qi::char_;
  auto&& s_ = qi::string;

  using ascii::space;

  string solid_name;
  auto sname = [&](auto&& s) { solid_name.append(s); };

  // bool s = phrase_parse(
  //  first, last, lit("solid") >> +char_[sname], space);

  // std::cout << std::endl;
  // std::cout << solid_name << std::endl;

  // parse solid name
  // parse tris
  // once first non tri
  // parse endsolid
}

int main() {
  using std::string;
  using std::cout;
  using std::endl;

  // clang-format off
  char ascii_stl_[] =  //
    "facet normal +0.00000000000000000e+00 +0.00000000000000000e+00 +1.00000000000000000e+00\n"
    "    outer loop\n"
    "        vertex +0.00000000000000000e+00 +0.00000000000000000e+00 +0.00000000000000000e+00\n"
    "        vertex +1.00000000000000000e+00 +0.00000000000000000e+00 +0.00000000000000000e+00\n"
    "        vertex +1.00000000000000000e+00 +1.00000000000000000e+00 +0.00000000000000000e+00\n"
    "    endloop\n"
    "endfacet\n";
  // clang-format on

  string ascii_stl(ascii_stl_);

  tri t;

  cout << parse_ascii_stl_tri(ascii_stl, t) << endl;
  cout << t << endl;

  {
    tri t2;
    auto f = std::begin(ascii_stl);
    auto l = std::end(ascii_stl);
    stl_tri_parser<std::string::iterator> sp(t2);
    auto b = qi::phrase_parse(f, l, sp, ascii::space);
    std::cout << "b: " << b << " | " << t2 << std::endl;
  }

  {
    // clang-format off
    char ascii_stl_mesh_[] =  //
    "solid two_tri_mesh\n"
    "facet normal +0.00000000000000000e+00 +0.00000000000000000e+00 +1.00000000000000000e+00\n"
    "    outer loop\n"
    "        vertex +0.00000000000000000e+00 +0.00000000000000000e+00 +0.00000000000000000e+00\n"
    "        vertex +1.00000000000000000e+00 +0.00000000000000000e+00 +0.00000000000000000e+00\n"
    "        vertex +1.00000000000000000e+00 +1.00000000000000000e+00 +0.00000000000000000e+00\n"
    "    endloop\n"
    "endfacet\n"
    "facet normal +0.00000000000000000e+00 +0.00000000000000000e+00 +1.00000000000000000e+00\n"
    "    outer loop\n"
    "        vertex +0.00000000000000000e+00 +0.00000000000000000e+00 +0.00000000000000000e+00\n"
    "        vertex +1.00000000000000000e+00 +0.00000000000000000e+00 +0.00000000000000000e+00\n"
    "        vertex +1.00000000000000000e+00 +1.00000000000000000e+00 +0.00000000000000000e+00\n"
    "    endloop\n"
    "endfacet\n"
    "endsolid two_tri_mesh\n"
    ;
    // clang-format on
    string ascii_stl_mesh(ascii_stl_mesh_);

    auto f = std::begin(ascii_stl_mesh);
    auto l = std::end(ascii_stl_mesh);
    // stl_parser<std::string::iterator, stl_mesh> parser();
    stl_mesh mesh;
    auto b = qi::parse(f, l, stl_parser, mesh);
    std::cout << "b: " << b << " | stl mesh name: " << mesh.name << std::endl;
    for (auto&& t : mesh.tris) { std::cout << t << std::endl; }
  }

  return 0;
}
#else
int main() { return 0; }
#endif
