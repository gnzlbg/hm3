# Tiles

> This is very experimental.

> TODO: this only contains an implementation of a "leading direction tile"

## Design

### Overview

A tile contains 2 major components:

- `cell_indices`: global cell traversal.
- `surface_indices`: global surface traversal.
- surfaces are always traversed following the global cell traversal.
  - interleaved-traversal: for each cell all surfaces that haven't been visited
  are visited before moving to the next cell.
  - de-interleaved-traversal: for each normal component `(x,y,z)` all surfaces are
    independently visited. That is, first all surfaces with `n_x` are visited,
    then all with `n_y`, ...

> TODO: extensions would be a `corner_indices` and `edge_indices`, and a
> ad-generalization. None of these have been necessary for implementing
> numerical methods yet. But might be.

### Tile coordinates

A cell/surface coordinate represents the `{ijk}` position of a cell/surface
within the tile, and allows converting the position from/to the global
cell/surface index.

Neighboring cell/surface coordinates can be computed by offseting the coordinates.

Surface coordinates also provide a way to obtaining the surface normal
direction, as well as the coordinates of the adjacent grid cells.

### Tile algorithms

- `for_each_cell(f)`: iterates over all cells in the tile.
- `for_each_cell(from, to, f)`: iterates over a sub-tile spanned by `[from, to]`.
- `for_each_cell_border(from, to, f, w)`: iterates over the border of a sub-tile
   spanned by `[from, to]` of width `w`.
- `for_each_neighbor(x, f)`: iterates over the neighbors of cell `x`.
- `closest_cell(x, p)`: finds the unique closest cell to cell `x` that satisfies
  the unary predicate `p`.

- `for_each_surface(f)`: iterates over all surfaces in the tile.
- `for_each_surface(f, d)`: iterates over all surfaces with normal direction `d`
  in the tile.
- `for_each_surface(from to, f)`: iterates over all surfaces in sub-tile
  `[from, to]`.
- `for_each_surface(from, to, f, d)`: iterates over all surfaces with normal
  direction `d` in the sub tile `[from, to]`.


## Types of tiles

### Leading direction tile

This is the only type of tile implemented, and its the easiest one to implement.
All cells are visited following the i-th leading direction, and then the j-th,
and k-th direction respectively. Its like traversing a ad-tensor in "row-major"
order. Should be fine for "small tiles", that is, where the whole tile fits in
L1 cache.

### TODO: Other types

- Morton tile (using an isotropic z-curve). This "might" be done "fast" in
  platforms with BMI2 and I believe this will improve cache locality for larger
  tiles.

- Hilbert tile (using a isotropic Hilber curve). I don't know how this could be
  implemented efficiently. This gives optimal cache locality for any tile, but it
  will probably require a loo-up table of Hilbert indices.
