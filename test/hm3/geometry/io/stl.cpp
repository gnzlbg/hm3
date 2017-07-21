#include <hm3/geometry/io/stl.hpp>
#include <hm3/utility/test.hpp>
#include <sstream>

using namespace hm3;
namespace stl = hm3::geometry::io::stl;

using mesh = vector<stl::triangle>;

int main() {
  stl::triangle tri1({{0.1f, 0.2f, 0.3f}},  //
                     {{
                      // vertices
                      stl::vec_t{{0.4f, 0.5f, 0.6f}},  //
                      stl::vec_t{{0.7f, 0.8f, 0.9f}},  //
                      stl::vec_t{{0.1f, 0.2f, 0.3f}}   //
                     }});

  stl::triangle tri2({{0.4f, 0.5f, 0.6f}},  //
                     {{
                      // vertices
                      stl::vec_t{{0.7f, 0.8f, 0.9f}},  //
                      stl::vec_t{{0.1f, 0.2f, 0.3f}},  //
                      stl::vec_t{{0.4f, 0.5f, 0.6f}}   //
                     }});

  auto tri_mesh_str =
   R"(solid mesh
  facet normal 0.1 2e-1 3E-1
    outer loop
      vertex 0.4 5e-1 6E-1
      vertex 0.7 8e-1 9E-1
      vertex 0.1 2e-1 3E-1
    endloop
  endfacet
  facet normal 0.4 5e-1 6E-1
    outer loop
      vertex 0.7 8e-1 9E-1
      vertex 0.1 2e-1 3E-1
      vertex 0.4 5e-1 6E-1
    endloop
  endfacet
endsolid mesh)";

  {  // ascii parse triangle

    auto tri_str =
     R"(  facet normal 0.1 2e-1 3E-1
    outer loop
      vertex 0.4 5e-1 6E-1
      vertex 0.7 8e-1 9E-1
      vertex 0.1 2e-1 3E-1
    endloop
  endfacet
)";

    auto in        = std::istringstream(tri_str);
    auto tri_parse = stl::ascii::parse_triangle(ranges::getlines(in));
    CHECK(tri_parse);
    auto tri = tri_parse.value();

    CHECK(tri.normal()[0] == 0.1f);
    CHECK(tri.normal()[1] == 0.2f);
    CHECK(tri.normal()[2] == 0.3f);

    CHECK(tri.vertices()[0][0] == 0.4f);
    CHECK(tri.vertices()[0][1] == 0.5f);
    CHECK(tri.vertices()[0][2] == 0.6f);
    CHECK(tri.vertices()[1][0] == 0.7f);
    CHECK(tri.vertices()[1][1] == 0.8f);
    CHECK(tri.vertices()[1][2] == 0.9f);
    CHECK(tri.vertices()[2][0] == 0.1f);
    CHECK(tri.vertices()[2][1] == 0.2f);
    CHECK(tri.vertices()[2][2] == 0.3f);
  }

  {  // parse mesh
    mesh m;

    auto in_mesh = std::istringstream(tri_mesh_str);
    auto r       = stl::ascii::read(ranges::getlines(in_mesh), m);
    CHECK(r);

    mesh m_should = {tri1, tri2};
    CHECK(m == m_should);

    std::stringstream ss;
    stl::ascii::write(ss, m, "mesh");

    auto tri_mesh_str_should = R"(solid mesh
  facet normal 0.1 0.2 0.3
    outerloop
      vertex 0.4 0.5 0.6
      vertex 0.7 0.8 0.9
      vertex 0.1 0.2 0.3
    endloop
  endfacet
  facet normal 0.4 0.5 0.6
    outerloop
      vertex 0.7 0.8 0.9
      vertex 0.1 0.2 0.3
      vertex 0.4 0.5 0.6
    endloop
  endfacet
endsolid mesh)";

    CHECK(ss.str() == tri_mesh_str_should);

    // 80 (header) + 4(no_tris) 2*48(tris) + 2 (no_bytes)
    char file[80 + 4 + 2 * sizeof(stl::triangle)];
    reinterpret_cast<std::uint32_t&>(file[80])     = 2;
    reinterpret_cast<stl::triangle&>(file[80 + 4]) = m[0];
    reinterpret_cast<stl::triangle&>(file[80 + 4 + sizeof(stl::triangle)])
     = m[1];

    std::istringstream is(std::string(file, sizeof(file)));

    // single read:
    mesh m2;
    stl::binary::read(is, m2, stl::binary::single);
    CHECK(m2 == m);

    // bulk read:
    mesh m3;
    stl::binary::read(is, m3, stl::binary::bulk);
    CHECK(m3 == m);

    {  // bulk write:
      std::stringstream out;
      stl::binary::write(out, m, stl::binary::bulk);

      mesh m_;
      stl::binary::read(out, m_, stl::binary::bulk);
      CHECK(m_ == m);
    }
    {  // single write
      std::stringstream out;
      stl::binary::write(out, m, stl::binary::single);

      mesh m_;
      stl::binary::read(out, m_, stl::binary::single);
      CHECK(m_ == m);
    }
  }

  {  // bunny
    std::size_t no_tris = 51978;
    // read ascii and binary bunnies:
    mesh ascii_bunny;
    stl::read(stl::ascii_format,
              HM3_TEST_RESOURCES "hm3/geometry/io/bunny_ascii.stl", ascii_bunny,
              mpi::comm::world());

    stl::triangle tri10({{0.588532, -0.806304, -0.0591908}},  // normal
                        {{
                         // vertices:
                         {{18.6831, -29.4588, 18.4701}},  //
                         {{19.3734, -28.9161, 17.941}},   //
                         {{18.4338, -29.6751, 18.9378}}   //
                        }});

    CHECK(ascii_bunny.size() == no_tris);
    CHECK(ascii_bunny[10] == tri10);

    mesh binary_bunny;
    stl::read(stl::binary_format,
              HM3_TEST_RESOURCES "hm3/geometry/io/bunny_binary.stl",
              binary_bunny, mpi::comm::world());
    CHECK(binary_bunny.size() == no_tris);
    CHECK(binary_bunny[10] == tri10);

    CHECK(ascii_bunny == binary_bunny);
    for (auto p : view::zip(ascii_bunny, binary_bunny)) {
      auto ascii_tri  = first(p);
      auto binary_tri = second(p);
      CHECK(ascii_tri == binary_tri);
    }

    // write ascii and binary bunnies:
    fs::remove("bunny_ascii2.stl", mpi::comm::world());
    stl::write(stl::ascii_format, "bunny_ascii2.stl", ascii_bunny,
               mpi::comm::world());
    fs::remove("bunny_binary2.stl", mpi::comm::world());
    stl::write(stl::binary_format, "bunny_binary2.stl", binary_bunny,
               mpi::comm::world());

    // read them back and compare them
    mesh ascii_bunny2;
    stl::read(stl::ascii_format, "bunny_ascii2.stl", ascii_bunny2,
              mpi::comm::world());
    CHECK(ascii_bunny2 == ascii_bunny);
    CHECK(ascii_bunny2[10] == tri10);

    mesh binary_bunny2;
    stl::read(stl::binary_format, "bunny_binary2.stl", binary_bunny2,
              mpi::comm::world());
    CHECK(binary_bunny2 == binary_bunny);
    CHECK(binary_bunny2[10] == tri10);

    fs::remove("bunny_ascii2.stl", mpi::comm::world());
    fs::remove("bunny_binary2.stl", mpi::comm::world());
  }

  return test::result();
}
