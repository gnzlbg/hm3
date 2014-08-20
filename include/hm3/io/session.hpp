#pragma once
/// \file
///
/// IO session
#include <type_traits>
#include <hm3/io/file.hpp>
#include <hm3/io/file_system.hpp>

namespace hm3 {
namespace io {

struct create_t {};
struct restart_t {};

static constexpr create_t create{};
static constexpr restart_t restart{};

/// Manages io from interdependent multiple sources
///
/// The sources might depend on each other, e.g., a grid and multiple solvers as
/// well as moving geometry.
struct session {
 private:
  string name_;           ///< Session name
  string dir_path_ = "";  ///< Session working directory
  json data_;             ///< Session data (in json format)
  mpi::comm comm_;        ///< Session communicator

  string const& name() const { return name_; }

  using index_t = typename file::index_t;

  /// A group of files within an io::session.
  ///
  /// Note: the file groups are managed by the io::session,
  /// there shouldn't be any need to manage them directly.
  ///
  /// Each group contains the following:
  /// - name: group name
  /// - type: group type
  /// - files: list of files
  /// - dependency (optional): a dependency to another group
  ///   (e.g. the solution data of a solver generally depends on the grid data)
  struct file_group {
   private:
    using index_t = typename file::index_t;

   public:
    /// Serialize from json
    file_group(io::session const& s, io::json g = io::json{})
     : session_(s), file_group_(std::move(g)) {}

    /// Create empty file group
    file_group(io::session const& s, string const& name, string const& type,
               string const& dependency = string{})
     : session_(s)
     , file_group_{
        {"name", name},  //
        {"type", type},  //
        {"files", {}}    //
       } {
      if (!dependency.empty()) { file_group_["dependency"] = dependency; }
    }

    /// File group name
    string name() const { return *(file_group_.find("name")); }

    /// File group type
    string type() const { return *(file_group_.find("type")); }

    /// Is the file group empty (i.e. it has no files)?
    bool empty() const { return file_group_["files"].is_null(); }

    /// Serialize the file-group to JSON
    explicit operator io::json() const { return file_group_; }

    /// Has the file group a dependency?
    bool has_dependency() const {
      return file_group_.find("dependency") != file_group_.end() ? true : false;
    }

    /// Name of the file group dependency
    /// \pre The file must have a dependency.
    string dependency_name() const {
      HM3_ASSERT(has_dependency(), "");
      return *(file_group_.find("dependency"));
    }

    /// File group of the group's dependency
    /// \pre The file must have a dependency.
    file_group dependency() const {
      HM3_ASSERT(has_dependency(), "");
      return session_[dependency_name()];
    }

    /// Number of files in the group
    index_t size() const { return file_group_["files"].size(); };

    /// Last file
    file last() {
      HM3_ASSERT(!empty(), "Empty file groups don't have a last file!");
      index_t i;
      file f{};
      for (auto&& fj : file_group_["files"]) {
        auto j = index_t{fj["index"].get<int_t>()};
        if (j > i) {
          i = j;
          f = file{fj};
        }
      }
      HM3_ASSERT(i, "file group not empty but no file found?!");
      f.set_dir(session_.dir_path());
      return f;
    }

    /// Add a file to the file group
    void push(file const& f) {
      HM3_ASSERT(name() == f.group_name(), "");
      HM3_ASSERT(f.index(), "");
      HM3_ASSERT(has_dependency() == f.has_dependency(), "");
      if (has_dependency()) {
        HM3_ASSERT(dependency_name() == f.dependency_group_name(), "");
      }
      if (!empty()) {
        auto last_f = last();
        HM3_ASSERT(f.index() > last_f.index(), "");
        HM3_ASSERT(f.index() == last_f.index() + 1, "");
        HM3_ASSERT(f.group_name() == last_f.group_name(), "");
        if (has_dependency()) {
          HM3_ASSERT(f.has_dependency(), "");
          HM3_ASSERT(f.dependency_index() >= last_f.dependency_index(), "");
          HM3_ASSERT(
           f.dependency_group_name() == last_f.dependency_group_name(), "");
        }
      }
      file_group_["files"].push_back(f.header());
    }

    file operator[](index_t i) const {
      HM3_ASSERT(!empty(), "file group is empty!");
      HM3_ASSERT(i, "invalid file index!");
      auto f = file{file_group_["files"][*i]};
      f.set_dir(session_.dir_path());
      return f;
    }

    friend bool operator==(file_group const& a, file_group const& b) {
      return a.file_group_ == b.file_group_;
    }

   private:
    io::session const& session_;
    io::json file_group_;
  };

 public:
  /// File extension of a session file
  static string extension() { return "hm3_session"; }
  /// File name of the session
  string file_name() const { return fs::make_file_name(name(), extension()); }
  /// File path to the session files
  string file_path() const {
    return fs::make_file_path(dir_path(), name(), extension());
  }
  /// Path to the working directory
  string const& dir_path() const { return dir_path_; }

  /// Is a session file
  static bool is_file(string const& name) {
    return fs::extension(name) == extension();
  }

  /// Removes the session file for a session with name or path \p path
  static bool remove(string path, mpi::comm const& c) {
    if (fs::split_extension(path).second.empty()) {
      path = fs::make_file_name(path, extension());
    }
    return fs::remove(path, c);
  }

  session(mpi::comm comm) : comm_{std::move(comm)} {}
  session(session const& other) = default;
  session(session&&) = default;
  session& operator=(session const& other) = default;
  session& operator=(session&&) = default;

  /// Create a new session
  session(create_t, string const& path, mpi::comm comm)
   : comm_{std::move(comm)} {
    set_file(create, path);
  }

  /// Restart from an already existing session
  session(restart_t, string const& path, mpi::comm comm)
   : comm_{std::move(comm)} {
    set_file(restart, path);
  }

 private:
  void set_path_extension(string& path) {
    if (fs::split_extension(path).second.empty()) {
      path = fs::make_file_name(path, extension());
    }
    string ext{};
    std::tie(dir_path_, name_, ext) = fs::split_path(path);
    HM3_ASSERT(
     ext == extension(),
     "extension \"{}\" of io::session at path \"{}\" does not match \"{}\"",
     ext, path, extension());
  }

  void set_file(create_t, string path) {
    set_path_extension(path);
    if (fs::exists(path, comm_)) {
      HM3_FATAL_ERROR(
       "Cannot create new session: {}, file: \"{}\" already exists!", name_,
       path);
    }
  }

  void set_file(restart_t, string path) {
    set_path_extension(path);
    if (!fs::exists(path, comm_)) {
      HM3_FATAL_ERROR("Cannot restart session {}, file: \"{}\" not found!",
                      name_, path);
    }
    data_ = json::parse(fs::read_whole(path, comm_));
  }

 public:
  /// Has the session the file group
  bool has(string const& file_group_name) const {
    return data_.find(file_group_name) != data_.end() ? true : false;
  }

  /// Get the file group
  file_group operator[](string const& file_group_name) {
    if (!has(file_group_name)) {
      std::cerr << std::setw(2) << data_ << std::endl;
      HM3_FATAL_ERROR(
       "entry \"{}\" is not registered in the io::session \"{}\"\n\n"
       "io::session dump:\n",
       file_group_name, name_);
    }
    return file_group{*this, data_[file_group_name]};
  }
  file_group operator[](string const& file_group_name) const {
    return const_cast<session&>(*this)[file_group_name];
  }

  /// Number of file groups
  auto size() const { return data_.size(); }
  auto begin() const { return ranges::begin(data_); }
  auto end() const { return ranges::end(data_); }

  /// Appends a file group to the io::session
  void push_group(string const& file_group_name, string const& file_group_type,
                  string const& dependency = string{}) {
    if (has(file_group_name)) {
      HM3_FATAL_ERROR(
       "entry \"{}\" is already registered in the io::session \"{}\"",
       file_group_name, name_);
    }

    data_[file_group_name] = static_cast<io::json>(
     file_group{*this, file_group_name, file_group_type, dependency});
  }

  void write(file& f) {
    auto file_group_name = f.group_name();
    auto fg              = (*this)[file_group_name];
    auto i               = fg.size();

    HM3_ASSERT(!f.index(), "file index is not invalid but {}", f.index());
    f.index(i);

    HM3_ASSERT(f.dependency_group_name().empty(), "");
    HM3_ASSERT(!f.dependency_index(), "dependency index is not invalid but {}",
               f.dependency_index());

    if (fg.has_dependency()) {
      f.dependency_group_name(fg.dependency_name());
      f.dependency_index(fg.dependency().last().index());
    }

    /// Write binary data
    f.write_data(comm_);

    // If writing data succeeds:

    fg.push(f);  // Modify the file group

    /// Push the modifications into the session
    /// and write the session to a file
    /// TODO: this should be guarded by a mutex
    {
      data_[file_group_name] = static_cast<io::json>(fg);

      write();  // write the session to a file
    }
  }

  template <typename OStream>
  friend OStream& operator<<(OStream& os, session const& s) {
    os << s.data_.dump(2);
    return os;
  }

  void write() {
    std::ofstream o{file_path()};
    o << (*this);
  }

  inline bool operator==(session const& other) { return data_ == other.data_; }
};

}  // namespace io
}  // namespace hm3
