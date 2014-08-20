/// \file vtkH3TreeReader.cxx Paraview's VTK Reader
#include <iostream>
#include "vtkH3TreeReader.h"
#include <hom3/grid/type.hpp>
#include <hom3/vis/vtk.hpp>

vtkStandardNewMacro(vtkH3TreeReader);

using namespace hom3;
using namespace vis;
using namespace vtk;

vtkH3TreeReader::vtkH3TreeReader() {
  SetNumberOfInputPorts(0);
  SetNumberOfOutputPorts(1);
}

vtkH3TreeReader::~vtkH3TreeReader() { SetFileName(NULL); }

/// Sets file name and tells reader to update pipeline if file changed
void vtkH3TreeReader::SetFileName(const char* file_name_) {
  std::cout << "file_name: ";
  if (file_name_) {
    std::cout << file_name_;
  } else {
    std::cout << "none";
  }
  std::cout << std::endl;

  std::string fname = file_name_ ? file_name_ : std::string{};

  if (full_file_path != fname && file_name_) {
    full_file_path  = fname;
    std::size_t pos = fname.rfind("/");
    file_path       = fname.substr(0, pos + 1);
    file_name       = fname.substr(pos + 1);
    create_reader();
  }

  this->Modified();
}

/// Print debug information (used by vtk error macro).
void vtkH3TreeReader::PrintSelf(ostream& os, vtkIndent indent) {
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Full file path:            " << full_file_path << endl;
  os << indent << "File name:                 " << file_name << endl;
  os << indent << "File path:                 " << file_path << endl;
}

// Can the file be read by the reader?
int vtkH3TreeReader::CanReadFile(const char* file_name) {
  const auto extension = io::file::extension(file_name);
  return extension == io::session::extension();
}

int vtkH3TreeReader::RequestInformation(
 vtkInformation* vtkNotUsed(request),
 vtkInformationVector** vtkNotUsed(inputVector),
 vtkInformationVector* outputVector) {
  // nothing yet
  double time_steps       = 1;
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), &time_steps, 1);
  return 1;
}

/// Read requested data from file:
int vtkH3TreeReader::RequestData(vtkInformation* /*request*/,
                                 vtkInformationVector** /*inputVector*/,
                                 vtkInformationVector* outputVector) {
  if (!reader) {
    std::cerr << "ERROR: trying to get data from uninitialized grid!"
              << std::endl;
    return 1;
  }

  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkDataSet* output      = vtkDataSet::GetData(outInfo);

  reader->update(output);
  return 1;
}

void vtkH3TreeReader::set_grid_visualization_level(int level) {
  if (!reader) { return; }
  if (reader->set_visualization_level(level)) { this->Modified(); };
}

void create_readers() {
  // read io session file

  // for every block, check out extensions and dependencies
  // look up table of readers, #of out put ports,
  // if grid or dependency on grid, create grid
  // create all other readers
}

void vtkH3TreeReader::create_reader() {
  auto file_descriptor = io::serial::read(full_file_path);

  SInt spatial_dimension;
  file_descriptor.field("spatial_dimension", spatial_dimension);

  switch (grid::which_grid_type(file_descriptor)) {
    case grid::type::hyperoctree: {
      io::serial::read(file_descriptor);
      std::cerr << "hyperoctree grid type" << std::endl;
      reader = rt_make_hyperoctree_reader(spatial_dimension);
      break;
    }
    case grid::type::connectivity: {
      std::cerr << "connectivity grid type" << std::endl;
      break;
    }
    case grid::type::hierarchical_cartesian: {
      SInt no_grids;
      file_descriptor.field("no_grids", no_grids);
      io::serial::read(file_descriptor);
      std::cerr << "hierarchical Cartesian grid type" << std::endl;
      reader
       = rt_make_hierarchical_cartesian_reader(spatial_dimension, no_grids);
      break;
    }
    case grid::type::unknown: {
      std::cerr << "unknown grid type" << std::endl;
      break;
    }
    default: { break; }
  }

  reader->initialize(full_file_path);
}

void vtkH3TreeReader::SetCellArrayStatus(const char* name, int status) {
  if (!reader) { return; }
  std::cerr << "set field: " << name << " to: " << status << std::endl;
  if (reader->field(name, status)) { this->Modified(); };
}
int vtkH3TreeReader::GetCellArrayStatus(const char* name) const {
  if (!reader) { return 0; }
  return reader->field(name);
}
int vtkH3TreeReader::GetNumberOfCellArrays() const {
  if (!reader) { return 0; }
  return reader->number_of_fields();
}
const char* vtkH3TreeReader::GetCellArrayName(int index) const {
  if (!reader) { return ""; }
  return reader->field(index);
};
