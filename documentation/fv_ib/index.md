---
layout: default
---

# Finite Volume Immersed Boundary Method

## Main algorithm (the inner loop)

### Preconditions of the inner loop

The following conditions have to be met before entering the inner-loop:

- Precompute unstructured distances
- Reset right-hand side
- Valid initial cell averages

#### Precompute unstructured distances

The distance between a cell and its neighbors is required during the
reconstruction process. Here, we consider cells with two different types of
connectivity: structured and unstructured cells. Most grid cells are structured.

A cell $\mathcal{C}\_i$ is said to be structured if the cell and all its
neighbors are at the same level (i.e. have the same length, size,
..). Otherwise, the cell is said to be unstructured.

The neighbor distances of structured cells is computed with the following relation

<div>
$\mathbf{\Delta x}(\mathcal{C}_i, \mathcal{N}_j)_k = L (k == \mathrm{neighbor_dir}(\mathcal{N}_j))$
</div>

where $L$ is the cell length.

The neighbor distances of unstructured cells are precomputed and stored as
follows (see also Fig. A):
