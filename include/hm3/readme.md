# HM3 - Hierarchical Multi-physics Multi-scale Methods

This is `hm3`'s directory structure in a nutshell:


- `geometry/`: geometry primitive and algorithms.
    - `sd/`: signed-distance functions.
- `grid/`: grid data-structure and algorithms:
    - `hierarchical/`
         - `amr/`: adaptive mesh refinement data-structures and algorithms.
         - `tree/`: generic hierarchical data-structure and algorithms.
         - `cartesian/`: hierarchical Cartesian grids.
    - `structured/`: structured grid data-structure and algorithms.
- `interpolation/`: interpolation data-structures and algorithms.
- `io/`: serialization/deserialization framework.
- `solver/`: numerical methods implementations and utilities:
    - `fv/`: finite-volume methods.
    - `lbm/`: lattice-boltzmann methods.
    - `utility/`: utilities for implementing solvers.
- `utility/`: vocabulary types and other "small" utilities.
    - `config/`: configurable macros.
- `vis/`: visualization framework (for building ParaView plugins).

Namespaces in `hm3` try to follow a structure closely related to this directory's
structure.

**TODO**:

- strip fast location from tree (use only slim, and rename it to location or
  something)
