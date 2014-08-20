/// \file vtkHM3Reader.cxx Paraview's VTK Reader
///
/// Notes about the reader:
/// - can read any session file and creates readers for
///   each type of data (solvers, geometry, ...) automatically
/// - each type of data is in its own block within vtkMultiBlockDataSet
/// - logs everything
/// - exceptions propagate to paraview crashing it, but the log contains
///   their origin (if this happens it is a bug)
//
#include "vtkHM3Reader.h"
#include <iostream>
#include <hm3/vis/vtk/readers/factory.hpp>
#include <hm3/utility/exception.hpp>

vtkStandardNewMacro(vtkHM3Reader);

using namespace hm3;
using namespace vis;
using namespace vtk;

vtkHM3Reader::vtkHM3Reader() try : session_(mpi::comm::world()) {
  SetNumberOfInputPorts(0);
  SetNumberOfOutputPorts(1);
}
HM3_REPORT_EXCEPTIONS

vtkHM3Reader::~vtkHM3Reader() try { SetFileName(NULL); }
HM3_REPORT_EXCEPTIONS

void vtkHM3Reader::SetFileName(const char* file_name) try {
  string fname = file_name ? file_name : string{};

  if (file_path_ != fname && !fname.empty()) {
    file_path_ = fname;

    log("session file_path: {}\n", file_path_);

    session_ = io::session(io::restart, file_path_, mpi::comm::world());
    initialize_readers();
  }

  this->Modified();  // TODO: put this inside the if!?
}
HM3_LOG_EXCEPTIONS

string vtkHM3Reader::dump_reader() const try {
  string dump = hm3::fmt::format("Session file path: {}\n", file_path_);
  return dump;
}
HM3_LOG_EXCEPTIONS

void vtkHM3Reader::PrintSelf(ostream& os, vtkIndent indent) try {
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Reader state dump (check also hm3.log/stdout):" << endl;
  os << indent << dump_reader() << endl << endl;

  log("Dumping reader state:\n\n{}\n\n", dump_reader());
}
HM3_LOG_EXCEPTIONS

int vtkHM3Reader::CanReadFile(const char* file_name) try {
  return io::session::is_file(file_name);
}
HM3_REPORT_EXCEPTIONS

/// Compute the maximum number of files
int vtkHM3Reader::max_number_of_files() const {
  int no_files = 0;
  for (auto&& r : readers_) {
    auto no_files_ = r.second->no_files();
    no_files       = std::max(no_files, no_files_);
    log("#of files in \"{}\": {}", r.first, no_files_);
  }
  log("max #of files: {}", no_files);
  return no_files;
}

// PROBLEM: this assumes that all readers have the exact same number of files
//
// TODO:
// - this should loop over all files of all blocks, and find
// an unique set of global time steps
// - other machinery should compute, from the time step selected,
// which file should be loaded from each block
int vtkHM3Reader::RequestInformation(
 vtkInformation* vtkNotUsed(request),
 vtkInformationVector** vtkNotUsed(inputVector),
 vtkInformationVector* outputVector) try {
  log("ParaView called RequestInformation...");
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  auto no_files = max_number_of_files();

  // hack: this assumes that all readers have the same number of files
  std::vector<double> time_steps(no_files);
  int no_files_ = 0;
  for (auto&& i : time_steps) { i = no_files_++; }

  outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(),
               time_steps.data(), time_steps.size());
  outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(),
               time_steps.data(), time_steps.size());
  log("...RequestInformation exit: back to ParaView!");
  return 1;
}
HM3_LOG_EXCEPTIONS

/// Updates a single reader
///
/// TODO: this assumes all readers have the same number of files. If that is not
/// the case one would need to check that if a reader is already updated, the
/// file it has loaded is the file that its dependency needs it to have loaded.
///
/// Otherwise two dependencies of a third reader could tell it to load two
/// different files.
void vtkHM3Reader::update_reader(std::unique_ptr<hm3::vis::vtk::reader>& reader,
                                 int time_step, vtkMultiBlockDataSet* mb) try {
  /// Reader was already updated
  if (reader->updated) { return; }
  auto d = reader->dependency(time_step);
  if (d) { update_reader(readers_[(*d).first], time_step, mb); }
  reader->update(mb, time_step);
  reader->updated = true;
}
HM3_LOG_EXCEPTIONS

int vtkHM3Reader::RequestData(vtkInformation* /*request*/,
                              vtkInformationVector** /*inputVector*/,
                              vtkInformationVector* outputVector) try {
  log("ParaView called RequestData...");
  vtkInformation* info = outputVector->GetInformationObject(0);

  int time_step
   = info->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP());

  log("Data at time step {} requested...", time_step);

  if (time_step >= max_number_of_files()) {
    // Paraview asks for e.g. timestep 2 even when requestinformation answers
    // that there is only 1 file...
    // Also when there are data-sets loaded with a larger number of files
    // paraview tries to call this reader with a file index from a different
    // data set...
    log.error("Time step {} requested but only {} time steps available...",
              time_step, max_number_of_files());
    time_step = max_number_of_files();
  }
  log("Loading file {}...", time_step);

  vtkDataObject* data_object = info->Get(vtkDataObject::DATA_OBJECT());
  vtkMultiBlockDataSet* mb = vtkMultiBlockDataSet::SafeDownCast(data_object);

  mb->GetInformation()->Set(vtkDataObject::DATA_TIME_STEP(), time_step);

  if (!mb) {
    log("...RequestData exit: back to ParaView!");
    return 0;
  }

  mb->SetNumberOfBlocks(readers_.size());

  /// Set all the readers to not updated yet
  for (auto&& reader : readers_) { reader.second->updated = false; }

  log("Updating output of all readers...");
  for (auto&& reader : readers_) {
    log("Updating reader: {}...", reader.first);
    update_reader(reader.second, time_step, mb);
    log("...reader {} updated!", reader.first);
  }

  /// Set all the readers to not updated yet
  for (auto&& reader : readers_) {
    HM3_ASSERT(reader.second->updated, "some redaer wasn't updated");
  }
  log("... output of all readers updated!");

  log("...RequestData exit: back to ParaView!");
  return 1;
}
HM3_LOG_EXCEPTIONS

int vtkHM3Reader::RequestUpdateExtent(vtkInformation*, vtkInformationVector**,
                                      vtkInformationVector*) try {
  log("ParaView called RequestUpdateExtent...");
  log("...RequestUpdateExtent exit: back to ParaView!");
  return 1;
}
HM3_LOG_EXCEPTIONS

void vtkHM3Reader::set_grid_visualization_level(int level) try {
  for (auto&& reader : readers_ | view::values) {
    if (reader->set_visualization_level(level)) { this->Modified(); };
  }
}
HM3_LOG_EXCEPTIONS

void vtkHM3Reader::initialize_readers() try {
  readers_.clear();

  // Create readers
  for (auto&& b : session_) {
    if (is_readable(b)) {
      string name = io::read_field(b, "name", HM3_AT_);
      log("Creating reader \"{}\"...", name);
      readers_[name] = make(b);
      log("... reader \"{}\" created!", name);
    } else {
      log("Block \"{}\" of type \"{}\" not readable.", b["name"].get<string>(),
          b["type"].get<string>());
    }
  }

  log("Number of readers: {}", readers_.size());

  // Initialize readers:
  std::size_t block_idx = 0;
  for (auto&& r : readers_) {
    log("Initializing reader \"{}\" with idx \"{}\"...", r.first, block_idx);
    r.second->initialize(r.first, session_, readers_, block_idx);
    block_idx++;
    log("... reader \"{}\" initialized!", r.first);
  }
  HM3_ASSERT(block_idx == readers_.size(), "");

  // Gather cell arrays from all readers:
  cell_arrays_.clear();
  log("Gathering cell arrays from all readers...");
  for (auto&& r : readers_) {
    auto no_fields = r.second->number_of_fields();
    for (auto field_idx : view::ints(0, no_fields)) {
      auto field_name          = r.second->field(field_idx);
      auto& readers_with_field = cell_arrays_[field_name];
      readers_with_field.push_back(r.first);
    }
  }
  log("... gathering cell arrays from all readers finished!");
  log("The following cell arrays are available");
  for (auto&& cell_array : cell_arrays_) {
    log("  - {}, in:", cell_array.first);
    for (auto&& reader : cell_array.second) { log("    - {}", reader); }
  }
}
HM3_LOG_EXCEPTIONS

void vtkHM3Reader::SetCellArrayStatus(const char* name, int status) try {
  string fname{name};
  log("Set status of cell array \"{}\" to \"{}\"", fname, status);
  bool changed = false;
  for (auto&& reader : cell_arrays_[fname]) {
    bool c  = readers_[reader]->field(fname, status);
    changed = changed || c;
  }
  // if data changed, this is modified
  if (changed) { this->Modified(); }
}
HM3_LOG_EXCEPTIONS

int vtkHM3Reader::GetCellArrayStatus(const char* name) const try {
  string fname{name};
  log("Status of cell array \"{}\" requested:", fname);
  int status = -1;
  for (auto&& reader : cell_arrays_.at(fname)) {
    auto s = readers_.at(reader)->field(fname);
    log("  - status in reader \"{}\": {}", reader, s);
    if (status == -1) {
      status = s;
    } else {
      if (status != s) {
        log.error("  - status value mismatch!");
        HM3_ASSERT(status == s, "status value mismatch between readers");
      }
    }
  }
  return status;
}
HM3_LOG_EXCEPTIONS

int vtkHM3Reader::GetNumberOfCellArrays() const try {
  log("Number of cell arrays \"{}\" requested.", cell_arrays_.size());
  return cell_arrays_.size();
}
HM3_LOG_EXCEPTIONS

const char* vtkHM3Reader::GetCellArrayName(int index) const try {
  log("Name of cell array \"{}\" requested:", index);
  for (auto&& cell_array : cell_arrays_) {
    if (index == 0) {
      log("  - name found: \"{}\"", cell_array.first);
      return cell_array.first.c_str();
    }
    --index;
  }
  log.error("  - name not found!");
  return "";
}
HM3_LOG_EXCEPTIONS
