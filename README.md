# 3D graphics @ UCU homework

**Name/Surname**: Roman Pavlosiuk

**API**: OpenGL

**Late days**: 0/7

## How to build

To build, just do

```sh
cmake -B build
cmake --build build
```

## HW1

#### Basic features

- Step 1: Create a project with an empty window
- Step 2: Add an Euler Camera
- Step 3: Handcraft a cube
- Step 4: Apply two different pixel shaders

#### Additional Features

## HW2

#### Fixes from HW1:

- Removed callbacks.h from CMakeLists
- Moved shaders from cmake-build-debug to project
- It's on hw1 branch, pull-request link: https://github.com/gllekkoff/my-first-engine/pull/2
#### Basic features

- Step 1: Use Assimp to create a 3D model loader
- Step 2: Load a model with the loader with its respective textures and render it in your scene ( lpshead )
- Step 3: Instanced rendering
- Step 4: Huge plaform a.k.a floor
- Step 5: Performance tests (10k cubes, also made additionally 100k cubes to see the real difference)

<img src="hw2/dataPlots/1k_comparison.png" alt="Comparison for 1000 cubes" width="800">

<img src="hw2/dataPlots/10k_comparison.png" alt="Comparison for 10000 cubes" width="800">

<img src="hw2/dataPlots/100k_comparison.png" alt="Comparison for 100000 cubes" width="800">

p.s: i have 120hz monitor on laptop\

p.s.s: change **defines** in `main.cpp` as you wish for turning on step2model, performanceLogging or for instanced method or not

We can see the difference in performance even on 100k cubes instance rendering is much more better than non-instanced.
The more cubes the more difference, but on small like 1k or less it's pretty the same, but it's because it almost always 120fps

On 100k cubes we got:
- 3-5 fps non-instanced
- +-60 fps instanced

On 10k cubes:
- +- 60 fps non-instanced 
- 115-120 fps instanced

On 1k cubes:
- Both like 115-120fps

#### Additional Features
(Own) Performance comparison on 100k cubes
## HW3
#### Fixes
Fixed HW2 model not showing because of cool feature of clion to rename similar names, so it renamed "model" into "modelHead".\
Also added 3 mains for different homeworks for fixes, because all homework has different models and purposes.
#### Basic features
Step 1: 3 lightings

Step 2: blinn-phong

Step 3 (i hope): clean code
#### Additional Features
\-
## HW4

#### Basic features
Step 1: added shadow rendering

Step 2: Comparisons
- No PCF, No Comparison sampler
  ![img.png](hw4/images/no_pcf_no_comp.png)
- PCF, No Comparison Sampler
  ![img.png](hw4/images/pcf_and_no_comp.png)
- No PCF, Comparison Sampler
![img.png](hw4/images/no_pcf_and_comp.png)
- PCF, Comparison Sampler
![img.png](hw4/images/pcf_and_comp.png)

Step 3: ImGui

Step 4: Transparency
#### Additional Features

## HW5

#### Fixes
**HW2:**
- Code in headers

**HW3:**
- Fixed HW2 compilation error
- Code in headers

#### Basic features

- Step 1: Implement HDR tonemapping
- Step 2: Implement physically based rendering
- Step 3: Normal mapping
- Step 4: Clean code ( i hope )

#### Additional Features

- Added DirLight, Spotlight, PointsLight settings in ImGui ( hope for additional points, i want 100 for the course :=) )
- Added a menu to imgui to pick the HDR tonemapping algorithm and add 2 more algorithms to pick from, +0.5 pts to each additional one above that. (7 in total)
- Added FXAA (2 pts.), also added it to ImGui

#### TYSM for course, pls 100