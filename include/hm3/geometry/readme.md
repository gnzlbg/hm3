# Geometry

### Geometric primitives

- `aabb<Nd>`: Axis-Aligned Bounding Box.
- `box<Nd>`: Square Axis-Aligned Bounding Box (constant length).
- `dimension/`: Utilities for creating types with a spatial dimension.
- `segment<Nd>`: Line segment (bounded by two end-points).
- `point<Nd>`: Point.
- `polygon<Nd, Np>`: Closed polygon with capacity for `Np` points
- `triangle<Nd>`: Triangle.
- `simplex<Nd>`: Point (1D), Line (2D), Triangle (3D)

### Geometric data-structures
- `point_set<Nd, MaxPPN>`: Stores a unique set of points in an octree.
- `simplex_mesh<Nd>`: Stores a mesh of simplices.
- `bvh<Nd>`: Builds a Bounding-Volume-Hierarchy using SAH.

### Signed-distance fields

- `sd/`: Signed-distance field utilities
  - `combinators/`: utilities for combining multiple signed-distance functions
    - `inverse`/`neg`:
    - `union` / `add`:
    - `intersection`/`common`:
    - `difference`/`sub`:
  - `adaptors/`: utilities for modifying single signed-distance functions
    - `translation`:
    - `rotation`: 
  - `sphere:`
  - `edge:`
  - `square:`  todo: rename to box
  - `rectangle:`  todo: rename to aabb

