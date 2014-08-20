/// \file session.cpp I/O session tests
#include <hm3/io/client.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  auto env            = mpi::env();
  auto comm_world     = env.world();
  string session_name = "test_session";
  io::session::remove(session_name, comm_world);
  io::session s(io::create, session_name, comm_world);
  {  // write a session

    // push files: grid, level_set_0, dynamic_geometry
    auto g = io::client(s, "grid", "hierarchical_cartesian");
    auto l = io::client(s, "level_set_0", "level_set", "grid");
    auto d = io::client(s, "dynamic_geometry", "geometry", "level_set_0");

    CHECK(s.size() == std::size_t{3});

    auto write = [&](auto&& client, auto&& should, io::file::index_t no_files,
                     auto&& dep) {
      fs::remove(should, comm_world);
      auto f = client.new_file();
      client.write(f);
      auto&& fg = s[client.name()];

      CHECK(fg.last().name() == string{should});
      CHECK(fg.last().index() == no_files - 1);
      CHECK(fg.size() == no_files);
      CHECK(fs::exists(should, comm_world));
      CHECK(fg.last().path() == string{should});

      if (string{dep} == string{}) {
        CHECK(fg.has_dependency() == false);
      } else {
        CHECK(fg.has_dependency() == true);
        CHECK(fg.dependency().name() == string{dep});
      }
    };

    auto check_dep_idx = [&](auto& client, auto idx) {
      CHECK(s[client.name()].last().dependency_index() == idx);
    };

    write(g, "grid_0", 1, "");
    {
      write(l, "level_set_0_0", 1, "grid");
      check_dep_idx(l, 0);
      write(d, "dynamic_geometry_0", 1, "level_set_0");
      check_dep_idx(d, 0);

      write(l, "level_set_0_1", 2, "grid");
      check_dep_idx(l, 0);
      write(d, "dynamic_geometry_1", 2, "level_set_0");
      check_dep_idx(d, 1);

      write(l, "level_set_0_2", 3, "grid");
      check_dep_idx(l, 0);
      write(d, "dynamic_geometry_2", 3, "level_set_0");
      check_dep_idx(d, 2);
    }

    write(g, "grid_1", 2, "");
    {
      write(l, "level_set_0_3", 4, "grid");
      check_dep_idx(l, 1);
      write(d, "dynamic_geometry_3", 4, "level_set_0");
      check_dep_idx(d, 3);

      write(l, "level_set_0_4", 5, "grid");
      check_dep_idx(l, 1);
      write(d, "dynamic_geometry_4", 5, "level_set_0");
      check_dep_idx(d, 4);

      write(l, "level_set_0_5", 6, "grid");
      check_dep_idx(l, 1);
      write(d, "dynamic_geometry_5", 6, "level_set_0");
      check_dep_idx(d, 5);
    }
    write(g, "grid_2", 3, "");
    check_dep_idx(l, 1);
    check_dep_idx(d, 5);
    CHECK(s.size() == std::size_t{3});
  }

  {  // read a session
    io::session s2(io::restart, session_name, comm_world);
    CHECK((s2 == s));
  }

  return test::result();
}
