# Discrete geometry

## Concepts

The main concept is the discrete geometry primitive:

- `Primitive`:
  - vertices: iteration over the primitive vertices.
  - bounding volume: bounding volumes of the primitive (aabb, box, ...).
  - bounding lengths: bounding lengths of the primitive.
  - centroid: centroid of the primitive (center of mass).

Three refinements provide additional operations:

- `VolumePrimitive`: `Nd`-dimensional shape in `Nd`-space.
  - perimeter: integral of the `Nd - 1`-dimensional boundary of the shape.
  - contains: does the primitive fully contain another primitive.
  - surfaces: iterates over the `Nd-1` surfaces of the volume primitive.
  - edges: iterates over the `Nd-2`edges of the volume primitive.
  
- `SurfacePrimitive`: `Nd-1`-dimensional shape in `Nd`-space.
  - perimeter: integral of the `Nd - 2`-dimensional boundary of the shape.
  - area: same as perimeter.
  - normal: `Nd`-dimensional vector normal to the `Nd-1`-dimensional shape.
  - contains: does the primitive fully contain another primitive
  - surfaces: iterates over the `Nd-2` boundaries of the surface primitive
  - edges: same as vertices

- `LinePrimitive`: `Nd-2`-dimensional shape in `Nd`-space
  - direction:
  - length: integral of the `Nd-2`-shape in `Nd`-space.
  - opt: contains / normal
  - surfaces: same as vertices

## Primitives

The following primitives are available:

- Point
- Line Segment
- Polygons:
  - Polygon: used to represent cut-cells
    - Triangle
    - Rectangle
      - Square

- Polyhedra:
  - Axis-Aligned Bounding Box (AABB)
    - Box
  - Sphere: TODO
  - Cylinder: TODO
  - Generic polyhedra: TODO (used to represent cut-cells)

## Data structures

The following data-structures are available:

- Point set: set of unique points (implemented with an octree)
- Surface set: set of unique surfaces (implemented with a BVH)
- Volume set: set of unique volumes (implemented with a BVH)
- Bounding-Volume-Hierarchy
- Simplex mesh: mesh of simplices (points 1D, segments 2D, triangles 3D), used
  to store surface meshes

## Algorithms

The following algorithms are available:

- Distance: distance between two primitives:
  - centroid: distance between the centroid of two primitives
  - smalles: smallest distance between the boundaries of two primitives
- Intersects: computes the intersection between two primitives


## Serialization

The following serialization formats are available

- STL: for 3D triangles
- PLY: for general geometry (recommended)
