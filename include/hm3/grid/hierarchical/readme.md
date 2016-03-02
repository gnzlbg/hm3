# HM3's hierarchical grids

- `amr/`: generic amr algorithm that works on trees
- `adaptor/`:
    - `multi`: stores multiple grid indices into a tree-like grid
- `tree/`: hierarchical data-structure
- `cartesian/`:
    - `single`: hierarchical Cartesian grid (tree + spatial information)
    - `multi`: hierarchical Cartesian grid that can store multiple grids
    (essentially a `single` grid wrapped with the `multi` adaptor).

