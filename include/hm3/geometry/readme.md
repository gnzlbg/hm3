# Geometry

This module implements:

- `algorithms/`: bounding boxes, intersect, split, ...
- `primitives/`: points, vectors, polygons, polyhedron, ...
- `data_structures`: point sets, bounding volume hierarchies, meshes, ...
- `sd/`: signed-distance fields (implicit geometry).
- `io/`: input-output of geometries.

## Concepts

There are two fundamental concepts:

- `Dimensional<Nd>`: where `Nd` is the ambient dimension where the geometry lives.
- `Rank<Nd, R>`: where `R` is the `Rank` of a geometry within its ambient
  dimension (e.g. `R == Nd` means that a geometry is a "volume" within their
  ambient dimension).

The main concept is the geometry `Primitive` which is:
  - a `Regular` and `Ranked`  type,
  - with vertices.

The rank of the geometry determines whether it is a refinement of `Primitive`:

  - `PathPrimitive`: rank 1.
  - `SurfacePrimitive`: rank `Nd - 1`
  - `VolumePrimitive`: rank `Nd`
  - `Polygon`: rank 2 and closed (as opposed to a line stripe).


## Primitives

The following primitives are available:

- Point
- Paths
  - Line (unbounded)
  - Ray (unbounded, positive values of the line parameter only)
  - Segment (bounded, line parameter in range [0, 1])
  - Polyline (bounded range of contiguous segments)
- Polygons:
  - Polygon: used to represent cut-cells
    - Triangle
    - Rectangle
      - Square

- Polyhedron:
  - Axis-Aligned Bounding Box (AABB)
    - Box
  - (TODO) Sphere
  - (TODO) Cylinder
  - (TODO) Generic polyhedron

## Algorithms

The following algorithms are available:

- Distance: distance between two primitives:
  - centroid: distance between the centroid of two primitives
  - smalles: smallest distance between the boundaries of two primitives
- Intersects: computes the intersection between two primitives
- Split: splits a rank R primitive with a primitive of rank R-1

## Data-structures

- `point_set<Nd, MaxPPN>`: Stores a unique set of points in an octree.
- `simplex_array<Nd>`: Stores an array of simplices with unique vertices.
- `bvh<Nd>`: Builds a Bounding-Volume-Hierarchy using SAH.
- `simplex_mesh<Nd>`: Stores a mesh of simplices (`simplex_array` + `bvh`).

## Serialization

The following serialization formats are available

- STL: for 3D triangles
- PLY: for general geometry (recommended)

## Signed-distance fields

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
