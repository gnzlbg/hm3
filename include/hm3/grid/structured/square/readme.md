# Square structured grid utilities

## Tile

A `tile::indices<Nd, Nc>` is a square tensor of rank `Nd` and per-rank-length `Nc` that
is useful to abstract over `Nd`-dimensional square grids with `Nc`-cells per
side.

It provides abstractions over tile coordinates and indices, conversions between
them, iteration over sub-tiles and neighbors of a given cell, as well as some
algorithms like finding the closest cell that satisfies a predicate.

- tile coordinates,
- tile indices,
- iteration over sub-tiles,
- iteration over the neighbors of cells within the tile.

These are useful for building up abstractions over more complex types of grids.

**TODO**:

- a `tile::tile<Nd, Nc>` data-type that stores a tile length and
implements algorithms for computing the coordinates of a given cell as well
as neighbor/sub-tile iteration based on lengths.

- iteration over cell surfaces

For more information see tile's documentation.

## Grid

A `grid::indices<Nd, Nic, Nhl>` is an abstraction over an `Nd`-dimensional
square structured grid with `Nic` internal cells and `Nhl` halo layers.

**THIS IS WIP**

It provides:

- iteration over internal and halo cells,
- iteration over sub-block of halo-cells,
- finding overlapping cells.

**TODO**:

- a `grid::grid<Nd, Nic, Nhl>` type analogous to `tile<Nd, Nc>` that
  knows the grid length and can compute cell coordinates.

- iteration over cell surfaces

For more information see grid's documentation.
