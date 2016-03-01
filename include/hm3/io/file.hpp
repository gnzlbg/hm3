#pragma once
/// \file
///
/// Describes a single file
#include <fstream>
#include <hm3/io/file_system.hpp>
#include <hm3/io/json.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/compact_optional.hpp>
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/log.hpp>
#include <hm3/utility/mpi.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace io {

struct array_data_t {};
static constexpr array_data_t array_data{};

inline string data_type(int_t) { return "integer"; }
inline string data_type(uint_t) { return "unsigned integer"; }

CONCEPT_REQUIRES(!Same<sint_t, int_t>{})
inline string data_type(sint_t) { return "integer"; }

CONCEPT_REQUIRES(!Same<suint_t, uint_t>{})
inline string data_type(suint_t) { return "unsigned integer"; }

inline string data_type(num_t) { return "number"; }
inline string data_type(bool) { return "boolean"; }

template <typename T> bool correct_datatype(T, string const& s) {
  return data_type(T{}) == s;
}

/// Describes a single file
///
/// Its JSON serialization contains the following:
/// - group_name: file group name
/// - index: index of the file within its group
/// - dependency_group_name (optional): file group name of its dependency
/// - dependency_index (optional): index of the dependency within its group
/// - file fields:
///   - name: field name
///   - size: number of elements in the field
///   - in_file [begin, end): location of data within the file
///   - type: integer, number, boolean
///
struct file {
  using index_t
   = compact_optional<empty_scalar_value<int_t, -1>, struct file_index_tag>;
  /// File identifier:
  /// - group name
  /// - file index within group
  struct id {
    using type   = index_t;
    string group = "";
    type index   = type{};
    /// File name
    string name() const {
      HM3_ASSERT(group.size() != 0, "group name not set");
      return !index ? group : group + "_" + std::to_string(*index);
    }
  };

 private:
  id current_;     ///< Current file identifier
  id dependency_;  ///< Identifier of current file's dependency

  io::json fields_ = io::json{};  ///< Data fields in the file

  /// Position of data fields in memory
  std::map<string, std::pair<void*, void*>> in_memory_;
  /// Callbacks to execute before writing each field
  std::map<string, std::function<void()>> execute_before_write_;

  string dir_path_ = "";

  hm3::log::serial log_ = string{"file_log"};

 public:
  /// Does the file have a dependency?
  bool has_dependency() const {
    if (dependency_.group.size() == 0) { return false; }

    HM3_ASSERT(dependency_.group.size() != 0,
               "dependency group not initialized");
    HM3_ASSERT(dependency_.index, "dependency index not initialized");

    return true;
  }

  /// File name
  string name() const { return current_.name(); }

  void set_dir(string const& dir_path) { dir_path_ = dir_path; }

  /// Full file path
  string path() const { return fs::make_file_path(dir_path_, name(), ""); }

  /// Group name of the file
  string const& group_name() const { return current_.group; }

  /// File index
  index_t index() const { return current_.index; }

  /// Set file index
  void index(index_t i) { current_.index = i; }

  /// File name
  string dependency_name() const { return dependency_.name(); }

  /// Full file path
  string dependency_path() const {
    return fs::make_file_path(dir_path_, dependency_name(), "");
  }

  /// Group name of the file
  string const& dependency_group_name() const { return dependency_.group; }

  /// Dependency file index
  index_t dependency_index() const { return dependency_.index; }

  /// Dependency group name
  void dependency_group_name(string const& g) { dependency_.group = g; }

  /// Set dependency file index
  void dependency_index(index_t i) { dependency_.index = i; }

 private:
  id& current() { return current_; }
  id current() const { return current_; }
  id& dependency() { return dependency_; }
  id dependency() const { return dependency_; }

 public:
  //////////////////////////////////////////////////////////////////////////////
  /// \name File fields
  ///
  /// These functions allow setting the fields of a file for reading and writing
  /// If an operation has to be performed right before writing a file (such as
  /// computing something and storing it in a temporary buffer) these can be
  /// specified on the "before_write" function arguments.
  ///
  ///@{

  bool has_field(string const& field_name) const {
    return ::hm3::io::has_field(fields_, field_name);
  }

  bool has_subfield(string const& field_name,
                    string const& subfield_name) const {
    return has_field(field_name)
           && ::hm3::io::has_field(fields_[field_name], subfield_name);
  }

  /// Adds field named \p field_name of constant \p value:
  template <typename T> file& field(string const& field_name, T value) {
    HM3_ASSERT(!has_field(field_name),
               "file \"{}\" already has a field named \"{}\"", name(),
               field_name);
    fields_[field_name]               = value;
    execute_before_write_[field_name] = []() {};
    return *this;
  }

  /// Validates an array field present in the file:
  ///
  /// - the field must be in the file
  /// - the field must contain the size amd in_file subfields
  /// - the byte size of the data-type stored in the file must match
  ///   the byte size of the data-type to store it in memory
  /// - the number of elements in the file must match the number of
  ///   elements in memory
  template <typename T>
  bool validate_array_field_in_file(string const& field_name, T* begin,
                                    T* end) const {
    bool valid = true;
    if (!has_field(field_name)) {
      fmt::print("File \"{}\" doesn't have field \"{}\"\n", name(), field_name);
      return false;
    }
    if (!has_subfield(field_name, "in_file")) {
      fmt::print("File \"{}\"'s field \"{}\" doesn't have sub-field \"{}\"\n",
                 name(), field_name, "in_file");
      valid = false;
    }
    if (!has_subfield(field_name, "size")) {
      fmt::print("File \"{}\"'s field \"{}\" doesn't have sub-field \"{}\"\n",
                 name(), field_name, "size");
      valid = false;
    }
    if (!has_subfield(field_name, "type")) {
      fmt::print("File \"{}\"'s field \"{}\" doesn't have sub-field \"{}\"\n",
                 name(), field_name, "type");
      valid = false;
    }
    if (!valid) { return false; }

    auto file_size = fields_[field_name]["size"].get<int_t>();
    int_t mem_size = end - begin;

    if (file_size != mem_size) {
      fmt::print("Field's \"{}\" file size \"{}\" is different from its memory "
                 "size \"{}\"\n",
                 field_name, file_size, mem_size);
      valid = false;
    }

    int_t file_bytes = fields_[field_name]["in_file"][1].get<int_t>()
                       - fields_[field_name]["in_file"][0].get<int_t>();
    int_t mem_bytes = mem_size * sizeof(T);

    if (file_bytes != mem_bytes) {
      fmt::print("field \"{}\" #of bytes in file \"{}\" does not match the #of "
                 "bytes in memory \"{}\"\n",
                 field_name, file_bytes, mem_bytes);
      valid = false;
    }

    auto field_type = fields_[field_name]["type"].get<string>();
    auto mem_type   = data_type(T{});
    if (field_type != mem_type) {
      fmt::print("field \"{}\"'s type \"{}\" does not match the type of memory "
                 "\"{}\"\n",
                 field_name, field_type, mem_type);
      valid = false;
    }

    return valid;
  }

  /// Add an array [begin, end) named \p field_name to the file
  template <typename T>
  file& field(string const& field_name, T* begin, T* end,
              std::function<void()> before_write = []() {}) {
    int_t size = end - begin;
    using T_   = ranges::uncvref_t<T>;
    auto p
     = std::pair<void*, void*>(reinterpret_cast<void*>(const_cast<T_*>(begin)),
                               reinterpret_cast<void*>(const_cast<T_*>(end)));
    // If field doesn't exist we are adding a field to write from memory into
    // the file:
    if (!has_field(field_name)) {
      fields_[field_name] = {{"size", size}, {"type", data_type(T{})}};
      execute_before_write_[field_name] = before_write;
    } else {  // Otherwise, reading an array field from a file into memory:
      HM3_ASSERT(validate_array_field_in_file(field_name, begin, end),
                 "Array field \"{}\" in file \"{}\" is not valid!", field_name,
                 name());
    }
    in_memory_[field_name] = p;
    return *this;
  }

  /// Add an array [data(array), data(array) + size) named \p field_name to the
  /// file
  template <typename T>
  file& field(string const& field_name, T const& array, array_data_t) {
    return field(field_name, array.data(), array.data() + array.size());
  }

  /// Add an array [data, data + size) named \p field_name to the file
  template <typename T>
  file& field(string const& field_name, T* data, std::size_t size) {
    return field(field_name, data, data + size);
  }

  //////////////////////////////////////////////////////////////////////////////
  /// \name Read constant fields
  ///
  /// These functions disambiguate reading from constant fields and return
  /// the result by value.
  ///
  ///@{

  template <typename T, CONCEPT_REQUIRES_(Range<T>{})>
  auto constant(string const& constant_name, T = T{}) const -> T {
    std::vector<num_t> v = fields_[constant_name];
    return T{ranges::begin(v), ranges::end(v)};
  }

  template <typename T, CONCEPT_REQUIRES_(!Range<T>{})>
  auto constant(string const& constant_name, T = T{}) const -> T {
    return fields_[constant_name].get<T>();
  }

  ///@}  // Read constant fields
  //////////////////////////////////////////////////////////////////////////////

  int_t field_size(string const& field_name) const {
    HM3_ASSERT(has_field(field_name), "field \"{}\" not found", field_name);
    HM3_ASSERT(has_subfield(field_name, "size"), "field \"{}\" is not an array",
               field_name);
    return fields_[field_name]["size"];
  }

  ///@}  // File fields
  //////////////////////////////////////////////////////////////////////////////

  file() = default;

  /// Creates a file without a dependency
  file(string const& group_name, index_t index = index_t{})
   : current_(id{group_name, std::move(index)}) {}

  /// Creates a file without a dependency
  file(const char* group_name) : file(string{group_name}) {}

  /// Serialize from json header
  explicit file(io::json header) {
    bool valid_header = true;
    auto verify       = [&](auto&& it, auto&& name) {
      if (it == end(header)) {
        fmt::print("Invalid file header, subfield \"{}\" not found!\n", name);
        valid_header = false;
      }
    };

    auto g = header.find("group_name");
    auto i = header.find("index");
    auto f = header.find("fields");

    // Every header must have these 3 fields:
    verify(g, "group_name");
    verify(i, "index");
    verify(f, "fields");

    if (!valid_header) {
      HM3_FATAL_ERROR("Invalid file header:\n\n{}", header.dump(2));
    }

    current_.group = static_cast<string const&>(*g);
    current_.index = static_cast<int_t>(*i);
    fields_        = *f;

    auto d = header.find("dependency_group_name");
    if (d != header.end()) {
      dependency_.group
       = static_cast<string const&>(*(header.find("dependency_group_name")));
      dependency_.index
       = static_cast<int_t>(*(header.find("dependency_index")));
    }
  }

  // Serialize header to json
  io::json header() const {
    HM3_ASSERT(current_.index, "current file index is invalid");
    io::json j{
     {"group_name", current_.group},  //
     {"index", *current_.index},      //
     {"fields", fields_}              //
    };

    if (has_dependency()) {
      HM3_ASSERT(dependency_.index,
                 "group {} has dependency {} but dependency index is invald",
                 current_.group, dependency_.group);
      j["dependency_group_name"] = dependency_.group;
      j["dependency_index"]      = *dependency_.index;
    }
    return j;
  }

 private:
  void prepare_write(mpi::comm const&) {
    int_t data_p   = 0;
    using byte_ptr = char const* const;
    for (auto&& f : in_memory_) {
      auto key                = f.first;
      auto in_mem_b           = reinterpret_cast<byte_ptr>(f.second.first);
      auto in_mem_e           = reinterpret_cast<byte_ptr>(f.second.second);
      auto size_bytes         = in_mem_e - in_mem_b;
      fields_[key]["in_file"] = {data_p, data_p + size_bytes};
      data_p += size_bytes;
    }
  }

 public:
  void write_data_only(mpi::comm const&) const {
    log_("writing file \"{}\" arrays...", path());
    log_("- with header:\n\n{}\n", header().dump(2));
    log_("- field | mem: [from, to) | size:");
    std::ofstream o(path());
    using byte_ptr = char const* const;
    for (auto&& f : in_memory_) {
      const auto mb     = reinterpret_cast<byte_ptr>(f.second.first);
      const auto me     = reinterpret_cast<byte_ptr>(f.second.second);
      const auto m_size = me - mb;
      log_("    {} | mem: [{}, {}) : {}", f.first,
           reinterpret_cast<void const*>(mb), reinterpret_cast<void const*>(me),
           m_size);
      o.write(mb, m_size);
    }
    log_("...writing file \"{}\" arrays done!", path());
  }

  void write_data(mpi::comm const& c) {
    prepare_write(c);
    write_data_only(c);
  }

  void read_arrays() {
    log_("reading file \"{}\" arrays...", path());
    log_("- with header:\n\n{}\n", header().dump(2));
    log_("- field | file [from, to) : size | mem: [from, to) : size:");
    std::ifstream fh(path());
    if (fh.fail()) { HM3_FATAL_ERROR("couldn't open file: \"{}\"", path()); }
    using byte_ptr = char*;
    for (auto i = begin(fields_), e = end(fields_); i != e; ++i) {
      auto f = *i;
      auto n = i.key();
      if (in_memory_.count(n)) {
        auto mr     = in_memory_[n];
        auto mb     = reinterpret_cast<byte_ptr>(mr.first);
        auto me     = reinterpret_cast<byte_ptr>(mr.second);
        auto m_size = me - mb;
        auto fr     = f["in_file"];
        auto fb     = fr[0].get<int_t>();
        auto fe     = fr[1].get<int_t>();
        auto f_size = fe - fb;

        HM3_ASSERT(m_size == f_size, "size_bytes differs");

        fh.seekg(fr[0], std::ios_base::beg);
        fh.read(mb, m_size);
        log_("    {} | file [{}, {}) : {} | mem [{}, {}) : {} | succeded: {}",
             n, fb, fe, f_size, reinterpret_cast<void*>(mb),
             reinterpret_cast<void*>(me), m_size, fh.fail() ? "false" : "true");
      }
    }
    log_("...reading file\"{}\" arrays done!", path());
  }
};

}  // namespace io
}  // namespace hm3
