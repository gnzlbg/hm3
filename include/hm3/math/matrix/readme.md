# Matrix utilities

> TODO: TL;DR: is complicated

The matrix story is complicated:

- want full control over layout, Eigen3 types give you no control
- want to reuse all of Eigen3 functionality
- construction is complicated
- initializer lists are complicated
- constexpr is complicated
- handling vectors, matrix, and bits is complicated
- easy switch between row and col major orders

In a nutshell:

- Bounds is an interface to matrix bounds (`size`, `nrows`, `ncols`)
- Storage abstracts the matrix storage (stack allocated if possible, otherwise
  heap allocated, bit-storage...)
- Eigen3 includes all of Eigen
- Access is just the comptuation of the 1D indices from multi-dimensional indices
- Matrix (does way to much):
  - binds `Storage`, `Access`, and `Bounds` together
  - provides "type-safe" indexing
  - provides constructors
  - maps storage to `Eigen::Map` types (allows reusing all of Eigen)
  - allows viewing matrices as ranges

Not happy about how complicated this is, I wish it would be easier.

Some ideas:

- move the type-safe indexing into its own template, so that one can adapt a
  matrix with it (should make it easier)
- move the Eigen3 wrapping into an `as_matrix(m)` function
- implement `as_matrix` for common storage schemes
- make the user choose a storage scheme, allow then wrapping the storage into
  type safe indexing
- split storage of vectors, matrices, bit vectors and bit matrices into distinc
  types
