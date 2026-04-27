#pragma once

#include <stdio.h>
#include <stdarg.h>

/**
 * @brief This function print to the stdout according to a format string and a
 * variable number of arguments, and return the number of characters printed
 * (not including the null byte used to end output to strings).
 *
 * @param format The format string that specifies how to format the output. It
 * can contain ordinary characters (which are copied to the output) and
 * conversion specifications (which specify how to format the arguments).
 *
 * @param ... The variable number of arguments that are formatted and printed
 * according to the format string. The number and types of these arguments
 * should match the conversion specifications in the format string.
 *
 * @return int The number of characters printed (not including the null byte
 * used to end output to strings). If an error occurs, a negative value is
 * returned.
 *
 * @note The behavior of this function is undefined if the format string is not
 * a valid format
 */
int my_printf(const char *restrict format, ...);

/**
 * @brief This function print to the given output stream according to a format
 * string and a variable number of arguments, and return the number of
 * characters printed (not including the null byte used to end output to
 * strings).
 *
 * @param stream The output stream to which the formatted output is written. It
 * can be a file pointer returned by fopen() or a standard stream like stdout or
 * stderr.
 * @param format The format string that specifies how to format the output. It
 * can contain ordinary characters (which are copied to the output) and
 * conversion specifications (which specify how to format the arguments).
 * @param ... The variable number of arguments that are formatted and printed
 * according to the format string. The number and types of these arguments
 * should match the conversion specifications in the format string.
 * @return int The number of characters printed (not including the null byte
 * used to end output to strings). If an error occurs, a negative value is
 * returned.
 * @note The behavior of this function is undefined if the format string is not
 * a valid format
 */
int my_fprintf(FILE *restrict stream, const char *restrict format, ...);

/**
 * @brief This function print to a string according to a format string and a
 * variable number of arguments, and return the number of characters printed
 * (not including the null byte used to end output to strings).
 *
 * @param str The string to which the formatted output is written. It should be
 * large enough to hold the resulting string and the null byte.
 *
 * @param format The format string that specifies how to format the output. It
 * can contain ordinary characters (which are copied to the output) and
 * conversion specifications (which specify how to format the arguments).
 *
 * @param ... The variable number of arguments that are formatted and printed
 * according to the format string. The number and types of these arguments
 * should match the conversion specifications in the format string.
 *
 * @return int The number of characters printed (not including the null byte
 * used to end output to strings). If an error occurs, a negative value is
 * returned.
 *
 * @note The behavior of this function is undefined if the format string is not
 * a valid format
 */
int my_sprintf(char *restrict str, const char *restrict format, ...);

/**
 * @brief This function print to a string according to a format string and a
 * variable number of arguments, and return the number of characters printed
 * (not including the null byte used to end output to strings). The size
 * parameter specifies the maximum number of characters to write to the string,
 * including the null byte. If the formatted output exceeds this limit, it is
 * truncated and null-terminated.
 *
 * @param str The string to which the formatted output is written. It should be
 * large enough to hold the resulting string and the null byte.
 *
 * @param size The maximum number of characters to write to the string,
 * including the null byte. If the formatted output exceeds this limit, it is
 * truncated and null-terminated.
 *
 * @param format The format string that specifies how to format the output. It
 * can contain ordinary characters (which are copied to the output) and
 * conversion specifications (which specify how to format the arguments).
 *
 * @param ... The variable number of arguments that are formatted and printed
 * according to the format string. The number and types of these arguments
 * should match the conversion specifications in the format string.
 *
 * @return int The number of characters printed (not including the null byte
 * used to end output to strings). If an error occurs, a negative value is
 * returned.
 *
 * @note The behavior of this function is undefined if the format string is not
 * a valid format
 */
int my_snprintf(char *restrict str, size_t size, const char *restrict format,
				...);

/**
 * @brief This function print to a string according to a format string and a
 * variable number of arguments, and return the number of characters printed
 * (not including the null byte used to end output to strings). The resulting
 * string is allocated with malloc() and should be freed by the caller.
 *
 * @param ret A pointer to a char pointer that will receive the address of the
 * allocated string containing the formatted output. The caller should free this
 * string when it is no longer needed.
 *
 * @param format The format string that specifies how to format the output. It
 * can contain ordinary characters (which are copied to the output) and
 * conversion specifications (which specify how to format the arguments).
 *
 * @param ... The variable number of arguments that are formatted and printed
 * according to the format string. The number and types of these arguments
 * should match the conversion specifications in the format string.
 *
 * @return int The number of characters printed (not including the null byte
 * used to end output to strings). If an error occurs, a negative value is
 * returned.
 *
 * @note The behavior of this function is undefined if the format string is not
 * a valid format
 */
int my_asprintf(char **ret, const char *format, ...);

/**
 * @brief This function print to the given file descriptor according to a format
 * string and a variable number of arguments, and return the number of
 * characters printed (not including the null byte used to end output to
 * strings).
 *
 * @param fd The file descriptor to which the formatted output is written. It
 * can be obtained from a file pointer using fileno() or from a socket using
 * socket().
 *
 * @param format The format string that specifies how to format the output. It
 * can contain ordinary characters (which are copied to the output) and
 * conversion specifications (which specify how to format the arguments).
 *
 * @param ... The variable number of arguments that are formatted and printed
 * according to the format string. The number and types of these arguments
 *
 * should match the conversion specifications in the format string.
 * @return int The number of characters printed (not including the null byte
 * used to end output to strings). If an error occurs, a negative value is
 * returned.
 *
 * @note The behavior of this function is undefined if the format string is not
 * a valid format
 */
int my_dprintf(int fd, const char *restrict format, ...);

/**
 * @brief This function is the va_list version of my_printf(). It print to the
 * stdout according to a format string and a va_list of arguments, and return
 * the number of characters printed (not including the null byte used to end
 * output to strings).
 *
 * @param format The format string that specifies how to format the output. It
 * can contain ordinary characters (which are copied to the output) and
 * conversion specifications (which specify how to format the arguments).
 *
 * @param ap A va_list of arguments that are formatted and printed according to
 * the format string. The number and types of these arguments should match the
 * conversion specifications in the format string.
 *
 * @return int The number of characters printed (not including the null byte
 * used to end output to strings). If an error occurs, a negative value is
 * returned.
 *
 * @note The behavior of this function is undefined if the format string is not
 * a valid format
 */
int my_vprintf(const char *restrict format, va_list ap);

/**
 * @brief This function is the va_list version of my_fprintf(). It print to the
 * given output stream according to a format string and a va_list of arguments,
 * and return the number of characters printed (not including the null byte used
 * to end output to strings).
 *
 * @param stream The output stream to which the formatted output is written. It
 * can be a file pointer returned by fopen() or a standard stream like stdout or
 * stderr.
 *
 * @param format The format string that specifies how to format the output. It
 * can contain ordinary characters (which are copied to the output) and
 * conversion specifications (which specify how to format the arguments).
 *
 * @param ap A va_list of arguments that are formatted and printed according to
 * the format string. The number and types of these arguments should match the
 * conversion specifications in the format string.
 *
 * @return int The number of characters printed (not including the null byte
 * used to end output to strings). If an error occurs, a negative value is
 * returned.
 *
 * @note The behavior of this function is undefined if the format string is not
 * a valid format
 */
int my_vfprintf(FILE *restrict stream, const char *restrict format, va_list ap);

/**
 * @brief This function is the va_list version of my_sprintf(). It print to a
 * string according to a format string and a va_list of arguments, and return
 * the number of characters printed (not including the null byte used to end
 * output to strings).
 *
 * @param str The string to which the formatted output is written. It should be
 * large enough to hold the resulting string and the null byte.
 *
 * @param format The format string that specifies how to format the output. It
 * can contain ordinary characters (which are copied to the output) and
 * conversion specifications (which specify how to format the arguments).
 *
 * @param ap A va_list of arguments that are formatted and printed according to
 * the format string. The number and types of these arguments should match the
 * conversion specifications in the format string.
 *
 * @return int The number of characters printed (not including the null byte
 * used to end output to strings). If an error occurs, a negative value is
 * returned.
 *
 * @note The behavior of this function is undefined if the format string is not
 * a valid format
 */
int my_vsprintf(char *restrict str, const char *restrict format, va_list ap);

/**
 * @brief This function is the va_list version of my_snprintf(). It print to a
 * string according to a format string and a va_list of arguments, and return
 * the number of characters printed (not including the null byte used to end
 * output to strings). The size parameter specifies the maximum number of
 * characters to write to the string, including the null byte. If the formatted
 * output exceeds this limit, it is truncated and null-terminated.
 *
 * @param str The string to which the formatted output is written. It should be
 * large enough to hold the resulting string and the null byte.
 *
 * @param size The maximum number of characters to write to the string,
 * including the null byte. If the formatted output exceeds this limit, it is
 * truncated and null-terminated.
 *
 * @param format The format string that specifies how to format the output. It
 * can contain ordinary characters (which are copied to the output) and
 * conversion specifications (which specify how to format the arguments).
 *
 * @param ap A va_list of arguments that are formatted and printed according to
 * the format string. The number and types of these arguments should match the
 * conversion specifications in the format string.
 *
 * @return int The number of characters printed (not including the null byte
 * used to end output to strings). If an error occurs, a negative value is
 * returned.
 *
 * @note The behavior of this function is undefined if the format string is not
 * a valid format
 */
int my_vsnprintf(char *restrict str, size_t size, const char *restrict format,
				 va_list ap);

/**
 * @brief This function is the va_list version of my_asprintf(). It print to a
 * string according to a format string and a va_list of arguments, and return
 * the number of characters printed (not including the null byte used to end
 * output to strings). The resulting string is allocated with malloc() and
 * should be freed by the caller.
 *
 * @param ret A pointer to a char pointer that will receive the address of the
 * allocated string containing the formatted output. The caller should free this
 * string when it is no longer needed.
 *
 * @param format The format string that specifies how to format the output. It
 * can contain ordinary characters (which are copied to the output) and
 * conversion specifications (which specify how to format the arguments).
 *
 * @param ap A va_list of arguments that are formatted and printed according to
 * the format string. The number and types of these arguments should match the
 * conversion specifications in the format string.
 *
 * @return int The number of characters printed (not including the null byte
 * used to end output to strings). If an error occurs, a negative value is
 * returned.
 *
 * @note The behavior of this function is undefined if the format string is not
 * a valid format
 */
int my_vasprintf(char **ret, const char *format, va_list ap);

/**
 * @brief This function is the va_list version of my_dprintf(). It print to the
 * given file descriptor according to a format string and a va_list of
 * arguments, and return the number of characters printed (not including the
 * null byte used to end output to strings).
 *
 * @param fd The file descriptor to which the formatted output is written. It
 * can be obtained from a file pointer using fileno() or from a socket using
 * socket().
 *
 * @param format The format string that specifies how to format the output. It
 * can contain ordinary characters (which are copied to the output) and
 * conversion specifications (which specify how to format the arguments).
 *
 * @param ap A va_list of arguments that are formatted and printed according to
 * the format string. The number and types of these arguments should match the
 * conversion specifications in the format string.
 *
 * @return int The number of characters printed (not including the null byte
 * used to end output to strings). If an error occurs, a negative value is
 * returned.
 *
 * @note The behavior of this function is undefined if the format string is not
 * a valid format
 */
int my_vdprintf(int fd, const char *restrict format, va_list ap);
