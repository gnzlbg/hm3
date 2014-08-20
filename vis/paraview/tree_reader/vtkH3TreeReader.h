#pragma once
/// \file vtkH3TreeReader.h Paraview's VTK Tree Reader
#include <memory>
#include <string>
#include <vector>
#include <hom3/vis/vtk.hpp>
#include <hom3/io/session.hpp>
#include <map>

class VTK_EXPORT vtkH3TreeReader : public vtkUnstructuredGridAlgorithm {
 public:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#pragma clang diagnostic ignored "-Wextra-semi"
  vtkTypeMacro(vtkH3TreeReader, vtkUnstructuredGridAlgorithm)
#pragma clang diagnostic pop

   static vtkH3TreeReader* New();

  // Methods to be called by ParaView to set the file name
  void SetFileName(const char* fileName);

  // Returns true (1) if the file can be read and false (0) otherwise
  static int CanReadFile(const char* fileName);

  // Debugging output
  virtual void PrintSelf(ostream& os, vtkIndent indent) override;

 protected:
  vtkH3TreeReader();
  ~vtkH3TreeReader();

  virtual int RequestInformation(vtkInformation* vtkNotUsed(request),
                                 vtkInformationVector** vtkNotUsed(inputVector),
                                 vtkInformationVector* outputVector) override;
  virtual int RequestData(vtkInformation* vtkNotUsed(request),
                          vtkInformationVector** vtkNotUsed(inputVector),
                          vtkInformationVector* outputVector) override;

 public:
  void set_grid_visualization_level(int level);
  void SetCellArrayStatus(const char* name, int status);
  int GetCellArrayStatus(const char* name) const;
  int GetNumberOfCellArrays() const;
  const char* GetCellArrayName(int index) const;

 private:
  vtkH3TreeReader(const vtkH3TreeReader&) = delete;
  vtkH3TreeReader& operator=(const vtkH3TreeReader&) = delete;
  void create_readers();
  std::unique_ptr<hom3::vis::vtk::any_reader> grid_reader;

  io::session session;
  std::string full_file_path;
  std::string file_name;
  std::string file_path;
};
