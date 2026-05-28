#pragma once

#include "my_printf/format.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Function pointer type for conversion specifier handlers. Each handler
 * takes a pointer to a conversion_specifier_t structure containing the parsed
 * metadata for the conversion and returns a newly allocated string with the
 * formatted output for that conversion. The caller is responsible for freeing
 * the returned string when it is no longer needed.
 */
typedef char *(*converter_function_t)(conversion_specifier_t *specifier);

/**
 * @brief Structure representing a conversion specifier handler. It contains the
 * conversion type that the handler is responsible for and a pointer to the
 * function that implements the conversion.
 */
typedef struct converter_s {
	/** The conversion type that this converter handles. */
	conversion_type_t type;
	/** The function that implements the conversion. */
	converter_function_t function;
} converter_t;

extern const converter_t converters[CONVERSION_TYPE_COUNT];

char *convert_percent(conversion_specifier_t *specifier);
char *convert_d(conversion_specifier_t *specifier);
char *convert_i(conversion_specifier_t *specifier);
char *convert_o(conversion_specifier_t *specifier);
char *convert_u(conversion_specifier_t *specifier);
char *convert_x(conversion_specifier_t *specifier);
char *convert_x_upper(conversion_specifier_t *specifier);
char *convert_d_upper(conversion_specifier_t *specifier);
char *convert_o_upper(conversion_specifier_t *specifier);
char *convert_u_upper(conversion_specifier_t *specifier);
char *convert_e(conversion_specifier_t *specifier);
char *convert_e_upper(conversion_specifier_t *specifier);
char *convert_f(conversion_specifier_t *specifier);
char *convert_f_upper(conversion_specifier_t *specifier);
char *convert_g(conversion_specifier_t *specifier);
char *convert_g_upper(conversion_specifier_t *specifier);
char *convert_a(conversion_specifier_t *specifier);
char *convert_a_upper(conversion_specifier_t *specifier);
char *convert_c(conversion_specifier_t *specifier);
char *convert_c_upper(conversion_specifier_t *specifier);
char *convert_s(conversion_specifier_t *specifier);
char *convert_s_upper(conversion_specifier_t *specifier);
char *convert_p(conversion_specifier_t *specifier);
char *convert_n(conversion_specifier_t *specifier);

char *convert(conversion_specifier_t *specifier);

#ifdef __cplusplus
}
#endif