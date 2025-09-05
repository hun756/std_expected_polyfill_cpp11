# std_expected_polyfill_cpp11 — concise guide

Practical, focused README for the C++11 `std::expected` polyfill in this repository. Short, human tone, no fluff.

Where to look

- Library: `include/expected/`
- Examples: `examples/`
- Benchmarks: `bench/`
- Tests: `test/`

Features (short)
| Feature | Location | What it does |
|---|---:|---|
| expected / unexpected | `include/expected/` | value or error container |
| Monadic ops (and_then/transform/or_else) | examples, test | chain operations; short-circuit on error |
| value_or / error handling | examples, bench | convenient fallback for errors |
| Move-only / large payloads | bench/bench_edge_cases.cpp | shows costs for move and large copies |

Minimal code examples

1. Construct and fallback

```cpp
#include <expected/expected.hpp>
// ...
std_::expected<int, std::string> f() { return 42; }
auto e = f();
int v = e.value_or(-1);
```

2. Chain operations

```cpp
std_::expected<int,std::string> add_one(int x){ return x+1; }
auto r = std_::expected<int,std::string>(1).and_then(add_one).and_then(add_one);
// r contains value or error
```

3. Move-only payload

```cpp
struct MoveOnly { std::unique_ptr<int> p; MoveOnly(int v):p(new int(v)){} };
std_::expected<MoveOnly,std::string> e = MoveOnly(42);
```

Build & run (PowerShell)

Note: for meaningful benchmark numbers use Release.

```powershell
# Configure Release and build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j 0

# Run example (adjust name as needed)
build\bin\example1.exe

# Run a single benchmark
build\bin\bench_monadic_chain.exe

# Run tests
cmake --build build --target test --config Release
ctest --test-dir build --output-on-failure
```

Notes

- This README is intentionally short. For API details, read headers under `include/expected/`.
- Benchmarks live in `bench/` and compare common patterns (value_or vs exceptions, monadic chains, move-only costs).

License

- See `LICENSE` in the project root.

# 📦 Modern C++ Project Template

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
![C++](https://img.shields.io/badge/C++-20-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.16%2B-blue)

A modern, feature-rich, customizable C++20 project template

## ✨ Features

- 🔄 Modern CMake (3.16+) setup
- 🚀 C++20 standard
- 📚 Google Test integration
- 📝 Comprehensive documentation structure
- 🧪 Example code
- 🛠️ Out-of-the-box compiler warnings
- 🔍 Code sanitizers
- 📊 Code coverage support
- 📋 Static analysis support
- 🔄 Continuous Integration ready
- 📦 Package management support
- 🚢 Installation and export targets

## 📋 Requirements

- CMake 3.16 or newer
- C++20 compatible compiler
- Git

## 🚀 Quick Start

```bash
# Clone the repository
git clone https://github.com/hun756/CPP-Starter-Template.git my-project
cd my-project

# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Run tests
ctest

# Run executable
./bin/cpp_project_template
```

## 📊 Project Structure

```
my-project/
├── CMakeLists.txt         # Main CMake configuration
├── cmake/                 # CMake modules and utilities
│   ├── CompilerWarnings.cmake
│   ├── Sanitizers.cmake
│   ├── StaticAnalyzers.cmake
│   ├── LTO.cmake
│   ├── Packaging.cmake
│   ├── FindGTest.cmake
│   ├── FindGBenchmark.cmake  # Benchmark için eklendi
│   └── configs/           # Configuration templates
├── include/               # Public header files
│   └── myproject/
│       ├── ModuleA.h
│       └── ModuleB.h
├── src/                   # Source files
│   ├── ModuleA.cpp
│   ├── ModuleB.cpp
│   └── main.cpp
├── test/                  # Test files
│   ├── CMakeLists.txt
│   ├── ModuleATest.cpp
│   └── ModuleBTest.cpp
├── bench/                 # Benchmark files
│   ├── CMakeLists.txt
│   ├── StringProcessorBench.cpp
│   └── CalculatorBench.cpp
├── examples/              # Example programs
│   ├── CMakeLists.txt
│   ├── example1.cpp
│   └── example2.cpp
├── docs/                  # Documentation
│   ├── CMakeLists.txt
│   └── Doxyfile.in
├── .clang-format          # Formatting configuration
├── .clang-tidy            # Static analysis configuration
├── .gitignore
└── README.md
```

## 🔧 Build Options

| Option            | Default | Description                            |
| ----------------- | ------- | -------------------------------------- |
| BUILD_SHARED_LIBS | OFF     | Build shared libraries                 |
| BUILD_EXAMPLES    | ON      | Build example programs                 |
| BUILD_TESTS       | ON      | Build tests                            |
| BUILD_BENCHMARKS  | ON      | Build benchmark programs               |
| ENABLE_COVERAGE   | OFF     | Enable coverage reporting              |
| ENABLE_SANITIZERS | OFF     | Enable sanitizers in debug builds      |
| ENABLE_PCH        | OFF     | Enable precompiled headers             |
| ENABLE_LTO        | OFF     | Enable Link Time Optimization          |
| ENABLE_CPPCHECK   | OFF     | Enable static analysis with cppcheck   |
| ENABLE_CLANG_TIDY | OFF     | Enable static analysis with clang-tidy |

Example usage:

```bash
cmake .. -DBUILD_SHARED_LIBS=ON -DENABLE_SANITIZERS=ON
```

## 📚 Documentation

Generate documentation with Doxygen:

```bash
cmake --build . --target docs
```

Create distributable packages:

```bash
cmake --build . --target package
```

This project can be easily integrated in other CMake projects:

```cmake
find_package(cpp_project_template REQUIRED)
target_link_libraries(your_target PRIVATE cpp_project_template::cpp_project_template)
```

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Commit your changes: `git commit -am 'Add my feature'`
4. Push to the branch: `git push origin feature/my-feature`
5. Submit a pull request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
