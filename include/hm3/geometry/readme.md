# Geometry

This module implements:

- `discrete/`: discrete geometry.
- `sd/`: signed-distance fields (implicit geometry).

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

