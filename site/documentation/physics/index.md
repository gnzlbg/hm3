---
layout: default
---

# Physical models

## Overview

- [Glossary](#GLOSSARY)
  - [Notation](#GLOSSARY_NOTATION)
  - [Units](#GLOSSARY_UNITS)
  - [Variables](#GLOSSARY_VARIABLES)
- [Physics](#PHYSICS)
  - [Heat Equation](#PHYSICS_HEAT_EQUATION)
     - [Variables](#PHYSICS_HEAT_EQUATION_VARIABLES)
     - [Assumptions](#PHYSICS_HEAT_EQUATION_ASSUMPTIONS)
     - [Equations](#PHYSICS_HEAT_EQUATION_EQUATIONS)
     - [Formulation](#PHYSICS_HEAT_EQUATION_FORMULATION)
     - [Non-dimensionalization](#PHYSICS_HEAT_EQUATION_NON_DIMENSIONALIZATION)
     - [Input variables](#PHYSICS_HEAT_EQUATION_INPUT)
     - [Output variables](#PHYSICS_HEAT_EQUATION_OUTPUT)
     - [Boundary conditions](#PHYSICS_HEAT_EQUATION_BOUNDARY_CONDITIONS)
  - [Euler Equations](#PHYSICS_EULER_EQUATIONS)
     - [Variables](#PHYSICS_EULER_EQUATIONS_VARIABLES)
     - [Assumptions](#PHYSICS_EULER_EQUATIONS_ASSUMPTIONS)
     - [Relationships](#PHYSICS_EULER_EQUATIONS_FORMULATION)
     - [Equations](#PHYSICS_EULER_EQUATIONS_EQUATIONS)
     - [Formulation](#PHYSICS_EULER_EQUATIONS_FORMULATION)
     - [Non-dimensionalization](#PHYSICS_EULER_EQUATIONS_NON_DIMENSIONALIZATION)
     - [Free-stream variables](#PHYSICS_EULER_EQUATIONS_FREE_STREAM_VARIABLES)
     - [Input variables](#PHYSICS_EULER_EQUATIONS_INPUT)
     - [Output variables](#PHYSICS_EULER_EQUATIONS_OUTPUT)
     - [Boundary conditions](#PHYSICS_EULER_EQUATIONS_BOUNDARY_CONDITIONS)
  - [Navier-Stokes Equations (ideal gas)](#PHYSICS_CNS_EQUATIONS)
     - [Variables](#PHYSICS_CNS_EQUATIONS_VARIABLES)
     - [Assumptions](#PHYSICS_CNS_EQUATIONS_ASSUMPTIONS)
     - [Relationships](#PHYSICS_CNS_EQUATIONS_RELATIONSHIPS)
     - [Equations](#PHYSICS_CNS_EQUATIONS_EQUATIONS)
     - [Formulation (TODO)](#PHYSICS_CNS_EQUATIONS_FORMULATION)
     - [Non-dimensionalization](#PHYSICS_CNS_EQUATIONS_NON_DIMENSIONALIZATION)
     - [Free-stream variables](#PHYSICS_CNS_EQUATIONS_FREE_STREAM_VARIABLES)
     - [Input variables](#PHYSICS_CNS_EQUATIONS_INPUT)
     - [Output variables](#PHYSICS_CNS_EQUATIONS_OUTPUT)
     - [Boundary conditions (TODO)](#PHYSICS_CNS_EQUATIONS_BOUNDARY_CONDITIONS)
     - [Surface force](#PHYSICS_CNS_EQUATIONS_SURFACE_FORCE)
  - [Navier-Stokes Equations (slightly compressible)](#PHYSICS_INS_EQUATIONS)
- [Coupling](#COUPLING)
  - [Heat Equation with Navier-Stokes Equations
    (ideal gas)](#COUPLING_HEAT_CNS)

### TODOs:
- add a Boost.Unit conversion table.
- some overlines for dimensioned variables are missing.

### <a id="GLOSSARY"></a>Glossary

#### <a id="GLOSSARY_NOTATION"></a>Notation
- Dimensioned variables are denoted with an overline: $\overline{(\cdot)}$.
- Dimensionless variables don't have an overline $(\cdot)$.
- Tensors/matrices are denoted with an underline: $\underline{(\cdot)}$.
- The identity matrix/tensor is $\underline{\mathbf{I}}$.

- Extends: `B extends A` if the physics model `B` is a superset of the physics
  model `A`.

#### <a id="GLOSSARY_UNITS"></a>Units
- Newton: $[N] = [kg \, m \, \, s^{-2}]$
- Pascal: $[\mathrm{Pa}] = [kg \, m^{-1} \, s^{-2}]$
- Watt: $[W] = [kg \, m^2 \, s^{-3}]$.
- Joule: $[J] = [W \, s]  = [kg  \, m^2 \, s^{-2}]$.

#### <a id="GLOSSARY_VARIABLES"></a>Variables
- Time: $\overline{t} \; [s]$.
- Temperature: $\overline{T} \; [K]$.
- Density: $\overline{\rho} \; [kg \, m^{-3}]$.
- Velocity: $\overline{\mathbf{u}} \; [m \, s^{-1}]$.
- Specific heat capacity per unit mass: $\overline{c}_p \; [J \, kg^{-1} \,
  K^{-1}]$.
- Thermal conductivity: $\overline{\lambda} \; [W \, m^{-1} \, K^{-1})]$.
- Thermal diffusivity: $\overline{\alpha} = \overline{\lambda} /
(\overline{\rho} \overline{c}_p) \; [ m^2 \, s^{-1}]$.
- Specific internal energy (per unit mass): $\overline{e} \; [J \, kg^{-1}]$.
- Internal energy density (per unit volume): $\overline{\rho} \overline{e} \;
  [J \, m^{-3}]$.
- Specific total energy (per unit mass): $\overline{E} \; [J \, kg^{-1}]$.
- Total energy density (per unit volume): $\overline{\rho} \overline{E} \; [J \,
  m^{-3}]$.
- Basis of Cartesian coordinate system: $\mathbf{e}_i \; [-]$.

## <a id="PHYSICS"></a>Physics

### <a id="PHYSICS_HEAT_EQUATION"></a>Heat equation

#### <a id="PHYSICS_HEAT_EQUATION_VARIABLES"></a>Variables
- Source term: $\overline{s} \; [J \, m^{-3} \, s^{-1}]$.
- Heat flux: $\overline{\mathbf{q}} \; [J \, m^{-2} \, s^{-1}]$.

#### <a id="PHYSICS_HEAT_EQUATION_ASSUMPTIONS"></a>Assumptions
- $(\overline{\rho} \overline{c}_p)(\overline{\mathbf{x}}, \overline{t}) =
  const $.
- $ \overline{e}(\overline{T}) = \overline{c}_p \overline{T} \; [J \, kg^{-1}]$.

#### <a id="PHYSICS_HEAT_EQUATION_EQUATIONS"></a>Equations
- Fourier's law of heat conduction: $\overline{\mathbf{q}} = -\overline{\lambda}
\overline{\nabla} \overline{T} \; [J \, m^{-2} \, s^{-1})]$.
- Conservation of energy: $(\overline{\rho} \overline{e})_{\overline{t}} +
\overline{\nabla} \cdot \overline{\mathbf{q}} = \overline{s}$.

#### <a id="PHYSICS_HEAT_EQUATION_FORMULATION"></a>Formulation
Inserting Fourier's law into the conservation of energy:

<div>
$(\overline{\rho} \overline{e})_{\overline{t}} + \overline{\nabla} \cdot
\overline{\mathbf{q}} = (\overline{\rho} \overline{e})_{\overline{t}} -
\overline{\nabla} \cdot \left(\overline{\lambda} \overline{\nabla} \overline{T}
\right) =(\overline{\rho} \overline{e})_{\overline{t}} - \overline{\nabla}
\cdot \left( \frac{\overline{\lambda}}{\overline{\rho} \overline{c}_p}
\overline{\nabla} (\overline{\rho} \overline{e}) \right) = \overline{s}$
</div>

we get a conservation law for the internal energy density:

$(\overline{\rho}
\overline{e})(\overline{\mathbf{x}},\overline{t})_{\overline{t}} -
\overline{\nabla} \cdot ( \overline{\alpha} \overline{\nabla} (\overline{\rho}
\overline{e})(\overline{\mathbf{x}},\overline{t}) ) =
\overline{s}(\overline{\mathbf{x}},\overline{\rho} \overline{e},\overline{t})$

that is equivalent (dividing by the constant $\overline{\rho} \overline{c}_p
$) to a conservation law for the temperature:

$\overline{T}(\overline{\mathbf{x}},\overline{t})_{\overline{t}} -
\overline{\nabla} \cdot (\overline{\alpha} \overline{\nabla}
\overline{T}(\overline{\mathbf{x}},\overline{t})) =
\overline{s}(\overline{\mathbf{x}},\overline{T},\overline{t})$.

The conservation law for the temperature is preferred since the temperature is
usually both the quantity of interest and the quantity that couples the heat
conduction with other physical models.

In short notation, we write it like this: $\overline{T}_{\overline{t}} -
\overline{\nabla} \cdot (\overline{\alpha} \overline{\nabla} \overline{T}) =
\overline{s}$.

####  <a id="PHYSICS_HEAT_EQUATION_NON_DIMENSIONALIZATION"></a>Non-dimensionalization
- Dimensionless coordinates: $\mathbf{x} = \overline{\mathbf{x}} /
  \overline{L}_\mathrm{ref}$.
- Dimensionless time: $t = \overline{t} \overline{\alpha} /
  \overline{L}_\mathrm{ref}^2$.
- Dimensionless temperature: $T = \overline{T} / \overline{T}_\mathrm{ref}$.

We substitute the dimensionless variables in

$\overline{T}_{\overline{t}} - \overline{\nabla} \cdot (\overline{\alpha}
\overline{\nabla} \overline{T} ) = \overline{s}(\overline{T})$

resulting in

<div>
$T_t \frac{\overline{T}_\mathrm{ref}
\overline{\alpha}}{\overline{L}_\mathrm{ref}^2} -
\frac{1}{\overline{L}_\mathrm{ref}} \nabla \cdot (
\frac{\overline{\alpha}}{\overline{L}_\mathrm{ref}} \nabla T)
\overline{T}_\mathrm{ref} = \overline{s}(T)$
</div>

which simplifies to:

<div>
$T_t - \nabla \cdot( \nabla T) =
\frac{\overline{L}_\mathrm{ref}^2}{\overline{\alpha} \overline{T}_\mathrm{ref}}
\overline{s}(T)$.
</div>

In the special case where $s = 0$ it reduces to $T_t - \nabla \cdot (\nabla T) =
0$.

####  <a id="PHYSICS_HEAT_EQUATION_INPUT"></a>Input variables
- Characteristic length (equals one grid distance unit):
  $\overline{L}_\mathrm{ref} = 1 \; [m]$.
- Thermal diffusivity: $\overline{\alpha} \; [m^2 \, s^{-1}]$.
- Reference temperature: $\overline{T}_\mathrm{ref} \; [K]$.

####  <a id="PHYSICS_HEAT_EQUATION_OUTPUT"></a>Output variables
- Temperature scaling factor: $1 / \overline{T}_\mathrm{ref} \; [K^{-1}]$.
- Time scaling factor: $\overline{\alpha} / \overline{L}_\mathrm{ref}^2 \;
  [s^{-1}]$.

####  <a id="PHYSICS_HEAT_EQUATION_BOUNDARY_CONDITIONS"></a>Boundary conditions
- Dirichlet boundary condition (imposed temperature): $T(\mathbf{x}, t)
  \vert\_{\Gamma(t)}$.
- Neumann boundary condition (imposed heat-flux): $\nabla\_n T(\mathbf{x},t)
  \vert\_{\Gamma(t)}$.

### <a id="PHYSICS_EULER_EQUATIONS"></a>Euler equations
#### <a id="PHYSICS_EULER_EQUATIONS_VARIABLES"></a>Variables
- Specific total energy (per unit mass): $\overline{E} = \lVert
  \overline{\mathbf{u}} \rVert^2_2 / 2 + \overline{e} \; [J \, kg^{-1}] $.
- Total energy density (per unit volume): $\overline{\rho} \overline{E} =
  \overline{\rho} \lVert \overline{\mathbf{u}} \rVert^2_2 / 2 + \overline{\rho}
  \overline{e} \; [J \, m^{3}]$.
- Gas constant: $\overline{R} = 8.3144621 \; [J \, mol^{-1} K^{-1}]$.
- Molar mass of gas mixture: $\overline{m} \; [ g \, mol^{-1} ]$.
- Specific gas constant: $\overline{R}_\mathrm{specific} = \overline{R} /
  \overline{m} \; [J \, kg^{-1} K^{-1}]$.
- Ratio of specific heats: $\gamma = \overline{c}\_p / \overline{c}_V \; [-]$.
- Speed of sound: $\overline{a} \; [m \, s^{-1}]$.
- Mach number: $M = \overline{u} / \overline{a} \; [-]$.
- Acceleration: $\mathbf{g} \; [m \, s^{-2}]$.
- Froude number: $\mathbf{Fr} = \overline{a} / \sqrt{\overline{\mathbf{g}}
  \overline{L}} \; [-]$.

#### <a id="PHYSICS_EULER_EQUATIONS_ASSUMPTIONS"></a>Assumptions
- Ideal gas: $\overline{p} = \overline{\rho} \overline{R}_\mathrm{specific}
  \overline{T}$.
- Calorically perfect gas: $\overline{e}(\overline{T}) = \overline{c}_V
  \overline{T} \; [J \, kg^{-1}]$.
- Speed of sound $\overline{a} = \sqrt{\gamma \overline{p} / \overline{\rho}}$.

#### <a id="PHYSICS_EULER_EQUATIONS_RELATIONSHIPS"></a>Relationships
- $\overline{R}\_\mathrm{specific} = \overline{c}\_p - \overline{c}\_V
  = (\gamma- 1) \overline{c}\_V$.
- $\overline{p} = \overline{\rho} \overline{R}\_\mathrm{specific} \overline{T}
  = \overline{\rho} (\gamma - 1) \overline{c}\_V \overline{T} = (\gamma - 1)
  \overline{\rho} \overline{e} = (\gamma - 1) (\overline{\rho} \overline{E} -
  \overline{\rho} \lVert \overline{\mathbf{u}} \rVert^2\_{2} / 2)$.
- $\overline{a} = \sqrt{\gamma \overline{p} / \overline{\rho}} = \sqrt{\gamma
  \overline{R}_\mathrm{specific} \overline{T}}$.


#### <a id="PHYSICS_EULER_EQUATIONS_EQUATIONS"></a>Equations
- Mass conservation: $\overline{\rho}_{\overline{t}} + \overline{\nabla}
\cdot(\overline{\rho} \overline{\mathbf{u}}) = 0$.
- Momentum conservation: $(\overline{\rho}
  \overline{\mathbf{u}})_{\overline{t}} + \overline{\nabla} \cdot
  (\overline{\rho} \overline{\mathbf{u}} \otimes \overline{\mathbf{u}} +
  \overline{p} \underline{\mathbf{I}}) = \overline{\rho} \overline{\mathbf{g}}$.
- Energy conservation: $(\overline{\rho} \overline{E})_{\overline{t}} +
  \overline{\nabla} \cdot ((\overline{\rho} \overline{E} + \overline{p})
  \overline{\mathbf{u}}) = \overline{\rho} (\overline{\mathbf{u}} \cdot
  \overline{\mathbf{g}} )$.

Let:

- Vector of conservative variables: $\overline{\mathbf{Q}} =
  (\overline{\rho}, \overline{\rho} \overline{\mathbf{u}}, \overline{\rho}
  \overline{E})^T \; [ kg \, m^{-3}, Ns\, m^{-3}, J \, m^{-3}]^T$,

- Flux tensor:  $\overline{\underline{\mathbf{H}}}(\overline{\mathbf{Q}}) =
  (\overline{\rho} \overline{\mathbf{u}}, \overline{\rho}
  \overline{\mathbf{u}} \otimes \overline{\mathbf{u}} + \overline{p}
  \underline{\mathbf{I}}, \overline{\rho}
  \overline{\mathbf{u}}(\overline{\rho} \overline{E} + \overline{p}))^T \; [N
  \, s \, m^{-3}, Pa, W m^{-2}]$, and

- Source term vector: $\overline{\mathbf{s}} = (0, \overline{\rho}
  \overline{\mathbf{g}}, \overline{\rho} (\overline{\mathbf{u}} \cdot
  \overline{\mathbf{g}} ))^T \; [ kg \, m^{-3} \, s^{-1} , N \, m^{-3} \,
  s^{1}, J \, m^{-3} \, s^{-1}]$.

Abusing notation, the divergence-form of the Euler-eqts. is: $
\overline{\mathbf{Q}}_{\overline{t}} + \overline{\nabla} \cdot
\overline{\underline{\mathbf{H}}}(\overline{\mathbf{Q}}) = 0 $.

#### <a id="PHYSICS_EULER_EQUATIONS_FORMULATION"></a>Formulation

- In three dimensions, the flux tensor expands to the following (all variables
  are dimensioned):

 $\underline{\mathbf{H}}(\mathbf{Q}) = \begin{bmatrix}
\rho u && \rho v && \rho w \\\
\rho u^2 + p && \rho u v && \rho u w \\\
\rho u v && \rho v^2 + p && \rho vw \\\
\rho u w && \rho vw && \rho w^2 + p \\\
u(\rho E + p) && v (\rho E + p) && w (\rho E + p) \end{bmatrix}^T \quad
\begin{bmatrix}
Ns \, m^{-3} \\\
Pa \\\
Pa \\\
Pa \\\
W \ m^{-2}
\end{bmatrix}$ ,

and the Euler-eqts. expand to:

$\begin{pmatrix} \rho \\\ \rho u \\\ \rho v \\\ \rho w \\\ \rho E
\end{pmatrix}\_t + \begin{pmatrix}
\rho u \\\ \rho u^2 + p \\\ \rho u v \\\ \rho u w \\\ u (\rho E + p)
\end{pmatrix}\_x + \begin{pmatrix}
\rho v \\\ \rho u v \\\ \rho v^2 + p \\\ \rho v w \\\ v (\rho E + p)
\end{pmatrix}\_y + \begin{pmatrix}
\rho w \\\ \rho u w \\\ \rho v w \\\ \rho w^2 + p \\\ w (\rho E + p)
\end{pmatrix}\_z = \begin{pmatrix}
0 \\\ \rho \mathbf{g} \cdot \mathbf{e}\_x \\\ \rho \mathbf{g} \cdot \mathbf{e}\_y
\\\ \rho \mathbf{g} \cdot \mathbf{e}\_z \\\ \rho ( \mathbf{u} \cdot \mathbf{g} )
\end{pmatrix} \; \begin{bmatrix}
kg \, m^{-3} \, s^{-1} \\\
N \, m^{-3} \, s^{-1} \\\
N \, m^{-3} \, s^{-1} \\\
N \, m^{-3} \, s^{-1} \\\
J \, m^{-3} \, s^{-1}] \end{bmatrix}$.

#### <a id="PHYSICS_EULER_EQUATIONS_NON_DIMENSIONALIZATION"></a>Non-dimensionalization
- The stagnation variables are denoted with $(\cdot)_0$.
- The free-stream variables are denoted with $(\cdot)_\infty$.
- Dimensionless coordinates: $\mathbf{x} = \overline{\mathbf{x}} /
  \overline{L}_\mathrm{ref}$.
- Dimensionless time: $t = \overline{t} \overline{a}\_0 /
  \overline{L}\_\mathrm{ref}$.
- Dimensionless density: $\rho = \overline{\rho} / \overline{\rho}_0$.
- Dimensionless velocity: $\mathbf{u} = \overline{\mathbf{u}} / \overline{a}_0$.
- Dimensionless pressure: $p = \overline{p} / (\overline{\rho}\_0
  \overline{a}\_0^2)$.
- Dimensionless specific internal energy (per unit mass): $e = \overline{e} /
  \overline{a}\_0^2$ .
- Dimensionless specific total energy (per unit mass): $E = \overline{E} /
  \overline{a}_0^2$.
- Dimensionless temperature: $T = \overline{T} / \overline{T}_0$.
- Stagnation Froude number: $\mathbf{Fr}\_0 = \overline{a}\_0 /
  \sqrt{\overline{\mathbf{g}} \overline{L}\_\mathrm{ref}}$.
- Dimensionless acceleration: $\mathbf{g} = \overline{\mathbf{g}}
  \overline{L}\_\mathrm{ref} / \overline{a}\_0^2 = 1 / \mathbf{Fr}\_0^2$.


Note:

- Substituting in the ideal gas law gives: $T = \gamma p / \rho \; [-]$.
- The stagnation speed of sound is: $ \overline{a}\_0 = \sqrt{ \gamma
  \overline{R}\_\mathrm{specific} \overline{T}\_0 }$.

##### Mass conservation

<div>
$\frac{\overline{\rho}_0 \overline{a}_0}{\overline{L}_\mathrm{ref}}\rho_{t} +
\frac{\overline{\rho}_0 \overline{a}_0}{\overline{L}_\mathrm{ref}}\nabla
\cdot(\rho \mathbf{u}) = 0$
</div>

$\Leftrightarrow \rho_{t} + \nabla \cdot(\rho \mathbf{u}) = 0$.

##### Momentum conservation

<div>
$ \frac{\overline{\rho}_0 \overline{a}_0^2}{\overline{L}_\mathrm{ref}}(\rho
\mathbf{u})_{t} + \frac{\overline{\rho}_0
\overline{a}_0^2}{\overline{L}_\mathrm{ref}} \nabla \cdot (\rho \mathbf{u}
\otimes \mathbf{u} + p \underline{\mathbf{I}}) = \frac{\overline{\rho}_0
\overline{a}_0^2}{\overline{L}_\mathrm{ref}} \rho \mathbf{g} $
</div>

$ \Leftrightarrow (\rho \mathbf{u})_{t} + \nabla \cdot (\rho \mathbf{u} \otimes
\mathbf{u} + p \underline{\mathbf{I}}) = \rho \mathbf{g} $

##### Energy conservation

<div>
$ \frac{\overline{\rho}_0 \overline{a}_0^3}{\overline{L}_\mathrm{ref}}(\rho
E)_t + \frac{\overline{\rho}_0 \overline{a}_0^3}{\overline{L}_\mathrm{ref}}
\nabla \cdot ((\rho E + p)\mathbf{u}) = \frac{\overline{\rho}_0
\overline{a}_0^3}{\overline{L}_\mathrm{ref}} \rho (\mathbf{u} \cdot \mathbf{g})
$
</div>

$ \Leftrightarrow (\rho E)_t + \nabla \cdot ((\rho E + p)\mathbf{u}) = \rho
(\mathbf{u} \cdot \mathbf{g}) $

##### System

<div>
$ \begin{pmatrix}
\rho \\
\rho \mathbf{u} \\
\rho E \end{pmatrix}_t
 + \nabla \cdot \begin{pmatrix}
\rho \mathbf{u} \\
\rho \mathbf{u} \otimes
\mathbf{u} + p \underline{\mathbf{I}} \\
(\rho E + p)\mathbf{u}
\end{pmatrix} = \begin{pmatrix}
0 \\
\rho \mathbf{g} \\
\rho \mathbf{u} \cdot \mathbf{g}
\end{pmatrix}$
</div>

#### <a id="PHYSICS_EULER_EQUATIONS_FREE_STREAM_VARIABLES"></a>Free-stream variables
- $ M\_\infty = \overline{u}\_\infty / \overline{a}\_\infty $.
- $ T\_\infty = \overline{T}\_\infty / \overline{T}\_0 = ( 1 + (\gamma - 1)
  M\_\infty^2/2 )^{-1} $.
- $ \rho\_\infty = \overline{\rho}\_\infty / \overline{\rho}\_0 = ( T\_\infty )^{
  \frac{1}{ \gamma - 1 } } $.
- $u\_\infty = \overline{u}\_\infty / \overline{a}\_0 = M\_\infty \sqrt{T\_\infty}$.
- $ p\_\infty = \overline{p}\_\infty / (\overline{\rho}\_0 \overline{a}\_0^2) = (
  T\_\infty )^{\frac{\gamma}{\gamma-1}}  / \gamma $.

#### <a id="PHYSICS_EULER_EQUATIONS_INPUT"></a>Input variables

- Characteristic length (equals one grid distance unit):
  $\overline{L}_\mathrm{ref} = 1 \; [m] $.
- Reference temperature: $\overline{T}_0 = 273.15 \; [K]$.
- Reference density: $\overline{\rho}_{0,\mathrm{air}} = 1.225 \; [kg \, m^{-3}]$.
- Specific gas constant: $\overline{R}_\mathrm{specific,air} = 287.058 \; [J \,
  kg^{-1} \, K^{-1}] $.
- Ratio of specific heats: $ \gamma_\mathrm{air} = 1.4 \; [-] $.
- Free-stream Mach's number: $M\_\infty \; [-]$.
- Stagnation Froude number: $\mathbf{Fr}\_0 [-]$.
  - if $\overline{g} = 0$, then $\mathbf{Fr}\_0 = \mathbf{\infty} \; [-]$.
  - if air and $\overline{g} = 9.81 [m \, s^{-2}]$, then $\mathbf{Fr}\_0 = 105.78 \; [-] $.


#### <a id="PHYSICS_EULER_EQUATIONS_OUTPUT"></a>Output variables
- Time scaling factor: $\overline{a}\_0 / \overline{L}\_\mathrm{ref} \; [ s^{-1} ]$.
- Temperature scaling factor: $1 / \overline{T}_\mathrm{0} \; [K^{-1}]$.

#### <a id="PHYSICS_EULER_EQUATIONS_BOUNDARY_CONDITIONS"></a>Boundary conditions

### <a id="PHYSICS_CNS_EQUATIONS"></a>Navier-Stokes equations (ideal gas)
- Extends:  Euler equations.

#### <a id="PHYSICS_CNS_EQUATIONS_VARIABLES"></a>Variables
- Dynamic Viscosity: $ \overline{\mu} \; [kg \, m^{-1} \, s^{-1}] $.
- Shear-stress tensor: $ \overline{\underline{\mathbf{\tau}}} \; [Pa] $.
- Stress tensor: $ \overline{\underline{\mathbf{\sigma}}} \; [Pa] $.
- Heat flux: $ \overline{\mathbf{q}} \; [J \, m^{-2} \, s^{-1}) ] $.
- Sutherland's constant: $ \overline{S} \; [K] $.
- Reynolds number: $ \mathrm{Re} = \overline{\rho} \overline{u} \overline{L} /
  \overline{\mu} \; [-] $.
- Prandtl number: $ \mathrm{Pr} = \overline{c}_p \overline{\mu} /
  \overline{\lambda} \; [-] $.

#### <a id="PHYSICS_CNS_EQUATIONS_ASSUMPTIONS"></a>Assumptions
- Newtonian fluid with zero bulk viscosity (monatomic gas): $
  \overline{\underline{\mathbf{\tau}}} = -(\overline{\mu} (\overline{\nabla}
  \overline{\mathbf{u}} + (\overline{\nabla} \overline{\mathbf{u}})^T) - 2/3
  \overline{\mu} ( \overline{\nabla} \cdot \overline{\mathbf{u}})
  \underline{\mathbf{I}}) \; [Pa] $.
- Constant Prandtl number.
- Constant ratio of specific heats.
- Constant specific heat capacities.
- Gas at [moderate
  temperature](http://en.wikipedia.org/wiki/Viscosity#Effect_of_temperature_on_the_viscosity_of_a_gas):
  $ 0K < \overline{T} < 555K $.

#### <a id="PHYSICS_CNS_EQUATIONS_RELATIONSHIPS"></a>Relationships
- Sutherland's law: $\overline{\mu} = \overline{\mu}\_0 \left
  (\frac{\overline{T}}{\overline{T}\_0} \right )^{3/2} \frac{\overline{T\_0} +
  \overline{S}}{\overline{T} + \overline{S}}$
- Stress-tensor: $ \overline{\underline{\mathbf{\sigma}}} =
  -\overline{\underline{\mathbf{\tau}}} - \overline{p} \underline{\mathbf{I}} $
- Fourier's law of heat conduction: $ \overline{\mathbf{q}} =
  -\overline{\lambda} \overline{\nabla} \overline{T} \; [J \; / (m^2 \; s) ] $

#### <a id="PHYSICS_CNS_EQUATIONS_EQUATIONS"></a>Equations
- Momentum conservation: $ (\overline{\rho}
  \overline{\mathbf{u}})_{\overline{t}} + \overline{\nabla} \cdot
  (\overline{\rho} \overline{\mathbf{u}} \otimes \overline{\mathbf{u}} +
  \overline{p} \underline{\mathbf{I}}) + \overline{\nabla} \cdot
  \overline{\underline{\mathbf{\tau}}} = \overline{\rho}
  \overline{\mathbf{g}}$.
- Energy conservation: $ (\overline{\rho} \overline{E})_{\overline{t}} +
  \overline{\nabla} \cdot ((\overline{\rho} \overline{E} + \overline{p})
  \overline{\mathbf{u}}) + \overline{\nabla} \cdot
  (\overline{\underline{\mathbf{\tau}}}\overline{\mathbf{u}}) +
  \overline{\nabla} \cdot \overline{\mathbf{q}} = \overline{\rho}
  (\overline{\mathbf{u}} \cdot \overline{\mathbf{g}} ) $.

Let the viscous flux tensor be defined as: $
\overline{\underline{\mathbf{H}}}_\mathrm{viscous} = (0,
\overline{\underline{\mathbf{\tau}}},
\overline{\underline{\mathbf{\tau}}}\overline{\mathbf{u}} +
\overline{\mathbf{q}})^T $.

Abusing notation, the divergence-form of the Navier-Stokes equations is: $
\overline{\mathbf{Q}}\_{\overline{t}} + \overline{\nabla} \cdot
\overline{\underline{\mathbf{H}}}\_\mathrm{Euler}(\overline{\mathbf{Q}}) +
\overline{\nabla} \cdot
\overline{\underline{\mathbf{H}}}\_\mathrm{viscous}(\overline{\mathbf{Q}}) =
\overline{\mathbf{s}} $.

#### <a id="PHYSICS_CNS_EQUATIONS_FORMULATION"></a>Formulation
- In three dimensions, the viscous flux tensor expands to: `TODO`.

#### <a id="PHYSICS_CNS_EQUATIONS_NON_DIMENSIONALIZATION"></a>Non-dimensionalization
- Dimensionless dynamic viscosity: $\mu = \overline{\mu} / \overline{\mu}\_0$
- Stagnation Reynolds number: $ \mathrm{Re}\_0 = \overline{a}\_0
  \overline{L}\_\mathrm{ref} \overline{\rho}\_0 / \overline{\mu}\_0 $
- Dimensionless shear-stress tensor: $ \underline{\mathbf{\tau}} =
  \overline{\underline{\mathbf{\tau}}} \overline{L}\_\mathrm{ref} /
  (\overline{\mu}\_0 \overline{a}\_0) $.
- Dimensionless stress tensor: $ \underline{\mathbf{\sigma}} = -
  \underline{\mathbf{\tau}} - p \underline{\mathbf{I}} = -
  \overline{\underline{\mathbf{\tau}}} \overline{L}\_\mathrm{ref} /
  (\overline{\mu}\_0 \overline{a}\_0) - \overline{p} / (\overline{\rho}\_0
  \overline{a}\_0^2) \underline{\mathbf{I}} $.
- Dimensionless heat flux: $ \mathbf{q} = \overline{\mathbf{q}}
  \overline{L}\_\mathrm{ref} / (\overline{a}\_0^2 \overline{\mu}\_0) $.

Note:
- The stagnation dynamic viscosity: $ \overline{\mu}\_0 = \frac{ \overline{a}\_0
\overline{L}\_\mathrm{ref} \overline{\rho}\_0 } { \mathrm{Re}\_0 } $.


##### Dimensionless heat flux:

To get the dimensionless heat flux:

<div>
$ \mathbf{q} = \frac{\overline{L}_\mathrm{ref}}{\overline{a}_0^2
\overline{\mu}_0} \overline{\mathbf{q}} $
</div>

we first insert Fourier's law of heat conduction $ \overline{\mathbf{q}} = -
\overline{\lambda} \overline{\nabla} \overline{T} $ and the dimensionless
variables $ T $ and $ \lambda $:

<div>
$ \mathbf{q} = \frac{\overline{L}_\mathrm{ref}}{\overline{a}_0^2
\overline{\mu}_0} (- \overline{\lambda} \overline{\nabla} \overline{T}) =
\frac{\overline{L}_\mathrm{ref}}{\overline{a}_0^2 \overline{\mu}_0} \frac{
\overline{\lambda}_0 \overline{T}_0}{\overline{L}_\mathrm{ref}} (- \lambda
\nabla T) $
</div>

The reference length $\overline{L}\_\mathrm{ref}$ vanishes and we extend with
$\overline{c}\_{p,0}$ as follows to substitute the stagnation Prandtl number
$\mathrm{Pr}\_0$:

<div>
$\mathbf{q} = \frac{ \overline{\lambda}_0 \overline{T}_0 }{\overline{a}_0^2
\overline{\mu}_0 } (- \lambda \nabla T) \frac{ \overline{c}_{p,0} }{
\overline{c}_{p,0} } = \frac{\overline{T}_0 \overline{c}_{p,0}}{\overline{a}_0^2
} \frac{\overline{\lambda}_0}{\overline{\mu}_0 \overline{c}_{p,0}} (- \lambda
\nabla T) = \frac{\overline{T}_0 \overline{c}_{p,0}}{\overline{a}_0^2 }
\frac{1}{\mathrm{Pr}_0} (- \lambda \nabla T)$.
</div>

Now we use the definition of the speed of sound $\overline{a}\_0 = \sqrt{\gamma
\overline{R}\_\mathrm{specific} \overline{T}\_0}$ as well as the definitions of
the specific gas constant $\overline{R}\_\mathrm{specific} = (\gamma -
1)\overline{c}\_V$ and the ratio of specific heats $\gamma = \overline{c}\_p /
\overline{c}\_V$:

<div>
$ \mathbf{q} = \frac{\overline{T}_0 \overline{c}_{p,0}}{\overline{a}_0^2 }
\left (- \frac{\lambda \nabla T}{\mathrm{Pr}_0} \right) = \frac{\overline{T}_0
\overline{c}_{p,0}}{\gamma \overline{R}_\mathrm{specific} \overline{T}_0 } \left
(- \frac{\lambda \nabla T}{\mathrm{Pr}_0} \right) =
\frac{\overline{c}_{p,0}}{\gamma \overline{R}_\mathrm{specific}} \left (-
\frac{\lambda \nabla T}{\mathrm{Pr}_0} \right) =
\frac{\overline{c}_{p,0}}{\gamma (\gamma - 1) \overline{c}_{V} } \left (-
\frac{\lambda \nabla T}{\mathrm{Pr}_0} \right ) = \frac{\gamma}{\gamma (\gamma -
1)} \left (- \frac{\lambda \nabla T}{\mathrm{Pr}_0} \right ) = - \frac{\lambda
\nabla T}{\mathrm{Pr}_0 (\gamma - 1)} $.
</div>

Note that since $c\_{p}$ and $c\_V$ are temperature
independent (i.e. constants) $\gamma = c\_{p,0} / c_V$.

The dimensionless heat conductivity and dynamic viscosity are related through
the Prandtl number:

<div>
$ \lambda = \frac{ \overline{\lambda} }{ \overline{\lambda}_0 } = \frac{
\overline{\lambda} }{ \overline{\lambda}_0 } \frac{ \overline{\mu}_0
\overline{c}_p }{ \overline{\mu}_0 \overline{c}_p}
\frac{\overline{\mu}}{\overline{\mu}} = \frac{\overline{\mu}}{\overline{\mu}_0}
\frac{\mathrm{Pr}_0}{\mathrm{Pr}} = \mu \frac{\mathrm{Pr}_0}{\mathrm{Pr}} $,
</div>

and since we assume that the Prandtl number is constant (which is a fair
approximation), then $\lambda(T) = \mu(T)$.

Using Sutherland's law in dimensionless form we can compute the dimensionless
dynamic viscosity as follows:

<div>
$ \mu = \frac{\overline{\mu}}{\overline{\mu}_0} = T^{3/2} \frac{\overline{T_0}
+ \overline{S}}{\overline{T} + \overline{S}} = T^{3/2} \frac{1 +
\overline{S}/\overline{T}_0}{T + \overline{S}/\overline{T}_0} $.
</div>

##### Dimensionless shear-stress tensor

<div>
$ \frac{\overline{\mu}_0 \overline{a}_0}{\overline{L}_\mathrm{ref}}
\underline{\mathbf{\tau}} = - \frac{\overline{\mu}_0
\overline{a}_0}{\overline{L}_\mathrm{ref}} (\mu (\nabla \mathbf{u} + (\nabla
\mathbf{u})^T) - 2/3 \mu ( \nabla \cdot \mathbf{u}) \underline{\mathbf{I}}) $

$ \Leftrightarrow \underline{\mathbf{\tau}} = - (\mu (\nabla \mathbf{u} +
(\nabla \mathbf{u})^T) - 2/3 \mu ( \nabla \cdot \mathbf{u})
\underline{\mathbf{I}}) $
</div>

##### Momentum conservation

<div>
    $ \frac{\overline{\rho}_0 \overline{a}_0^2}{\overline{L}_\mathrm{ref}}(\rho
    \mathbf{u})_t + \frac{\overline{\rho}_0
    \overline{a}_0^2}{\overline{L}_\mathrm{ref}} \nabla \cdot (\rho \mathbf{u}
    \otimes \mathbf{u} + p \underline{\mathbf{I}}) + \frac{\overline{\mu}_0
    \overline{a}_0}{\overline{L}_\mathrm{ref}^2} \nabla \cdot
    \underline{\mathbf{\tau}} = \frac{\overline{\rho}_0
    \overline{a}_0^2}{\overline{L}_\mathrm{ref}} \rho \mathbf{g} $

    $ \Leftrightarrow (\rho \mathbf{u})_t + \nabla \cdot (\rho \mathbf{u}
    \otimes \mathbf{u} + p \underline{\mathbf{I}}) + \frac{1}{\mathrm{Re}_0}
    \nabla \cdot \underline{\mathbf{\tau}} = \rho \mathbf{g} $
</div>

##### Energy conservation

<div>
$ \frac{\overline{\rho}_0 \overline{a}_0^3}{\overline{L}_\mathrm{ref}}(\rho
E)_t + \frac{\overline{\rho}_0 \overline{a}_0^3}{\overline{L}_\mathrm{ref}}
\nabla \cdot ((\rho E + p)\mathbf{u}) + \frac{\overline{\mu}_0
\overline{a}_0^2}{\overline{L}_\mathrm{ref}^2} \nabla \cdot
(\underline{\mathbf{\tau}} \mathbf{u}) + \frac{\overline{\mu}_0
\overline{a}_0^2}{\overline{L}_\mathrm{ref}^2} \nabla \cdot \mathbf{q} =
\frac{\overline{\rho}_0 \overline{a}_0^3}{\overline{L}_\mathrm{ref}} \rho
(\mathbf{u} \cdot \mathbf{g}) $

$ \Leftrightarrow (\rho E)_t + \nabla \cdot ((\rho E + p)\mathbf{u}) +
\frac{1}{\mathrm{Re}_0} \left( \nabla \cdot (\underline{\mathbf{\tau}}
\mathbf{u}) + \nabla \cdot \mathbf{q} \right )= \rho (\mathbf{u} \cdot
\mathbf{g}) $
</div>

##### System

<div>
$ \begin{pmatrix}
\rho \\
\rho \mathbf{u} \\
\rho E \end{pmatrix}_t
 + \nabla \cdot \begin{pmatrix}
\rho \mathbf{u} \\
\rho \mathbf{u} \otimes
\mathbf{u} + p \underline{\mathbf{I}} \\
(\rho E + p)\mathbf{u}
\end{pmatrix} + \frac{1}{\mathrm{Re}_0} \nabla \cdot \begin{pmatrix}
0 \\
\underline{\mathbf{\tau}} \\
\underline{\mathbf{\tau}} \mathbf{u} + \mathbf{q}
\end{pmatrix}
= \begin{pmatrix}
0 \\
\rho \mathbf{g} \\
\rho \mathbf{u} \cdot \mathbf{g}
\end{pmatrix}$
</div>

##### Divergence form

<div>
$ \mathbf{Q}_t + \nabla \cdot \underline{\mathbf{H}}_\mathrm{Euler}(\mathbf{Q})
+ \frac{1}{\mathrm{Re}_0} \nabla \cdot
\underline{\mathbf{H}}_\mathrm{viscous}(\mathbf{Q}) = 0 $.
</div>

#### <a id="PHYSICS_CNS_EQUATIONS_FREE_STREAM_VARIABLES"></a>Free-stream variables
- $ \mu\_\infty = \overline{\mu}\_\infty / \overline{\mu}\_0 = (T\_\infty)^{3/2}
\frac{1 + \overline{S} / \overline{T}\_0}{T\_\infty + \overline{S} /
\overline{T\_0}} $

- $ Re\_\infty = \frac{ \overline{\rho}\_\infty \overline{u}\_\infty
  \overline{L}\_\mathrm{ref}}{ \overline{\mu}\_\infty} = \frac{\overline{\rho}\_0
  \overline{a}\_0 \overline{L}\_\mathrm{ref}}{\overline{\mu}\_0} \frac{\rho\_\infty
  u\_\infty}{\mu\_\infty} = \mathrm{Re}\_0 \frac{\rho\_\infty u\_\infty}{\mu\_\infty}
  $
$ \Leftrightarrow \mathrm{Re}\_0 = \frac{ \mathrm{Re}\_\infty
  \mu\_\infty }{ \rho\_\infty u\_\infty } $.

#### <a id="PHYSICS_CNS_EQUATIONS_INPUT"></a>Input variables
- Sutherland's constant: $ \overline{S} \; [K] $, $
\overline{S}\_\mathrm{air,0} = 110.4 K $.
- Free-stream Reynolds number: $ \mathrm{Re}\_\infty \; [-] $.
- Stagnation Prandtl number: $ \mathrm{Pr}\_0 \; [-] $, $
  \mathrm{Pr}\_{\mathrm{air},0} = 0.71 \; [-] $.


#### <a id="PHYSICS_CNS_EQUATIONS_OUTPUT"></a>Output variables
- Time scaling factor: $ \overline{a}\_0 / \overline{L}\_\mathrm{ref} \; [ s^{-1} ]
$.
- Temperature scaling factor: $ 1 / \overline{T}_\mathrm{0} \; [K^{-1}] $.

#### <a id="PHYSICS_CNS_EQUATIONS_BOUNDARY_CONDITIONS"></a>Boundary conditions: TODO

#### <a id="PHYSICS_CNS_EQUATIONS_SURFACE_FORCE"></a> Surface Force

The dimensioned force normal to a surface $\Gamma$ is:

$\overline{\mathbf{F}} = \int_\Gamma - \overline{p} \cdot \mathbf{n} +
\overline{\underline{\mathbf{\tau}}} \cdot \mathbf{n} \; d \overline{A}$

which in dimensionless form is:

$\mathbf{F} = \int_\Gamma - p \cdot \mathbf{n} +
\frac{1}{\mathrm{Re}_0}\underline{\mathbf{\tau}} \cdot \mathbf{n} \; d A$


where the dimensionless force $\mathbf{F} =
\frac{\overline{\mathbf{F}}}{\overline{\rho}_0 \overline{a}_0^2
\overline{A}_\mathrm{ref}}$.

## <a id="COUPLING"></a>Coupling

### <a id="COUPLING_HEAT_CNS"></a>Heat Conduction with Navier-Stokes (ideal gas)

#### Thermal diffusivity ratio

- Thermal diffusivity ratio: $\mathrm{Tr} =
  \overline{\alpha}\_{\mathrm{fluid},0} / \overline{\alpha}\_\mathrm{solid} \;
  [-]$.

#### Time scaling
- Dimensionless acoustic time (fluid): $ t\_\mathrm{fluid} = \overline{t} /
(\overline{a}\_0 / \overline{L}\_\mathrm{ref,fluid}) \; [-] $.
- Dimensionless diffusive time (solid): $ t\_\mathrm{solid} = \overline{t} / (
  \overline{L}\_\mathrm{ref,solid}^2 / \overline{\alpha}\_\mathrm{solid} ) \; [-]
  $.

The ratio between the acoustic and the diffusive time is:

<div>
$ \frac{ t_\mathrm{solid} }{ t_\mathrm{fluid} } = \frac{
\overline{L}_\mathrm{ref,solid}^2 }{ \overline{\alpha}_\mathrm{solid} } \frac{
\overline{a}_{\mathrm{fluid},0} }{ \overline{L}_\mathrm{ref,fluid} } $
</div>

Substituting

<div>
$\mathrm{Re}_0 = \overline{a}_{\mathrm{fluid},0}
\overline{\rho}_{\mathrm{fluid},0} \overline{L}_\mathrm{ref,fluid} /
\overline{\mu}_0 $ and $ \mathrm{Pr}_0 = \overline{\mu}_0
\overline{c}_{p,\mathrm{fluid}, 0} / \overline{\lambda}_{\mathrm{fluid}, 0} $
</div>

for the stagnation speed of sound:

<div>
$ \frac{ t_\mathrm{solid} }{ t_\mathrm{fluid} } = \frac{
\overline{L}_\mathrm{ref,solid}^2 }{ \overline{\alpha}_\mathrm{solid} } \frac{
\overline{\lambda}_{\mathrm{fluid}, 0} }{ \overline{\rho}_{\mathrm{fluid},0}
\overline{c}_{p,\mathrm{fluid}, 0}} \frac{ \mathrm{Re}_0 \mathrm{Pr}_0}{
\overline{L}_\mathrm{ref,fluid}^2 } $
</div>

and using the fluid's stagnation thermal diffusivity:

<div>
$ \frac{ t_\mathrm{solid} }{ t_\mathrm{fluid} } = \frac{
\overline{\alpha}_{\mathrm{fluid}, 0} }{ \overline{\alpha}_\mathrm{solid} }
\frac{ \overline{L}_\mathrm{ref,solid}^2 }{ \overline{L}_\mathrm{ref,fluid}^2 }
\mathrm{Re}_0 \mathrm{Pr}_0 = \frac{ \overline{L}_\mathrm{ref,solid}^2 }{
\overline{L}_\mathrm{ref,fluid}^2 } \mathrm{Tr} \mathrm{Re}_0 \mathrm{Pr}_0$ .
</div>
