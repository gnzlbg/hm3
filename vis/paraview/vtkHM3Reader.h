#pragma once
/// \file vtkH3TreeReader.h Paraview's VTK Tree Reader
#define HM3_PARAVIEW_PLUGIN
#include <map>
#include <memory>
#include <string>
#include <hm3/vis/vtk.hpp>
#include <hm3/io/session.hpp>
#include <hm3/utility/log.hpp>

class VTK_EXPORT vtkHM3Reader : public vtkMultiBlockDataSetAlgorithm {
 public:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#pragma clang diagnostic ignored "-Wextra-semi"
  vtkTypeMacro(vtkHM3Reader, vtkMultiBlockDataSetAlgorithm);
#pragma clang diagnostic pop

  /// Creates a new reader plugin (called by ParaView)
  static vtkHM3Reader* New();

  /// Sets the file to be read (called by ParaView)
  void SetFileName(const char* fileName);

  /// Can the file be read by the plugin (e.g. checks the file extension)?
  /// (called by ParaView)
  static int CanReadFile(const char* fileName);

  /// Debug output within ParaView (called by ParaView through vtk error macro)
  ///
  /// It dumps the whole state of the reader into paraview and the log file
  virtual void PrintSelf(ostream& os, vtkIndent indent) override;

  /// Generates a dump of the whole state of the reader (useful for
  /// debugging/logging)
  std::string dump_reader() const;

 protected:
  vtkHM3Reader();
  ~vtkHM3Reader();

  /// Returns information about the number of time steps (called by ParaView)
  virtual int RequestInformation(vtkInformation* vtkNotUsed(request),
                                 vtkInformationVector** vtkNotUsed(inputVector),
                                 vtkInformationVector* outputVector) override;

  /// Returns a vtkMultiBlockDataSet (called by ParaView)
  ///
  /// ParaView asks the reader for the data to visualize.
  /// The reader returns a vtkMultiBlockDataSet containing multiple grids,
  /// with different cell and point variables loaded.
  virtual int RequestData(vtkInformation* vtkNotUsed(request),
                          vtkInformationVector** vtkNotUsed(inputVector),
                          vtkInformationVector* outputVector) override;

  /// !!
  virtual int RequestUpdateExtent(vtkInformation*, vtkInformationVector**,
                                  vtkInformationVector*) override;

 public:
  /// Sets the grid visualization level (this is a plugin option)
  void set_grid_visualization_level(int level);

  /// \name Cell data
  /// These functions query the available cell data, and enable/disable which
  /// data is to be visualized (that is, loaded into memory) by the reader
  ///@{

  /// Returns true if the cell data array \p name is to be visualized and false
  /// otherwise
  int GetCellArrayStatus(const char* name) const;

  /// Returns the number of cell data arrays available
  int GetNumberOfCellArrays() const;

  /// Returns the name of the cell data array \p index
  const char* GetCellArrayName(int index) const;

  /// Enables/disables the visualization of the cell data array \p
  /// name (status = true -> enable, status = false -> disable), that is,
  /// the data that will be loaded into memory
  void SetCellArrayStatus(const char* name, int status);

  ///@}  // Cell data

 private:
  vtkHM3Reader(const vtkHM3Reader&) = delete;
  vtkHM3Reader& operator=(const vtkHM3Reader&) = delete;

  /// Initializes all the readers for the curren session file
  void initialize_readers();

  /// Updates a single reader
  void update_reader(std::unique_ptr<hm3::vis::vtk::reader>&, int,
                     vtkMultiBlockDataSet*);

  /// Max number of files
  int max_number_of_files() const;

  /// Current session
  hm3::io::session session_;

  /// Readers of the current session
  hm3::vis::vtk::readers readers_;

  /// Path to the current session file
  std::string file_path_;

  /// Cell array names
  ///
  /// key: array name
  /// value: names of readers with the array
  std::map<std::string, std::vector<std::string>> cell_arrays_;

  /// \name Logging
  ///@{
  hm3::log::serial log{std::string("paraview")};
  ///@}
};
