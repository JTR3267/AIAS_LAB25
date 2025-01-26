
[Lab25 Documentation Link](https://codimd.playlab.tw/PfPUSKZ8T3yN8oRVatt6bw)

## Quick Start
```
git submodule update --init --recursive
BUILD_DIR="build/debug" &&     cmake -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Debug &&     cmake --build "${BUILD_DIR}" -j $(nproc) --target demo
```

## File Struct
```
/path/to/lab25/
├── CMakeLists.txt          # Top-level CMake file for the project
├── demo/                   # Source code of Lab 25-2 & Lab 25-3
│   ├── CMakeLists.txt      # CMake settings for demo/ folder
│   ├── main.cc             # Entry point of the simulator program
│   ├── Memory.cc
│   ├── Memory.hh
│   ├── TrafficGenerator.cc
│   └── TrafficGenerator.hh
└── third-party/            # Third-party libraries
    ├── acalsim/            # ACALSim Framework Library (as a Git submodule)
    └── CMakeLists.txt      # CMake settings for third-party/ folder
```
