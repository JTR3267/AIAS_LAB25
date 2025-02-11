# ACAL Curriculum Lab 25 - Event-Driven Simulation and Simulator Design

[Documentation Link](https://course.playlab.tw/md/XtO984FhRluy1AOBNfW3gw)

## Quick Start

```shell
$ git submodule update --init --recursive
$ BUILD_DIR="build/debug" && \
    cmake -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Debug && \
    cmake --build "${BUILD_DIR}" -j $(nproc) --target <TARGET>
```

## File Structure

```
lab25/
├── CMakeLists.txt          # Top-level CMake file for the project
├── demo/                   # Source code of Lab 25-2 & Lab 25-3
│   ├── CMakeLists.txt      # CMake settings for demo/ folder
│   ├── main.cc             # Entry point of the simulator program
│   ├── Memory.cc
│   ├── Memory.hh
│   ├── TrafficGenerator.cc
│   └── TrafficGenerator.hh
├── emulator/               # Source code of Lab 25-4
└── third-party/            # Third-party libraries
    ├── acalsim/            # ACALSim Framework Library (as a Git submodule)
    └── CMakeLists.txt      # CMake settings for third-party/ folder
```
