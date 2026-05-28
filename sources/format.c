#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

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

const format_flag_e flag_list[FORMAT_FLAG_COUNT] = {
	FORMAT_FLAG_NONE,		 FORMAT_FLAG_ALTERNATE,	 FORMAT_FLAG_ZERO_PAD,
	FORMAT_FLAG_LEFT_ADJUST, FORMAT_FLAG_SPACE_SIGN, FORMAT_FLAG_FORCE_SIGN,
	FORMAT_FLAG_GROUPING,
};

const char *flag_to_string[FORMAT_FLAG_COUNT] = {
	[FORMAT_FLAG_NONE] = "",		[FORMAT_FLAG_ALTERNATE] = "#",
	[FORMAT_FLAG_ZERO_PAD] = "0",	[FORMAT_FLAG_LEFT_ADJUST] = "-",
	[FORMAT_FLAG_SPACE_SIGN] = " ", [FORMAT_FLAG_FORCE_SIGN] = "+",
	[FORMAT_FLAG_GROUPING] = "'",
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
	CONVERSION_TYPE_NONE,	 CONVERSION_TYPE_PERCENT, CONVERSION_TYPE_D,
	CONVERSION_TYPE_I,		 CONVERSION_TYPE_O,		  CONVERSION_TYPE_U,
	CONVERSION_TYPE_X,		 CONVERSION_TYPE_X_UPPER, CONVERSION_TYPE_D_UPPER,
	CONVERSION_TYPE_O_UPPER, CONVERSION_TYPE_U_UPPER, CONVERSION_TYPE_E,
	CONVERSION_TYPE_E_UPPER, CONVERSION_TYPE_F,		  CONVERSION_TYPE_F_UPPER,
	CONVERSION_TYPE_G,		 CONVERSION_TYPE_G_UPPER, CONVERSION_TYPE_A,
	CONVERSION_TYPE_A_UPPER, CONVERSION_TYPE_C,		  CONVERSION_TYPE_C_UPPER,
	CONVERSION_TYPE_S,		 CONVERSION_TYPE_S_UPPER, CONVERSION_TYPE_P,
	CONVERSION_TYPE_N,
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
	/* Prefer the longest matching modifier string to handle prefixes like 'l'
	 * vs 'll'. */
	size_t best_len = 0;
	size_t best_idx = 0;
	for (size_t i = 0; i < FORMAT_LENGTH_MODIFIER_COUNT; i++) {
		const char *modifier_str = length_modifier_to_string[i];
		if (modifier_str == NULL)
			continue;
		size_t modifier_len = strlen(modifier_str);
		if (modifier_len > 0 && strncmp(*token, modifier_str, modifier_len) == 0
			&& modifier_len > best_len) {
			best_len = modifier_len;
			best_idx = i;
		}
	}
	if (best_len > 0) {
		*token += best_len;
		return format_length_modifier_list[best_idx];
	}
	return FORMAT_LENGTH_NONE;
}

static uint32_t extract_flags(char **token)
{
	uint32_t flags = 0;

	while (**token != '\0') {
		int matched = 0;

		for (size_t i = 0; i < FORMAT_FLAG_COUNT; i++) {
			const char *flag_str = flag_to_string[i];

			if (flag_str == NULL)
				continue;
			size_t flag_len = strlen(flag_str);

			if (flag_len > 0 && strncmp(*token, flag_str, flag_len) == 0) {
				flags |= (uint32_t)i;
				*token += flag_len;
				matched = 1;
				break;
			}
		}
		if (!matched)
			return flags;
	}
	return flags;
}

static uint32_t extract_positional_index(char **token)
{
	char *cursor   = *token;
	uint32_t value = 0;

	if (!isdigit((unsigned char)*cursor))
		return 0;
	while (isdigit((unsigned char)*cursor)) {
		value = (value * 10u) + (uint32_t)(*cursor - '0');
		cursor++;
	}
	if (*cursor == '$') {
		*token = cursor + 1;
		return value;
	}
	return 0;
}

static format_arg_value_t extract_arg_value(char **token)
{
	format_arg_value_t value = { .type			 = FORMAT_ARG_VALUE_NONE,
								 .value			 = 0,
								 .argument_index = 0 };
	char *cursor			 = *token;

	if (*cursor == '*') {
		value.type = FORMAT_ARG_VALUE_ARGUMENT;
		cursor++;
		while (isdigit((unsigned char)*cursor))
			cursor++;
		if (*cursor == '$')
			cursor++;
		*token = cursor;
		return value;
	}
	if (!isdigit((unsigned char)*cursor))
		return value;
	value.type = FORMAT_ARG_VALUE_LITERAL;
	while (isdigit((unsigned char)*cursor)) {
		value.value = (value.value * 10) + (*cursor - '0');
		cursor++;
	}
	*token = cursor;
	return value;
}

static char *dup_range(const char *start, size_t length)
{
	char *copy = malloc(length + 1);

	if (copy == NULL)
		return NULL;
	memcpy(copy, start, length);
	copy[length] = '\0';
	return copy;
}

static int is_conversion_type_char(char c)
{
	for (size_t i = 0; i < CONVERSION_TYPE_COUNT; i++) {
		const char *conv_str = conversion_type_to_char[i];
		if (conv_str == NULL)
			continue;
		if (conv_str[0] != '\0' && conv_str[0] == c)
			return 1;
	}
	return 0;
}

static conversion_type_t extract_conversion_type(char **token)
{
	for (size_t i = 0; i < CONVERSION_TYPE_COUNT; i++) {
		const char *conv_str = conversion_type_to_char[i];
		if (conv_str[0] != '\0' && conv_str[0] == **token) {
			(*token)++;
			return conversion_type_list[i];
		}
	}
	return CONVERSION_TYPE_NONE;
}

static format_part_t *format_part_from_conversion(char *token, va_list ap)
{
	format_part_t *current_part = calloc(1, sizeof(format_part_t));
	char *cursor				= token;

	if (current_part == NULL)
		return NULL;
	(void)ap;
	current_part->type						= FORMAT_PART_TYPE_CONVERSION;
	current_part->conversion.argument_index = extract_positional_index(&cursor);
	current_part->conversion.flags			= extract_flags(&cursor);
	current_part->conversion.vector_separator = '\0';
	current_part->conversion.width			  = extract_arg_value(&cursor);
	if (*cursor == '.') {
		cursor++;
		current_part->conversion.precision = extract_arg_value(&cursor);
		if (current_part->conversion.precision.type == FORMAT_ARG_VALUE_NONE) {
			current_part->conversion.precision.type	 = FORMAT_ARG_VALUE_LITERAL;
			current_part->conversion.precision.value = 0;
		}
	} else {
		current_part->conversion.precision.type	 = FORMAT_ARG_VALUE_NONE;
		current_part->conversion.precision.value = 0;
	}
	current_part->conversion.length = extract_length_modifier(&cursor);
	if ((*cursor == ',' || *cursor == ';' || *cursor == ':' || *cursor == '_')
		&& cursor[1] != '\0') {
		current_part->conversion.vector_separator = *cursor;
		cursor++;
	}
	current_part->conversion.type = extract_conversion_type(&cursor);
	if (current_part->conversion.type == CONVERSION_TYPE_NONE) {
		free(current_part);
		return NULL;
	}
	return current_part;
}

static format_part_t *format_part_from_literal(char *token)
{
	format_part_t *current_part = calloc(1, sizeof(format_part_t));
	if (current_part == NULL)
		return NULL;
	current_part->type		   = FORMAT_PART_TYPE_LITERAL;
	current_part->literal.data = strdup(token);
	if (current_part->literal.data == NULL) {
		free(current_part);
		return NULL;
	}
	return current_part;
}

static format_part_t **add_part_to_list(format_part_t **parts,
										format_part_t *part)
{
	size_t count			  = 0;
	format_part_t **new_parts = NULL;
	for (count = 0; parts != NULL && parts[count] != NULL; count++)
		;
	new_parts = realloc(parts, sizeof(format_part_t *) * (count + 2));
	if (new_parts == NULL)
		return NULL;
	new_parts[count]	 = part;
	new_parts[count + 1] = NULL;
	return new_parts;
}

format_part_t **initialize_format_parts(const char *format, va_list ap)
{
	format_part_t **parts		= NULL;
	const char *segment_start	= format;
	const char *cursor			= format;
	char *segment				= NULL;
	format_part_t *current_part = NULL;

	(void)ap;
	if (format == NULL)
		return NULL;
	while (*cursor != '\0') {
		if (*cursor != '%') {
			cursor++;
			continue;
		}
		if (cursor > segment_start) {
			segment =
				dup_range(segment_start, (size_t)(cursor - segment_start));
			if (segment == NULL)
				goto error;
			current_part = format_part_from_literal(segment);
			free(segment);
			if (current_part == NULL)
				goto error;
			parts = add_part_to_list(parts, current_part);
			if (parts == NULL)
				goto error;
		}
		cursor++;
		segment_start = cursor;
		while (*cursor != '\0' && !is_conversion_type_char(*cursor))
			cursor++;
		if (*cursor == '\0')
			goto error;
		segment =
			dup_range(segment_start, (size_t)(cursor - segment_start + 1));
		if (segment == NULL)
			goto error;

		current_part = format_part_from_conversion(segment, ap);
		free(segment);
		if (current_part == NULL)
			goto error;
		parts = add_part_to_list(parts, current_part);
		if (parts == NULL)
			goto error;
		cursor++;
		segment_start = cursor;
	}
	if (*segment_start != '\0') {
		current_part = format_part_from_literal((char *)segment_start);
		if (current_part == NULL)
			goto error;
		parts = add_part_to_list(parts, current_part);
		if (parts == NULL)
			goto error;
	}
	return parts;
error:
	destroy_format_parts(parts);
	return NULL;
}

void destroy_format_parts(format_part_t **parts)
{
	format_part_t **original_parts = parts;

	if (parts == NULL)
		return;
	while (*parts != NULL) {
		if ((*parts)->type == FORMAT_PART_TYPE_LITERAL) {
			free((void *)(*parts)->literal.data);
		}
		free(*parts);
		parts++;
	}
	free(original_parts);
}
