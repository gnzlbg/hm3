/// \file
///
/// File I/O tests
#include <hm3/geometry/point.hpp>
#include <hm3/io/client.hpp>
#include <hm3/io/session.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/test.hpp>
#include <iostream>

using namespace hm3;

int main() {
  auto env  = mpi::env();
  auto comm = env.world();

  string session_name = "test_session";
  string group_name   = "test_group";
  string type         = "test_type";
  io::session::remove(session_name, comm);

  // some constants:
  int value1   = 1;
  num_t value2 = 2.5;
  geometry::point<3> value3{0., 1., 2.};

  // some arrays:
  std::vector<int_t> vector1{0, 2, 3};
  std::vector<num_t> vector2{2.0, 3.0, 4.0};

  {  // write a file
    io::session s(io::create, session_name, comm);
    io::client c(s, group_name, type);

    auto f = c.new_file();

    f.field("value1", value1)
     .field("value2", value2)
     .field("value3", value3)
     .field("vector1", vector1, io::array_data)
     .field("vector2", vector2, io::array_data);

    c.write(f);

    std::cout << std::setw(2) << f.header() << std::endl;
  }

  {  // read the file
    io::session s(io::restart, session_name, comm);
    io::client c(s, group_name, type);

    int value1r   = 42;
    num_t value2r = 42;
    geometry::point<3> value3r{42., 42., 42.};

    auto f = c.get_file();

    CHECK(f.name() == "test_group_0");
    CHECK(f.index() == 0);
    CHECK(f.path() == "test_group_0");

    std::vector<int_t> vector1r(f.field_size("vector1"), 42);
    std::vector<num_t> vector2r(f.field_size("vector2"), 42);

    // read the constants:
    value1r = f.constant("value1", value1r);
    CHECK(value1r == value1);
    value2r = f.constant("value2", value2r);
    CHECK(value2r == value2);
    value3r = f.constant("value3", value3r);
    CHECK(value3r == value3);

    f.field("vector1", vector1r, io::array_data)
     .field("vector2", vector2r, io::array_data);

    // read the data arrays:
    f.read_arrays();
    CHECK(vector1r == vector1);
    CHECK(vector2r == vector2);
  }

  return test::result();
}
