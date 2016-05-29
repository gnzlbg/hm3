#pragma once
/// \file
///
/// Helper class to interact with an I/O session
#include <hm3/io/session.hpp>

namespace hm3 {
namespace io {

/// Helps a single file group interact with an io::session
struct client {
 private:
  io::session* session_;
  string name_;
  string type_;
  string dependency_;

  // buffer
 public:
  client()              = default;
  client(client const&) = default;
  client(client&&)      = default;
  client& operator=(client const&) = default;
  client& operator=(client&&) = default;

  /// Read client \p name from sesion \p s
  client(io::session& s, string name) : session_(&s), name_(std::move(name)) {
    if (!session().has(name_)) {
      HM3_FATAL_ERROR("Could not find client \"{}\" in session \"{}\"", name_,
                      s.file_path());
    }
    if (session()[name_].has_dependency()) {
      dependency_ = session()[name_].dependency_name();
    }

    type_ = session()[name_].type();
  }

  /// Registers client with the session
  client(io::session& s, string const& name, string const& type,
         string const& dependency = string{})
   : session_(&s) {
    // if not in the session, we just add it to the session
    if (!session().has(name)) {
      name_       = name;
      type_       = type;
      dependency_ = dependency;
      session().push_group(name_, type_, dependency_);
      HM3_ASSERT(no_files() == 0, "");
      return;
    }
    // Group already registered: we must be reading from a session
    // read the session:
    (*this) = io::client(s, name);

    // verify arguments:
    if (type_ != type) {
      HM3_FATAL_ERROR("mismatching file group types for client:\n\n  name: "
                      "{}\n  type: {}\n  "
                      "dependency: {}\n\n  passed type: \"{}\"\n\n",
                      name_, type_, dependency_, type);
    }
    if (!dependency_.empty() && dependency_ != dependency) {
      HM3_FATAL_ERROR(
       "mismatching dependency names\n\n  name: {}\n  type: {}\n  "
       "dependency: {}\n\n  passed dependency: {}\n\n",
       name_, type_, dependency_, dependency);
    }
  }

  /// Returns a reference to the underlying io::session
  io::session& session() {
    if (session_ == nullptr) {
      HM3_FATAL_ERROR("io::client for \"{}\" is not initialized (null session)",
                      name_);
    }
    return *session_;
  }

  /// Returns a reference to the underlying io::session
  io::session const& session() const {
    return const_cast<io::client&>(*this).session();
  }

  /// Client name
  string const& name() const { return name_; }

  /// Client type
  string const& type() const { return type_; }

  /// Has the client a dependency
  bool has_dependency() const { return !dependency_.empty(); }

  /// Client dependency name
  string const& dependency() const { return dependency_; }

  /// Empty file for the file group
  io::file new_file() const { return io::file(name_); }

  /// Write file
  ///
  /// Note: \p f should have been obtained from a call to `file()`.
  void write(io::file& f) {
    HM3_ASSERT(f.group_name() == name_, "");
    session().write(f);
  }

  /// Number of files for this group in the session
  io::file::index_t no_files() const { return session()[name_].size(); }

  /// Get the file with index \p i to read it (by default returns the last
  /// file)
  io::file get_file(io::file::index_t i = io::file::index_t{}) const {
    HM3_ASSERT(
     no_files() > 0,
     "Trying to obtain file \"{}\" of file-group without files \"{}\"!", i,
     name_);

    // By default return the last file
    i = !i ? no_files() - 1 : i;
    HM3_ASSERT(i < no_files(),
               "file index \"{}\" is out-of-bounds (fg: \"{}\")", i, name_);

    return session()[name_][*i];
  }
};

}  // namespace io
}  // namespace hm3
