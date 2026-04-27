#include "my_printf/my_printf.h"
#include "my_printf/format.h"

int my_printf(const char *restrict format, ...)
{
	va_list ap = { 0 };
	va_start(ap, format);
	int result = my_vprintf(format, ap);
	va_end(ap);
	return result;
}

int my_fprintf(FILE *restrict stream, const char *restrict format, ...)
{
	va_list ap = { 0 };
	va_start(ap, format);
	int result = my_vfprintf(stream, format, ap);
	va_end(ap);
	return result;
}

int my_sprintf(char *restrict str, const char *restrict format, ...)
{
	va_list ap = { 0 };
	va_start(ap, format);
	int result = my_vsprintf(str, format, ap);
	va_end(ap);
	return result;
}

int my_snprintf(char *restrict str, size_t size, const char *restrict format,
				...)
{
	va_list ap = { 0 };
	va_start(ap, format);
	int result = my_vsnprintf(str, size, format, ap);
	va_end(ap);
	return result;
}

int my_asprintf(char **ret, const char *format, ...)
{
	va_list ap = { 0 };
	va_start(ap, format);
	int result = my_vasprintf(ret, format, ap);
	va_end(ap);
	return result;
}

int my_dprintf(int fd, const char *restrict format, ...)
{
	va_list ap = { 0 };
	va_start(ap, format);
	int result = my_vdprintf(fd, format, ap);
	va_end(ap);
	return result;
}

int my_vprintf(const char *restrict format, va_list ap)
{
	format_part_t *parts = parse_format_string(format, ap);
	// Implementation for handling parsed format parts
	return 0;
}

int my_vfprintf(FILE *restrict stream, const char *restrict format, va_list ap)
{
	format_part_t *parts = parse_format_string(format, ap);
	// Implementation for handling parsed format parts
	return 0;
}

int my_vsprintf(char *restrict str, const char *restrict format, va_list ap)
{
	format_part_t *parts = parse_format_string(format, ap);
	// Implementation for handling parsed format parts
	return 0;
}

int my_vsnprintf(char *restrict str, size_t size, const char *restrict format,
				 va_list ap)
{
	format_part_t *parts = parse_format_string(format, ap);
	// Implementation for handling parsed format parts
	return 0;
}

int my_vasprintf(char **ret, const char *format, va_list ap)
{
	format_part_t *parts = parse_format_string(format, ap);
	// Implementation for handling parsed format parts
	return 0;
}

int my_vdprintf(int fd, const char *restrict format, va_list ap)
{
	format_part_t *parts = parse_format_string(format, ap);
	// Implementation for handling parsed format parts
	return 0;
}
