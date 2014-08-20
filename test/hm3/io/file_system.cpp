/// \file file_system.cpp File system tests
#include <hm3/io/file_system.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  string session_name{"some_session"};
  string session_ext{"hm3_session"};
  string session_dir{"path/to"};
  string session_dir2{};
  string session_dir3{"path/to/"};

  string session_file_name{"some_session.hm3_session"};
  string session_file_path{"path/to/some_session.hm3_session"};
  string session_file_path2{"/some_session.hm3_session"};
  string session_file_path3{"some_session.hm3_session"};

  string data_file_name{"hierarchical_cartesian_0"};
  string data_file_path{"path/to/hierarchical_cartesian_0"};

  CHECK(std::make_pair(session_name, session_ext)
        == fs::split_extension(session_file_name));

  CHECK(std::make_pair(session_dir, session_file_name)
        == fs::split_dir(session_file_path));

  CHECK(std::make_tuple(session_dir, session_name, session_ext)
        == fs::split_path(session_file_path));

  CHECK(std::make_pair(session_dir2, session_file_name)
        == fs::split_dir(session_file_path2));

  CHECK(std::make_tuple(session_dir2, session_name, session_ext)
        == fs::split_path(session_file_path2));

  CHECK(std::make_pair(session_dir2, session_file_name)
        == fs::split_dir(session_file_path3));

  CHECK(std::make_tuple(session_dir2, session_name, session_ext)
        == fs::split_path(session_file_path3));

  CHECK(std::make_pair(data_file_name, string{})
        == fs::split_extension(data_file_name));

  CHECK(std::make_pair(string{}, data_file_name)
        == fs::split_dir(data_file_name));

  CHECK(std::make_pair(session_dir, data_file_name)
        == fs::split_dir(data_file_path));

  CHECK(std::make_tuple(session_dir, data_file_name, string{})
        == fs::split_path(data_file_path));

  CHECK(std::make_tuple(session_dir, string{}, string{})
        == fs::split_path(session_dir3));

  return test::result();
}
