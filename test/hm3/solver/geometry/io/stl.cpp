#ifdef ABC
#define BOOST_RESULT_OF_USE_DECLTYPE
#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <array>
#include <boost/bind.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <string>
namespace ph    = boost::phoenix;
namespace qi    = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

namespace my_ns {

template <typename OStream>
OStream& operator<<(OStream& os, std::array<double, 3> p) {
  os << "(" << p[0] << "," << p[1] << "," << p[2] << ")";
  return os;
}

}  // my_ns

namespace stl {

namespace detail {

template <class T> struct static_const { static constexpr T value{}; };

template <class T> constexpr T static_const<T>::value;

struct set_normal_fn {
  template <typename Tri>
  constexpr auto operator()(Tri&& tri, int i, double v) const
   -> decltype(set_normal(std::forward<Tri>(tri), i, v)) {
    return set_normal(std::forward<Tri>(tri), i, v);
  }
};

}  // namespace detail

namespace {
constexpr auto const& set_normal
 = detail::static_const<detail::set_normal_fn>::value;
}

}  // namespace stl

int main() {
  using std::string;
  using std::cout;
  using std::endl;
  using namespace my_ns;

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

  {  // parse a single triangle
    tri t;
    auto f = std::begin(ascii_stl);
    auto l = std::end(ascii_stl);
    stl::tri_parser<std::string::iterator, tri> p;
    auto b = qi::phrase_parse(f, l, p, qi::blank, t);
    std::cout << "b: " << b << " | " << t << std::endl;
  }

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
    "        vertex +1.00000000000000000e+00 +1.00000000000000000e+00 +0.00000000000000000e+00\n"
    "        vertex +0.00000000000000000e+00 +1.00000000000000000e+00 +0.00000000000000000e+00\n"
    "        vertex +0.00000000000000000e+00 +0.00000000000000000e+00 +0.00000000000000000e+00\n"
    "    endloop\n"
    "endfacet\n"
    "endsolid two_tri_mesh\n"
    ;
  // clang-format on
  string ascii_stl_mesh(ascii_stl_mesh_);
  {
    tri_mesh mesh;
    auto f = std::begin(ascii_stl_mesh);
    auto l = std::end(ascii_stl_mesh);
    stl::tri_mesh_parser<std::string::iterator, tri_mesh, tri> p;
    auto b = qi::phrase_parse(f, l, p, qi::blank, mesh);
    std::cout << "b: " << b << " | stl mesh name: " << mesh.name << std::endl;
    for (auto&& t : mesh.tris) { std::cout << t << std::endl; }
  }

  return 0;
}
#else
int main() { return 0; }
#endif
