#pragma once
/// \file
///
/// VTK Reader interface (implemented by every VTK reader)
#ifdef HM3_ENABLE_VTK
#include <hm3/types.hpp>
#include <hm3/vis/vtk/vtk.hpp>
#include <hm3/io/client.hpp>
#include <hm3/utility/log.hpp>
#include <hm3/utility/optional.hpp>
#include <map>
#include <memory>
#include <string>

namespace hm3 {
namespace vis {
namespace vtk {

/// VTK Reader interface
///
/// All readers implement this interface
struct reader;

/// Map from reader name to reader interface
///
/// Used to store multiple readers polymorphically in paraview plugins
using readers = std::map<string, std::unique_ptr<reader>>;

using optional_dependency = opt::optional<std::pair<string, io::file::index_t>>;

struct reader {
  virtual ~reader() {}

  /// Initializes the reader
  void initialize(string const& reader_name, io::session& session, readers& rs,
                  const int block_idx) {
    if (status != reader_status::uninitialized) { return; }
    log = hm3::log::serial("reader: " + reader_name, log.console_output());
    log("Initializing reader...", reader_name);
    io_        = io::client(session, reader_name);
    readers_   = &rs;
    block_idx_ = block_idx;
    initialize_();
    log("... reader initialized!", reader_name);
  }

  /// Updates the vtkDataset \p output with the reader data corresponding to
  /// file number \p file_idx
  void update(vtkMultiBlockDataSet* output, int file_idx) {
    log("Updating reader...", io_.name());
    update_(output, file_idx);
    log("...reader updated!", io_.name());
  }

  /// Visualization level: -1 means leaf level, < -1 means no level, > -1 means
  /// the input level
  bool set_visualization_level(const int level) noexcept {
    return set_visualization_level_(level);
  }

  /// Returns if the reading file \p file_idx has a dependency
  /// If so, it returns which dependency and which file of it
  optional_dependency dependency(int file_idx) const noexcept {
    auto f = io_.get_file(io::file::index_t{file_idx});
    /// If it doesn't have a dependency, return empty
    if (!f.has_dependency()) { return optional_dependency{}; }
    return optional_dependency{
     std::make_pair(f.dependency_group_name(), f.dependency_index())};
  }

  bool updated = false;

  /// Set field \p name to value \p value
  ///
  /// Returns true if the value of any field has changed!
  ///
  /// Note: see hm3::vis::field::value_t for the possible values
  /// that a field can take (e.g. active/inactive
  ///
  /// TODO: once more solvers are implemented consider making value
  /// a boolean and simplifing the whole field thing
  bool field(string const& name, const int value) noexcept {
    return field_(name, value);
  }
  /// Get value of field \p name
  int field(string const& name) const noexcept { return field_(name); }

  /// Get field name of field number \p index
  const char* field(const int index) const noexcept { return field_(index); }

  /// Number of fields in the reader
  int number_of_fields() const noexcept { return number_of_fields_(); }

  /// Number of files of the reader
  int no_files() const { return *io_.no_files(); }

  /// Index of the current loaded file
  int current_file() const noexcept { return last_file_idx_; }

 protected:
  /// Loads file number \p file_idx
  void load_file(int file_idx) {
    HM3_ASSERT(file_idx >= 0, "");
    // If file didn't change no need to update the file
    if (last_file_idx_ == file_idx) {
      log("File did not change!");
      return;
    }

    log("File changed from {} to {}", last_file_idx_, file_idx);
    log("Loading new file...");
    load_file_(file_idx);

    // update the last file loaded and set that the grid topology has changed
    last_file_idx_ = file_idx;
    status         = reader_status::topology_changed;
    log("... new file loaded!");
  }

  /// \name Reader hooks
  ///
  /// Each reader has to define these hooks to model the reader interface.
  /// Note that these are called by the non-virtual interface of the reader
  /// API, which performs some common operations before and after calling
  /// these hooks (the readers don't need to perform these operations
  /// themselves).
  ///
  ///@{

  /// Initializes the reader
  virtual void initialize_() = 0;
  /// Updates the vtkMultiBlockDataSet \p output with the data of the file \p
  /// file_idx of the reader
  virtual void update_(vtkMultiBlockDataSet* output, int file_idx) = 0;
  /// Sets the reader visualization level to \p level
  virtual bool set_visualization_level_(const int level) noexcept = 0;
  /// Sets the value of the data field \p name to \p value and returns true if
  /// the value changed or false otherwise
  virtual bool field_(string const& name, const int value) noexcept = 0;
  /// Index of the data field \p name within the readers data set
  virtual int field_(string const& name) const noexcept = 0;
  /// Name of the data field number \p index
  virtual const char* field_(const int index) const noexcept = 0;
  /// Number of data fields in the reader
  virtual int number_of_fields_() const noexcept = 0;
  /// Loads the file number \p file_idx
  virtual void load_file_(int file_idx) = 0;

  ///@}  // Reader hooks

 protected:
  /// The reader's io client
  io::client io_;

  /// A pointer back to the collection of readers of the plugin
  /// (Used by readers that need to query information from their dependencies,
  /// like a solver reader from a grid or geometry reader)
  readers* readers_;

  /// Index of the reader within the plugin (indicates which data set the reader
  /// needs to update within a vtkMultiBlockDataSet)
  int block_idx_ = -1;
  /// Last file loaded by the reader
  int last_file_idx_ = -1;

  /// Reader status
  enum class reader_status {
    /// To initialize the reader call: initialize(...)
    uninitialized,
    /// Grid topology is ready
    topology_ready,
    /// Grid topology changes (a new vtk grid might need to be created)
    topology_changed
  };
  /// Status of the reader
  reader_status status = reader_status::uninitialized;

 public:
  /// Log of the reader
  hm3::log::serial log;
};

}  // namespace vtk
}  // namespace vis
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
