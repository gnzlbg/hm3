# VTK Serialization

- FAQ: How to add a new geometry type?

1. Make it support the cell geometry concept.
2. Add it to the supported geometries in geometry.hpp.
3. Map it to its VTK type in vtk.hpp.
