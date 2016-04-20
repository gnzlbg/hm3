# (Square) Structured grids

> Note: right now only square structured grids are supported

## Tiles

Two different types of `Tile`s are implemented:

- `tile`: without halo cells
- `halo_tile`: with `Nhl` halo layers.
    
A tile is made of 3 components:

- `cells`: indices for interating over the tile's cells (cells, sub-tiles,
  halos, cell neighbors, ...)
- `surfaces`: indices for iterating over the tile surfaces (over all cells, in a
  single cell, per dimension, in a sub tile, ...)
- `geometry`: tile geometry for computing spatial information (cell/surface
  centroid, length/area/volume, ...) and/or performing spatial queries (is point
  x inside the tile? in which cell?...)

The iteration indices traverse the tile _in a particular order_ (for now linear)
and use the following two abstractions:
- tile `bounds`: number of cells/surfaces per length, in total, ...
- tile `coordinates`: position of a cell/surface within the tile
  (multidimensional indices, linear index, ...)

## Grid

The grid type is just an alias for a type of `Tile`. The `cell` and `surface`
abstractions wrap a `coordinate` and a tile's `geometry` to allow obtaining
cell/surface properties like the cell volume on demand.

**TODO**:

- (Student Task): iteration over surfaces using an order that maximizes temporal
  cache locality.
- iteration over corner points.
