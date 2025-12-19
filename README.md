# Computer_Science_for_High_Energy_Physics_project
## Parallel Mandelbrot Set with TBB and SFML using Docker

This project computes and renders the Mandelbrot set using C++ with **Intel oneAPI Threading Building Blocks (TBB)** for parallelism and **SFML** for image handling. It runs using different grain sizes for a 2D parallel loop to find the optimal configuration and then generates an image using that value.

### Remarks
- The project is built and run inside Docker containers to ensure a reproducible environment.
- These Docker containers are built in a virtual machine in Google Cloud, so nothing is run locally. VM characteristics:
  - 2 CPUs, 4 GB memory
  - Rocky Linux 9 operating system
  - 30 GB disk
- Inside the containers, the storage directory `$HOME/containers/IMAPP25` from the VM is mounted on `/workspace`.

### Features

- Parallel Mandelbrot computation using `tbb::parallel_for`
- Grain size performance exploration
- Automatic selection of optimal grain size
- Docker-based development and runtime environments
- Debug and Release builds with CMake; the executable is found in the corresponding directories (`build-d` or `build-o`) under the name `mandelbrot`
- Outputs:
  - Timing results for each grain size saved to `.txt` files
  - Optimal timing result saved to a separate `.txt`
  - PNG image generation of the Mandelbrot set

### Dependencies

All dependencies are provided through Docker:

- Ubuntu 24.04
- g++
- CMake
- SFML
- Intel oneAPI TBB

No installation in the VM is required apart from Docker.

### Project Structure

```text
$HOME/containers/IMAPP25/
├── main.cpp
├── CMakeLists.txt
├── README.md
├── Dockerfile          # Development image (build tools)
├── Dockerfile.app      # Runtime image (only libraries)
├── build-d/            # Debug build directory (generated)
│   ├── mandelbrot      # Debug executable
│   ├── timing_results.txt
│   ├── optimal_values.txt
│   └── OptimalMandelbrot.png
├── build-o/            # Release build directory (generated)
│   ├── mandelbrot      # Release executable
│   ├── timing_results.txt
│   ├── optimal_values.txt
│   └── OptimalMandelbrot.png
```
All output files are generated inside the build directory from which the executable is run.

### Build and Run

1. Build Docker images
```bash
docker build -t imapp25-dev .
docker build -f Dockerfile.app -t imapp25-app .
```

2. Run development container
```bash
docker run -it --rm \
  -v $HOME/containers/IMAPP25:/workspace \
  imapp25-dev
```

3. Build with CMake inside the container
  - Debug build:
  ```bash
  cmake -S . -B build-d -DCMAKE_BUILD_TYPE=Debug
  cmake --build build-d
  ```

  - Release build:
  ```bash
  cmake -S . -B build-o -DCMAKE_BUILD_TYPE=Release
  cmake --build build-o
  ```

4. Run the application:
  - First, exit the development container if still running.
  - Using the runtime image:
  ```bash
  docker run -it --rm \
    -v $HOME/containers/IMAPP25:/workspace \
    imapp25-app
  ```
  - Then inside the container:
  ```bash
  cd build-o
  ./mandelbrot
  ```
  (You can also run the debug version from build-d.)

### Notes

- The Release build (build-o) is recommended for performance measurements.

- The Debug build (build-d) is useful for development and debugging.

- Output files are written to the directory from which the executable is run.

### Authors

Project for Computer Science for High Energy Physics
by Juan Mínguez and Mireya Pando
