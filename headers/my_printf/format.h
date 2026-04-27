#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

/**
 * @brief A non-owning slice of text inside the original format string.
 */
typedef struct format_text_span_s {
	/** Pointer to the first character in the original format string. */
	const char *data;
	/** Number of characters in the span (not including a null terminator). */
	size_t length;
} format_text_span_t;

/**
 * @brief Flag bits parsed after '%'.
 */
typedef enum format_flag_e {
	FORMAT_FLAG_ALTERNATE	= 1u << 0,	  ///< '#'
	FORMAT_FLAG_ZERO_PAD	= 1u << 1,	  ///< '0'
	FORMAT_FLAG_LEFT_ADJUST = 1u << 2,	  ///< '-'
	FORMAT_FLAG_SPACE_SIGN	= 1u << 3,	  ///< ' '
	FORMAT_FLAG_FORCE_SIGN	= 1u << 4,	  ///< '+'
	FORMAT_FLAG_GROUPING	= 1u << 5,	  ///< '\''
} format_flag_e;

/**
 * @brief How width/precision values are provided.
 */
typedef enum format_arg_value_type_e {
	FORMAT_ARG_VALUE_NONE,		  ///< Value is absent.
	FORMAT_ARG_VALUE_LITERAL,	  ///< Value comes from parsed digits.
	FORMAT_ARG_VALUE_ARGUMENT,	  ///< Value is supplied by '*' argument.
} format_arg_value_type_e;

/**
 * @brief Stores width/precision as either literal digits or '*' argument.
 */
typedef struct format_arg_value_s {
	/** Source kind for this value. */
	format_arg_value_type_e type;
	/** Parsed numeric value when type is FORMAT_ARG_VALUE_LITERAL. */
	int value;
	/** 1-based argument index for positional '*' (0 when not positional). */
	uint32_t argument_index;
} format_arg_value_t;

/**
 * @brief Length modifier parsed before conversion type.
 */
typedef enum format_length_modifier_e {
	FORMAT_LENGTH_NONE,			///< No length modifier.
	FORMAT_LENGTH_HH,			///< hh
	FORMAT_LENGTH_H,			///< h
	FORMAT_LENGTH_L,			///< l
	FORMAT_LENGTH_LL,			///< ll
	FORMAT_LENGTH_J,			///< j
	FORMAT_LENGTH_T,			///< t
	FORMAT_LENGTH_Z,			///< z
	FORMAT_LENGTH_L_CAPITAL,	///< L
	FORMAT_LENGTH_Q,			///< q (extension/deprecated)
	FORMAT_LENGTH_V,			///< v (extension)
	FORMAT_LENGTH_VH,			///< vh (extension)
	FORMAT_LENGTH_HV,			///< hv (extension)
	FORMAT_LENGTH_VL,			///< vl (extension)
	FORMAT_LENGTH_LV,			///< lv (extension)
	FORMAT_LENGTH_VLL,			///< vll (extension)
	FORMAT_LENGTH_LLV,			///< llv (extension)
} format_length_modifier_e;

/**
 * @brief Conversion character parsed at the end of a directive.
 */
typedef enum conversion_type_e {
	CONVERSION_TYPE_NONE,		///< Invalid or not-yet-parsed conversion.
	CONVERSION_TYPE_PERCENT,	///< %%
	CONVERSION_TYPE_D,			///< %d
	CONVERSION_TYPE_I,			///< %i
	CONVERSION_TYPE_O,			///< %o
	CONVERSION_TYPE_U,			///< %u
	CONVERSION_TYPE_X,			///< %x
	CONVERSION_TYPE_X_UPPER,	///< %X
	CONVERSION_TYPE_D_UPPER,	///< %D (deprecated)
	CONVERSION_TYPE_O_UPPER,	///< %O (deprecated)
	CONVERSION_TYPE_U_UPPER,	///< %U (deprecated)
	CONVERSION_TYPE_E,			///< %e
	CONVERSION_TYPE_E_UPPER,	///< %E
	CONVERSION_TYPE_F,			///< %f
	CONVERSION_TYPE_F_UPPER,	///< %F
	CONVERSION_TYPE_G,			///< %g
	CONVERSION_TYPE_G_UPPER,	///< %G
	CONVERSION_TYPE_A,			///< %a
	CONVERSION_TYPE_A_UPPER,	///< %A
	CONVERSION_TYPE_C,			///< %c
	CONVERSION_TYPE_C_UPPER,	///< %C
	CONVERSION_TYPE_S,			///< %s
	CONVERSION_TYPE_S_UPPER,	///< %S
	CONVERSION_TYPE_P,			///< %p
	CONVERSION_TYPE_N,			///< %n
} conversion_type_e;

/**
 * @brief Parsed metadata for one conversion directive starting with '%'.
 */
typedef struct conversion_specifier_s {
	/** 1-based positional argument index for the converted value (0 if
	 * sequential). */
	uint32_t argument_index;
	/** Bitmask of format_flag_e values. */
	uint32_t flags;
	/** Optional vector separator (',', ';', ':', '_', or '\0' when absent). */
	char vector_separator;
	/** Optional minimum field width. */
	format_arg_value_t width;
	/** Optional precision. */
	format_arg_value_t precision;
	/** Optional length modifier. */
	format_length_modifier_e length;
	/** Conversion kind. */
	conversion_type_e type;
	/** Value for the conversion. */
	union {
		/** Integer value for numeric conversions. */
		int int_value;
		/** Floating-point value for floating-point conversions. */
		double double_value;
		/** Pointer value for %p conversion. */
		const void *pointer_value;
		/** Character value for %c conversion. */
		char char_value;
		/** String value for %s conversion. */
		char *string_value;
	} value;
} conversion_specifier_t;

typedef enum format_part_type_e {
	FORMAT_PART_TYPE_LITERAL,		///< Plain text copied as-is.
	FORMAT_PART_TYPE_CONVERSION,	///< Parsed '%' conversion directive.
} format_part_type_e;

/**
 * @brief One piece of a parsed format string.
 */
typedef struct format_part_s {
	/** Discriminator selecting the active union field. */
	format_part_type_e type;
	union {
		/** Non-conversion text segment from the format string. */
		format_text_span_t literal;
		/** Conversion directive metadata. */
		conversion_specifier_t conversion;
	};
} format_part_t;

/**
 * @brief Parses a format string into an array of format_part_t structures.
 *
 * @param format The format string to parse. It should be a valid format string
 * containing ordinary characters and conversion specifications.
 *
 * @param ap A va_list of arguments that use to fill conversion_specifier value.
 * The number and types of these arguments should match the conversion
 * specifications in the format string, especially for '*' width/precision
 * specifiers.
 *
 * @return format_part_t* A pointer to a dynamically allocated array of
 * format_part_t structures representing the parsed format string. The caller is
 * responsible for freeing this memory when it is no longer needed.
 *
 * @note The behavior of this function is undefined if the format string is not
 * a valid format or if the va_list does not contain the expected arguments for
 * '*' specifiers.
 */
format_part_t *parse_format_string(const char *format, va_list ap);