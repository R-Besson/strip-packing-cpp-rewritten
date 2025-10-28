## Strip Packing C++ Rewritten

### Description
This repository contains an implementation for the strip packing problem that can be found here: https://en.wikipedia.org/wiki/Strip_packing_problem

Our goal is to optimize the placing of rectangles in a strip of fixed width (W) and variable height, such that the overall final height of the strip is minimal.
This is an NP-hard optimization problem.

In this particular implementation, we keep our focus on the empty spaces in the strip or the "holes" instead of keeping track of the rectangles per se. The first hole starts as the whole infinite strip of size (W, ∞). As we place a rectangle at the top left or top right of the hole, we break that hole into new holes, and resolve any overlaps with other holes, making sure that the holes we keep are maximal. We can also allow the rotation of rectangles. We can specify the width of the strip, specify the initial sorting strategy for the rectangles, verbosity, output file, etc...

### Setup (for users wanting to use the graphical interface)
- Linux users follow [Linux SFML Installation Guide](https://www.sfml-dev.org/tutorials/3.0/getting-started/linux/)
- Mac users follow [macOS SFML Installation Guide](https://www.sfml-dev.org/tutorials/3.0/getting-started/macos/)
- Windows users need to setup [MSYS2](https://www.msys2.org/), run `pacman -Ss SFML` and find the appropriate SFML package, and install it using `pacman -S <target_package>`

### Building
```bash
git clone 
cd ./strip-packing-cpp-rewritten/
make
cd build

# ready to pack rectangles, run this to see usage instructions
packer.exe
```

### Input File Format
```
<rectangle 1 width: int> <rectangle 1 height: int>
<rectangle 2 width: int> <rectangle 2 height: int>
...
```

### Testing
- You can make your own test instances using the `generate` executable. Run it to see usage instructions.
- You can run a multitude of tests and generate a comprehensive output file using the `bench` executable.
- You can use famous [instances](./instances_no_rotation/) in the research world of strip-packing. Keep in mind due to the very origin of these files, they are extremely hard to find and track downloads for and consequently many instances aren't correct or the results obtained them don't line up / make sense with results showed in other papers.

## Results
3D graph made using Plotly showing the evolution of the optimality (α = H/OPT(I)) of the algorithm as the number of rectangles (N) → ∞ and the length of the optimal solution compared to its width changes.

<img src="runs/graph.png">