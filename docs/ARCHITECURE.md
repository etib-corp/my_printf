# Architecture of my_printf

The `my_printf` library is designed to be a flexible and efficient implementation of the standard `printf` function in C. The architecture of the library is modular, allowing for easy maintenance and extension. Below is an overview of the key components and their interactions.

## 1. Core Components

We implement this list of functions:

- **my_printf** - Print formatted output to stdout.
- **my_fprintf** - Print formatted output to a specified file stream.
- **my_sprintf** - Print formatted output to a string. The size of the outputstring is not checked, which can lead to buffer overflows if the output exceeds the allocated size.
- **my_snprintf** - Print formatted output to a string, with a specified maximum size. This function prevents buffer overflows by ensuring that the output does not exceed the allocated size.
- **my_asprintf** - Print formatted output to a string, allocating memory as needed.
- **my_dprintf** - Print formatted output to a file descriptor.

Same functions are implemented, with `va_list` arguments:

- **my_vprintf** - Print formatted output to stdout using a `va_list` of arguments.
- **my_vfprintf** - Print formatted output to a specified file stream using a `va_list` of arguments.
- **my_vsprintf** - Print formatted output to a string using a `va_list` of arguments. The size of the output string is not checked, which can lead to buffer overflows if the output exceeds the allocated size.
- **my_vsnprintf** - Print formatted output to a string, with a specified maximum size, using a `va_list` of arguments. This function prevents buffer overflows by ensuring that the output does not exceed the allocated size.
- **my_vasprintf** - Print formatted output to a string, allocating memory as needed, using a `va_list` of arguments.
- **my_vdprintf** - Print formatted output to a file descriptor using a `va_list` of arguments.

We implement `printf` and other non v functions as wrappers around their `v` counterparts, which handle the core formatting logic.

```c
int printf(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int result = vprintf(format, ap);
    va_end(ap);
    return result;
}
```

## 2. How It Works

When user calls `my_*printf` function, the library performs the following steps:

### **Parsing the Format String**

The library parses the format string to identify format specifiers (e.g., `%d`, `%s`, etc.) and their associated flags, width, precision, and length modifiers.

We obtain a list of `format_part_t` from the format string, which can be either literal text or conversion specifications. Each conversion specification is represented as a `format_part_t` with the relevant details (e.g., type, flags, width, precision).

For each conversion specification, the library retrieves the corresponding argument from the `va_list` and formats it according to the specified rules.

Step by step:

1. The library starts by parsing the format string character by character.
2. When it encounters a `%` character, it recognizes the start of a format specifier

### **Applying Conversion Logic**

For each `format_part_t` that represents a conversion, the library applies the appropriate formatting logic based on the type of conversion (e.g., integer, string, floating-point) and the specified flags, width, and precision.

After that we have a list of `formatted_part_t`, which contains the formatted string for each part of the output.

### **Output Generation**

Finally, the library concatenates the formatted parts and outputs the final string to the appropriate destination (e.g., stdout, file stream, string buffer).

The library is designed to be efficient and robust, handling various edge cases and ensuring that memory management is properly handled to avoid leaks and buffer overflows.
