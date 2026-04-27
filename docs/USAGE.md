# Use my_printf

To use the `my_printf` library in your C projects, follow the instructions below:

## 1. Link the Library

When compiling your project, link against the `my_printf` library. If you are using CMake, you can add the following lines to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
    my_printf
    GIT_REPOSITORY https://github.com/etib-corp/my_printf.git
    GIT_TAG        main
)

target_link_libraries(your_target PRIVATE my_printf)
```

## 2. Include the Header

Include the main header file in your source code:

```c
#include <my_printf/my_printf.h>
```

## 3. Use the Functions

You can now use the various `my_printf` functions in your code. Here are some examples:

```c
int main() {
    my_printf("Hello, %s!\n", "world");
    my_printf("The value of pi is approximately %.2f\n", 3.14159);
    my_printf("Binary representation of %d is %b\n", 10, 10);
    return 0;
}
```
