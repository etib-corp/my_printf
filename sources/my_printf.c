#include "my_printf/my_printf.h"
#include "my_printf/format.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "my_printf/convert.h"

/* shared buffer context and writers */
struct buf_ctx {
	char *cur;
	size_t remaining;
};

static ssize_t writer_file(void *ctx, const char *data, size_t len)
{
	FILE *f	 = (FILE *)ctx;
	size_t l = len ? len : strlen(data);
	return (ssize_t)fwrite(data, 1, l, f);
}

static ssize_t writer_fd(void *ctx, const char *data, size_t len)
{
	int fd_local = (int)(intptr_t)ctx;
	size_t l	 = len ? len : strlen(data);
	return write(fd_local, data, l);
}

static ssize_t writer_buf(void *ctx, const char *data, size_t len)
{
	struct buf_ctx *c = ctx;
	size_t l		  = len ? len : strlen(data);
	size_t to_copy	  = l;
	if (c->remaining != (size_t)-1 && to_copy > c->remaining)
		to_copy = c->remaining;
	if (to_copy > 0) {
		memcpy(c->cur, data, to_copy);
		c->cur += to_copy;
		if (c->remaining != (size_t)-1)
			c->remaining -= to_copy;
		*c->cur = '\0';
	}
	return (ssize_t)to_copy;
}
static void populate_conversion_values(format_part_t **parts, va_list ap)
{
	if (parts == NULL)
		return;
	va_list args;
	va_copy(args, ap);
	for (size_t i = 0; parts[i] != NULL; ++i) {
		if (parts[i]->type != FORMAT_PART_TYPE_CONVERSION)
			continue;
		conversion_specifier_t *spec = &parts[i]->conversion;
		if (spec->width.type == FORMAT_ARG_VALUE_ARGUMENT) {
			spec->width.value = va_arg(args, int);
			spec->width.type  = FORMAT_ARG_VALUE_LITERAL;
		}
		if (spec->precision.type == FORMAT_ARG_VALUE_ARGUMENT) {
			spec->precision.value = va_arg(args, int);
			spec->precision.type  = FORMAT_ARG_VALUE_LITERAL;
		}
		switch (spec->type) {
			case CONVERSION_TYPE_D:
			case CONVERSION_TYPE_I:
			case CONVERSION_TYPE_O:
			case CONVERSION_TYPE_U:
			case CONVERSION_TYPE_X:
			case CONVERSION_TYPE_X_UPPER:
			case CONVERSION_TYPE_D_UPPER:
			case CONVERSION_TYPE_O_UPPER:
			case CONVERSION_TYPE_U_UPPER:
				spec->value.int_value = va_arg(args, int);
				break;
			case CONVERSION_TYPE_F:
			case CONVERSION_TYPE_F_UPPER:
			case CONVERSION_TYPE_E:
			case CONVERSION_TYPE_E_UPPER:
			case CONVERSION_TYPE_G:
			case CONVERSION_TYPE_G_UPPER:
			case CONVERSION_TYPE_A:
			case CONVERSION_TYPE_A_UPPER:
				spec->value.double_value = va_arg(args, double);
				break;
			case CONVERSION_TYPE_C:
			case CONVERSION_TYPE_C_UPPER:
				spec->value.char_value = (char)va_arg(args, int);
				break;
			case CONVERSION_TYPE_S:
			case CONVERSION_TYPE_S_UPPER:
				spec->value.string_value = va_arg(args, char *);
				break;
			case CONVERSION_TYPE_P:
				spec->value.pointer_value = va_arg(args, void *);
				break;
			default:
				break;
		}
	}
	va_end(args);
}

static int process_format_parts(format_part_t **parts, void *ctx,
								ssize_t (*writer)(void *, const char *,
												  size_t));
int my_printf(const char *format, ...)
{
	va_list ap = { 0 };
	va_start(ap, format);
	int result = my_vprintf(format, ap);
	va_end(ap);
	return result;
}

int my_fprintf(FILE *stream, const char *format, ...)
{
	va_list ap = { 0 };
	va_start(ap, format);
	int result = my_vfprintf(stream, format, ap);
	va_end(ap);
	return result;
}

int my_sprintf(char *str, const char *format, ...)
{
	va_list ap = { 0 };
	va_start(ap, format);
	int result = my_vsprintf(str, format, ap);
	va_end(ap);
	return result;
}

int my_snprintf(char *str, size_t size, const char *format, ...)
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

int my_dprintf(int fd, const char *format, ...)
{
	va_list ap = { 0 };
	va_start(ap, format);
	int result = my_vdprintf(fd, format, ap);
	va_end(ap);
	return result;
}

int my_vprintf(const char *format, va_list ap)
{
	format_part_t **parts = initialize_format_parts(format, ap);
	int total_printed	  = 0;
	/* Process parts using the writer */
	total_printed = 0;
	if (parts != NULL) {
		populate_conversion_values(parts, ap);
		total_printed = process_format_parts(parts, stdout, writer_file);
	}
	destroy_format_parts(parts);
	return total_printed;
}

int my_vfprintf(FILE *stream, const char *format, va_list ap)
{
	format_part_t **parts = initialize_format_parts(format, ap);
	int total_printed	  = 0;
	if (parts != NULL) {
		populate_conversion_values(parts, ap);
		total_printed = process_format_parts(parts, stream, writer_file);
	}
	destroy_format_parts(parts);
	return total_printed;
}

int my_vsprintf(char *str, const char *format, va_list ap)
{
	format_part_t **parts = initialize_format_parts(format, ap);
	int total_printed	  = 0;
	if (parts != NULL) {
		populate_conversion_values(parts, ap);
		struct buf_ctx b = { .cur = str, .remaining = (size_t)-1 };
		total_printed	 = process_format_parts(parts, &b, writer_buf);
	}
	destroy_format_parts(parts);
	return total_printed;
}

int my_vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
	format_part_t **parts = initialize_format_parts(format, ap);
	int total_printed	  = 0;
	if (parts != NULL) {
		populate_conversion_values(parts, ap);
		struct buf_ctx b = { .cur = str, .remaining = size > 0 ? size - 1 : 0 };
		total_printed	 = process_format_parts(parts, &b, writer_buf);
	}
	destroy_format_parts(parts);
	return total_printed;
}

int my_vasprintf(char **ret, const char *format, va_list ap)
{
	format_part_t **parts = initialize_format_parts(format, ap);
	size_t total_length	  = 0;
	if (parts != NULL) {
		populate_conversion_values(parts, ap);
		for (size_t i = 0; parts[i] != NULL; ++i) {
			if (parts[i]->type == FORMAT_PART_TYPE_LITERAL) {
				total_length += strlen(parts[i]->literal.data);
			} else if (parts[i]->type == FORMAT_PART_TYPE_CONVERSION) {
				char *converted = convert(&parts[i]->conversion);
				if (converted != NULL) {
					total_length += strlen(converted);
					free(converted);
				}
			}
		}
	}
	*ret = malloc(total_length + 1);
	if (*ret == NULL) {
		destroy_format_parts(parts);
		return -1;
	}

	if (parts != NULL) {
		struct buf_ctx b = { .cur = *ret, .remaining = total_length };
		process_format_parts(parts, &b, writer_buf);
		*(*ret + total_length) = '\0';
	}
	destroy_format_parts(parts);
	return (int)total_length;
}

int my_vdprintf(int fd, const char *format, va_list ap)
{
	format_part_t **parts = initialize_format_parts(format, ap);
	int total_printed	  = 0;
	if (parts != NULL) {
		populate_conversion_values(parts, ap);
		total_printed =
			process_format_parts(parts, (void *)(intptr_t)fd, writer_fd);
	}
	destroy_format_parts(parts);
	return total_printed;
}

/*
 * Generic processor for format parts. The writer callback should return
 * the number of bytes actually written (may be less than requested), but
 * this function counts the intended length (sum of part lengths) which
 * matches the previous behavior of these APIs.
 */
static int process_format_parts(format_part_t **parts, void *ctx,
								ssize_t (*writer)(void *, const char *, size_t))
{
	int total_printed = 0;
	if (parts == NULL)
		return 0;
	for (size_t i = 0; parts[i] != NULL; ++i) {
		if (parts[i]->type == FORMAT_PART_TYPE_LITERAL) {
			const char *lit = parts[i]->literal.data;
			size_t len		= strlen(lit);
			writer(ctx, lit, len);
			total_printed += (int)len;
		} else if (parts[i]->type == FORMAT_PART_TYPE_CONVERSION) {
			char *converted = convert(&parts[i]->conversion);
			if (converted != NULL) {
				size_t len = strlen(converted);
				writer(ctx, converted, len);
				total_printed += (int)len;
				free(converted);
			}
		}
	}
	return total_printed;
}
