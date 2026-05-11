#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "my_printf/format.h"

const format_length_modifier_t
	format_length_modifier_list[FORMAT_LENGTH_MODIFIER_COUNT] = {
		FORMAT_LENGTH_NONE, FORMAT_LENGTH_HH,  FORMAT_LENGTH_H,
		FORMAT_LENGTH_L,	FORMAT_LENGTH_LL,  FORMAT_LENGTH_J,
		FORMAT_LENGTH_T,	FORMAT_LENGTH_Z,   FORMAT_LENGTH_L_CAPITAL,
		FORMAT_LENGTH_Q,	FORMAT_LENGTH_V,   FORMAT_LENGTH_VH,
		FORMAT_LENGTH_HV,	FORMAT_LENGTH_VL,  FORMAT_LENGTH_LV,
		FORMAT_LENGTH_VLL,	FORMAT_LENGTH_LLV,
	};

const char *length_modifier_to_string[FORMAT_LENGTH_MODIFIER_COUNT] = {
	[FORMAT_LENGTH_NONE] = "",		 [FORMAT_LENGTH_HH] = "hh",
	[FORMAT_LENGTH_H] = "h",		 [FORMAT_LENGTH_L] = "l",
	[FORMAT_LENGTH_LL] = "ll",		 [FORMAT_LENGTH_J] = "j",
	[FORMAT_LENGTH_T] = "t",		 [FORMAT_LENGTH_Z] = "z",
	[FORMAT_LENGTH_L_CAPITAL] = "L", [FORMAT_LENGTH_Q] = "q",
	[FORMAT_LENGTH_V] = "v",		 [FORMAT_LENGTH_VH] = "vh",
	[FORMAT_LENGTH_HV] = "hv",		 [FORMAT_LENGTH_VL] = "vl",
	[FORMAT_LENGTH_LV] = "lv",		 [FORMAT_LENGTH_VLL] = "vll",
	[FORMAT_LENGTH_LLV] = "llv",
};

const conversion_type_t conversion_type_list[CONVERSION_TYPE_COUNT] = {
	CONVERSION_TYPE_PERCENT, CONVERSION_TYPE_D,		  CONVERSION_TYPE_I,
	CONVERSION_TYPE_O,		 CONVERSION_TYPE_U,		  CONVERSION_TYPE_X,
	CONVERSION_TYPE_X_UPPER, CONVERSION_TYPE_D_UPPER, CONVERSION_TYPE_O_UPPER,
	CONVERSION_TYPE_U_UPPER, CONVERSION_TYPE_E,		  CONVERSION_TYPE_E_UPPER,
	CONVERSION_TYPE_F,		 CONVERSION_TYPE_F_UPPER, CONVERSION_TYPE_G,
	CONVERSION_TYPE_G_UPPER, CONVERSION_TYPE_A,		  CONVERSION_TYPE_A_UPPER,
	CONVERSION_TYPE_C,		 CONVERSION_TYPE_C_UPPER, CONVERSION_TYPE_S,
	CONVERSION_TYPE_S_UPPER, CONVERSION_TYPE_P,		  CONVERSION_TYPE_N,
};

const char *conversion_type_to_char[CONVERSION_TYPE_COUNT] = {
	[CONVERSION_TYPE_NONE] = "\0",	 [CONVERSION_TYPE_PERCENT] = "%",
	[CONVERSION_TYPE_D] = "d",		 [CONVERSION_TYPE_I] = "i",
	[CONVERSION_TYPE_O] = "o",		 [CONVERSION_TYPE_U] = "u",
	[CONVERSION_TYPE_X] = "x",		 [CONVERSION_TYPE_X_UPPER] = "X",
	[CONVERSION_TYPE_D_UPPER] = "D", [CONVERSION_TYPE_O_UPPER] = "O",
	[CONVERSION_TYPE_U_UPPER] = "U", [CONVERSION_TYPE_E] = "e",
	[CONVERSION_TYPE_E_UPPER] = "E", [CONVERSION_TYPE_F] = "f",
	[CONVERSION_TYPE_F_UPPER] = "F", [CONVERSION_TYPE_G] = "g",
	[CONVERSION_TYPE_G_UPPER] = "G", [CONVERSION_TYPE_A] = "a",
	[CONVERSION_TYPE_A_UPPER] = "A", [CONVERSION_TYPE_C] = "c",
	[CONVERSION_TYPE_C_UPPER] = "C", [CONVERSION_TYPE_S] = "s",
	[CONVERSION_TYPE_S_UPPER] = "S", [CONVERSION_TYPE_P] = "p",
	[CONVERSION_TYPE_N] = "n",
};

static format_length_modifier_t extract_length_modifier(char **token)
{
	for (size_t i = 0; i < FORMAT_LENGTH_MODIFIER_COUNT; i++) {
		const char *modifier_str = length_modifier_to_string[i];
		size_t modifier_len		 = strlen(modifier_str);
		if (strncmp(*token, modifier_str, modifier_len) == 0) {
			*token += modifier_len;	   // Move past the length modifier
			return format_length_modifier_list[i];
		}
	}
	return FORMAT_LENGTH_NONE;	  // No length modifier found
}

static conversion_type_t extract_conversion_type(char **token)
{
	for (size_t i = 0; i < CONVERSION_TYPE_COUNT; i++) {
		const char *conv_str = conversion_type_to_char[i];
		if (conv_str[0] == **token) {
			(*token)++;	   // Move past the conversion character
			return conversion_type_list[i];
		}
	}
	return CONVERSION_TYPE_NONE;	// No valid conversion type found
}

static format_part_t *format_part_from_conversion(const char *token, va_list ap,
												  size_t index,
												  size_t *consumed_length)
{
	format_part_t *part = calloc(1, sizeof(format_part_t));
	if (part == NULL)
		return NULL;

	part->type						   = FORMAT_PART_TYPE_CONVERSION;
	conversion_specifier_t *conversion = &part->conversion;
	conversion->argument_index		   = index;

	return part;
}

format_part_t **initialize_format_parts(const char *format, va_list ap)
{
	format_part_t **parts = NULL;	 // Array of pointers to format_part_t
									 // structures, dynamically allocated.
	size_t part_count = 0;	  // Number of parts parsed from the format string.
	char *token = NULL;		  // Temporary variable to hold tokens extracted by
	size_t consumed_length = 0;	   // Number of characters consumed from the
								   // format string for the current token.

	while ((token = strchr(format, '%')) != NULL) {
		parts = realloc(parts, sizeof(format_part_t *) * (part_count + 2));
		parts[part_count++] = format_part_from_conversion(
			token, ap, part_count + 1, &consumed_length);
		parts[part_count] = NULL;	 // Null-terminate the array
		format += (token - format) + consumed_length;	 // Move past the
														 // processed token
	}

	return parts;
}

void destroy_format_parts(format_part_t **parts)
{
	if (parts == NULL)
		return;
	while (*parts != NULL) {
		free(*parts);
		parts++;
	}
	free(parts);
}
