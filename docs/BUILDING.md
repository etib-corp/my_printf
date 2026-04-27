# Building my_printf

## Prerequisites

- CMake 3.10 or higher
- A C compiler (e.g., GCC, Clang)
- Doxygen (optional, for generating documentation)

## Building the Library

1. Clone the repository:

    ```bash
    git clone https://github.com/etib-corp/my_printf.git
    cd my_printf
    ```

2. Configure the build using CMake:

    ```bash
    cmake -S . -B build
    ```

3. Build the library:

    ```bash
    cmake --build build
    ```

4. (Optional) Launch tests to verify the build:

    ```bash
    ctest --test-dir build/tests
    ```
