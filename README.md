# Strip Packing

C++ solver for the NP-hard [strip packing problem](https://en.wikipedia.org/wiki/Strip_packing_problem): fit `N` rectangles into a strip of fixed width `W` without overlap, using as little height as possible.

We keep track of empty space as we place the rectangles instead of tracking the rectangles themselves. The strip starts as a single *"hole"* of size `(W, ∞)`. Placing a rectangle splits whatever hole it landed in into several new holes, and overlaps with neighbouring holes are then fixed.

## Example (visualizer.cpp)

<img src="src/example.png">

## Build

```bash
git clone https://github.com/R-Besson/strip-packing-cpp-rewritten.git
cd strip-packing-cpp-rewritten

make            # optimized (default)
# make debug
# make release  # static

cd build
# run any executable with no arguments to see its usage
```

For the SFML visualizer: [Linux](https://www.sfml-dev.org/tutorials/3.0/getting-started/linux/), [macOS](https://www.sfml-dev.org/tutorials/3.0/getting-started/macos/). Windows users install [MSYS2](https://www.msys2.org/), then `pacman -Ss SFML` and `pacman -S <package>` in the MSYS2 terminal.

## Input File Format

```
<rect 1 width> <rect 1 height>
<rect 2 width> <rect 2 height>
...
```

## A note on "OPT" and lower bounds

The solver reports the height lower bound

```
LB(I) = max( ⌈total area / W⌉ , h_max )
```

which is printed as `opt_h` / `OPT(I)` in the program output. **For the generated instances below, `LB(I) = OPT(I)` exactly** (see next section), so `H / LB` is the true approximation ratio. For the standard benchmark sets, `LB` is only a lower bound on the optimum, so `H / LB` *overestimates* the true ratio.

## Instances & Benchmarks

`generate` builds an instance `I` whose optimum is known. Given a width `W` and a height ratio `r = H/W`, it takes the rectangle `(W, W·r)` and recursively splits it with guillotine cuts until there are `N` pieces. Nothing is lost when splitting, so the areas sum to `W·H`, the area lower bound `⌈area/W⌉` equals `H`, and therefore `OPT(I) = H`.

These are **zero-waste, guillotine-generated instances**: a perfect packing always exists. This is what makes the optimum known, but it is also a restricted class, and results on it should not be read as results on arbitrary instances.

You can also use the standard sets from [2DPackLib](https://site.unibo.it/operations-research/en/research/2dpacklib) or [OR-Datasets](https://github.com/Oscar-Oliveira/OR-Datasets) (see [Standard benchmarks](#standard-benchmarks)).

`bench` runs repeated random instance generations and solves, and saves the results to a .csv file.

## Results on generated instances

`α = H / OPT(I)` measures how close the solver's height is to optimal. We use `N` from 5 to 2000 and `r = OPT/W` from 0.1 to 10, with 2000 random instances per `(N, r)` configuration (500 for `N = 2000, r = 10`). The raw data is in [./runs/](./runs/), with per-configuration averages in [summary_results.csv](./runs/summary_results.csv) and worst cases in [summary_results_worst.csv](./runs/summary_results_worst.csv).

### Worst case

<img src="./runs/graph_worst.png">

The worst observed ratio is **α ≈ 1.66** (`N = 10`, `r = 10`). Every configuration with `N = 10` has a worst case above 1.5. The worst case falls as `N` grows: for `N = 2000` it is at most 1.0032.

For context: no polynomial-time algorithm can guarantee an absolute ratio below 3/2 unless P = NP (by reduction from PARTITION), and the best proven guarantee is 5/3 + ε (Harren, Jansen, Prädel & van Stee). Those are worst-case statements over *all* instances, so they cannot be compared directly with the worst case observed on random instances here.

### Average case

<img src="./runs/graph_avg.png">

The average `α` stays below 1.09 in every configuration, and below 1.03 once `N ≥ 200`. The largest averages occur at small `N` (peaking around `N = 50`). With a few hundred rectangles or more, the average packing is within a few percent of optimal for every `r`.

### Convergence

<img src="./runs/graph_convergence.png">

With `N` on a log axis and one curve per `r`, the average excess `α − 1` is about 6–7% at `N = 10`, *rises* to about 8–9% at `N = 50`, and then decreases, reaching between 10⁻³ and 10⁻⁴ (or exactly 0) at `N = 2000`.

At the largest `N` the decay is steeper than `1/N`. Between `N = 500` and `N = 1000`, the local log–log slope ranges from about −1.1 (`r = 10`) to about −3.4 (`r = 0.5`). Between `N = 1000` and `N = 2000`, it is between about −2 and −4 for the curves that are still nonzero. This is an observation, not an established rate: each curve has only two or three points in that range, and several curves reach `α = 1` exactly, so they drop off the log plot. A proper fit with confidence intervals is future work.

One interpretation: at fixed geometry, raising `N` makes the pieces smaller relative to the strip, so the excess height should be governed by the tallest rectangle. This is consistent with the form of Kenyon and Rémila's AFPTAS bound, `(1+ε)·OPT + O(1/ε²)·h_max`, whose additive term shrinks as `h_max / OPT` shrinks.

The curves are also ordered by `r` across most of the range: taller strips give a slightly larger `α − 1`, and the gap widens with `N`.

## Standard benchmarks

Results on standard instances without rotations, from [./instances_no_rotation/](./instances_no_rotation/). Ratios are `H / LB`, which **overestimates** the true ratio `H / OPT` (see above). On the `gcut` instances in particular, the area bound is often well below the true optimum.

| Set | Instances | Mean `H / LB` | Worst `H / LB` |
| --- | --- | --- | --- |
| beng (Bengtsson) | beng1–10 | 1.030 | 1.100 (beng1) |
| Berkey & Wang | 46–50 | 1.038 | 1.051 (48) |
| gcut (Beasley) | gcut2–5, 9–13 | 1.165 | 1.271 (gcut5) |

The result files for gcut1, gcut6 and gcut8 were produced with rotations enabled and are left out of this table. The hopper, hto and oliveira sets have not been run yet.

## Complexity

`O(N·M²)` time and `O(M)` space, where `M` is the number of holes held. Across the benchmark runs, `M` grows linearly in `N` (empirically, not proven), which gives **`O(N³)` time and `O(N)` space**.

The initial sort of the rectangles is `O(N log N)`. It is followed by `N` iterations consisting of:

| Step | Cost |
| --- | --- |
| `getBestHole` | `O(M)` |
| `fewNeighborsOnLeft` | `O(N)` |
| `updateHoles` | `O(M²)` |

<img src="./runs/hole_count.png">

## References

- Kenyon & Rémila (2000), *A near-optimal solution to a two-dimensional cutting stock problem*
- Harren, Jansen, Prädel & van Stee (2014), *A (5/3 + ε)-approximation for strip packing*
- Burke, Kendall & Whitwell (2004), *A new placement heuristic for the orthogonal stock-cutting problem*
- Iori, Martello, Monaci et al., *2DPackLib*