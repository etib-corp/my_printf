#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <ctype.h>
#include <math.h>

#include "my_printf/convert.h"
#include "my_printf/format.h"

const converter_t converters[CONVERSION_TYPE_COUNT] = {
	{ CONVERSION_TYPE_PERCENT, convert_percent },
	{ CONVERSION_TYPE_D, convert_d },
	{ CONVERSION_TYPE_I, convert_i },
	{ CONVERSION_TYPE_O, convert_o },
	{ CONVERSION_TYPE_U, convert_u },
	{ CONVERSION_TYPE_X, convert_x },
	{ CONVERSION_TYPE_X_UPPER, convert_x_upper },
	{ CONVERSION_TYPE_D_UPPER, convert_d_upper },
	{ CONVERSION_TYPE_O_UPPER, convert_o_upper },
	{ CONVERSION_TYPE_U_UPPER, convert_u_upper },
	{ CONVERSION_TYPE_E, convert_e },
	{ CONVERSION_TYPE_E_UPPER, convert_e_upper },
	{ CONVERSION_TYPE_F, convert_f },
	{ CONVERSION_TYPE_F_UPPER, convert_f_upper },
	{ CONVERSION_TYPE_G, convert_g },
	{ CONVERSION_TYPE_G_UPPER, convert_g_upper },
	{ CONVERSION_TYPE_A, convert_a },
	{ CONVERSION_TYPE_A_UPPER, convert_a_upper },
	{ CONVERSION_TYPE_C, convert_c },
	{ CONVERSION_TYPE_C_UPPER, convert_c_upper },
	{ CONVERSION_TYPE_S, convert_s },
	{ CONVERSION_TYPE_S_UPPER, convert_s_upper },
	{ CONVERSION_TYPE_P, convert_p },
	{ CONVERSION_TYPE_N, convert_n },
};

char *convert_percent(conversion_specifier_t *specifier)
{
	(void)specifier;
	char *result = calloc(2, sizeof(char));
	if (result == NULL) {
		return NULL;
	}
	result[0] = '%';
	return result;
}

/* Helper utilities: integer to string, apply width/precision, float formatting
 */

static char *uint_to_base(uintmax_t value, int base, int uppercase,
						  int min_digits)
{
	char buf[65];
	const char *digits_l = "0123456789abcdef";
	const char *digits_u = "0123456789ABCDEF";
	const char *digits	 = uppercase ? digits_u : digits_l;
	int pos				 = 0;
	if (value == 0) {
		buf[pos++] = '0';
	} else {
		while (value != 0 && pos < (int)sizeof(buf) - 1) {
			buf[pos++] = digits[value % (uintmax_t)base];
			value /= (uintmax_t)base;
		}
	}
	while (pos < min_digits)
		buf[pos++] = '0';
	buf[pos] = '\0';
	/* reverse */
	for (int i = 0, j = pos - 1; i < j; ++i, --j) {
		char t = buf[i];
		buf[i] = buf[j];
		buf[j] = t;
	}
	return strdup(buf);
}

static char *apply_width_and_flags(char *s, conversion_specifier_t *spec,
								   int negative)
{
	if (s == NULL || spec == NULL)
		return s;
	int width = 0;
	if (spec->width.type == FORMAT_ARG_VALUE_LITERAL)
		width = spec->width.value;
	int len	 = (int)strlen(s);
	int need = width > len ? width - len : 0;
	if (need <= 0)
		return s;
	char pad  = (spec->flags & FORMAT_FLAG_ZERO_PAD)
			&& !(spec->flags & FORMAT_FLAG_LEFT_ADJUST)
		? '0'
		: ' ';
	char *out = malloc((size_t)len + need + 1);
	if (out == NULL) {
		free(s);
		return NULL;
	}
	if (spec->flags & FORMAT_FLAG_LEFT_ADJUST) {
		memcpy(out, s, (size_t)len);
		memset(out + len, pad, (size_t)need);
	} else {
		memset(out, pad, (size_t)need);
		memcpy(out + need, s, (size_t)len);
	}
	out[len + need] = '\0';
	free(s);
	return out;
}

static char *format_signed(intmax_t val, int base, int uppercase,
						   conversion_specifier_t *spec)
{
	int negative   = val < 0;
	uintmax_t uval = negative ? (uintmax_t)(-val) : (uintmax_t)val;
	int prec	   = 0;
	if (spec->precision.type == FORMAT_ARG_VALUE_LITERAL)
		prec = spec->precision.value;
	char *digits = uint_to_base(uval, base, uppercase, prec);
	if (digits == NULL)
		return NULL;
	int prefix_len	   = 0;
	const char *prefix = "";
	if (negative) {
		prefix	   = "-";
		prefix_len = 1;
	} else if (spec->flags & FORMAT_FLAG_FORCE_SIGN) {
		prefix	   = "+";
		prefix_len = 1;
	} else if (spec->flags & FORMAT_FLAG_SPACE_SIGN) {
		prefix	   = " ";
		prefix_len = 1;
	}
	size_t out_len = strlen(prefix) + strlen(digits) + 1;
	char *out	   = malloc(out_len);
	if (out == NULL) {
		free(digits);
		return NULL;
	}
	memcpy(out, prefix, (size_t)prefix_len);
	memcpy(out + prefix_len, digits, strlen(digits) + 1);
	free(digits);
	out = apply_width_and_flags(out, spec, negative);
	return out;
}

static char *format_unsigned(uintmax_t val, int base, int uppercase,
							 conversion_specifier_t *spec, int add_prefix)
{
	int prec = 0;
	if (spec->precision.type == FORMAT_ARG_VALUE_LITERAL)
		prec = spec->precision.value;
	char *digits = uint_to_base(val, base, uppercase, prec);
	if (digits == NULL)
		return NULL;
	const char *prefix = "";
	char *out		   = NULL;
	if (add_prefix && val != 0) {
		if (base == 16)
			prefix = uppercase ? "0X" : "0x";
		else if (base == 8)
			prefix = "0";
	}
	size_t out_len = strlen(prefix) + strlen(digits) + 1;
	out			   = malloc(out_len);
	if (out == NULL) {
		free(digits);
		return NULL;
	}
	memcpy(out, prefix, strlen(prefix));
	memcpy(out + strlen(prefix), digits, strlen(digits) + 1);
	free(digits);
	out = apply_width_and_flags(out, spec, 0);
	return out;
}

static char *format_double_f(double value, conversion_specifier_t *spec,
							 char conv)
{
	if (isnan(value))
		return strdup("nan");
	if (isinf(value))
		return strdup(value < 0 ? "-inf" : "inf");
	int prec = 6;
	if (spec->precision.type == FORMAT_ARG_VALUE_LITERAL)
		prec = spec->precision.value;
	int negative	  = signbit(value);
	double av		  = negative ? -value : value;
	uintmax_t intpart = (uintmax_t)av;
	double frac		  = av - (double)intpart;
	/* scale fractional and round */
	double scale = 1.0;
	for (int i = 0; i < prec; ++i)
		scale *= 10.0;
	uintmax_t frac_scaled = (uintmax_t)(frac * scale + 0.5);
	/* handle carry */
	if (frac_scaled >= (uintmax_t)scale) {
		intpart += 1;
		frac_scaled -= (uintmax_t)scale;
	}
	char *intstr = uint_to_base(intpart, 10, 0, 0);
	if (intstr == NULL)
		return NULL;
	char *fracstr = NULL;
	if (prec > 0) {
		fracstr = uint_to_base(frac_scaled, 10, 0, prec);
		if (fracstr == NULL) {
			free(intstr);
			return NULL;
		}
	}
	size_t len = strlen(intstr) + (prec > 0 ? 1 + strlen(fracstr) : 0) + 2;
	if (negative || (spec->flags & FORMAT_FLAG_FORCE_SIGN)
		|| (spec->flags & FORMAT_FLAG_SPACE_SIGN))
		len += 1;
	char *out = malloc(len);
	if (out == NULL) {
		free(intstr);
		free(fracstr);
		return NULL;
	}
	char *p = out;
	if (negative)
		*p++ = '-';
	else if (spec->flags & FORMAT_FLAG_FORCE_SIGN)
		*p++ = '+';
	else if (spec->flags & FORMAT_FLAG_SPACE_SIGN)
		*p++ = ' ';
	memcpy(p, intstr, strlen(intstr));
	p += strlen(intstr);
	if (prec > 0) {
		*p++ = '.';
		memcpy(p, fracstr, strlen(fracstr));
		p += strlen(fracstr);
	}
	*p = '\0';
	free(intstr);
	free(fracstr);
	out = apply_width_and_flags(out, spec, negative);
	return out;
}

static char *format_double_e(double value, conversion_specifier_t *spec,
							 char conv)
{
	if (isnan(value))
		return strdup("nan");
	if (isinf(value))
		return strdup(value < 0 ? "-inf" : "inf");
	int prec = 6;
	if (spec->precision.type == FORMAT_ARG_VALUE_LITERAL)
		prec = spec->precision.value;
	int negative = signbit(value);
	double av	 = negative ? -value : value;
	int exp		 = 0;
	if (av != 0.0) {
		exp = (int)floor(log10(av));
		av	= av / pow(10.0, exp);
		/* normalize to [1,10) */
		if (av < 1.0) {
			av *= 10.0;
			exp -= 1;
		} else if (av >= 10.0) {
			av /= 10.0;
			exp += 1;
		}
	}
	/* format leading digit and fractional */
	uintmax_t lead = (uintmax_t)av;
	double frac	   = av - (double)lead;
	double scale   = 1.0;
	for (int i = 0; i < prec; ++i)
		scale *= 10.0;
	uintmax_t frac_scaled = (uintmax_t)(frac * scale + 0.5);
	if (frac_scaled >= (uintmax_t)scale) {
		lead += 1;
		frac_scaled -= (uintmax_t)scale;
		if (lead >= 10) {
			lead = 1;
			exp += 1;
		}
	}
	char *leadstr = uint_to_base(lead, 10, 0, 0);
	char *fracstr = NULL;
	if (prec > 0)
		fracstr = uint_to_base(frac_scaled, 10, 0, prec);
	if (leadstr == NULL || (prec > 0 && fracstr == NULL)) {
		free(leadstr);
		free(fracstr);
		return NULL;
	}
	/* build exponent manually: sign + at least two digits */
	char expbuf[32];
	int eabs	  = exp < 0 ? -exp : exp;
	int idx		  = 0;
	expbuf[idx++] = (exp < 0) ? '-' : '+';
	/* produce digits for eabs, ensure at least two digits with leading zero */
	char rev[32];
	int rpos = 0;
	if (eabs == 0)
		rev[rpos++] = '0';
	while (eabs > 0 && rpos < (int)sizeof(rev) - 1) {
		rev[rpos++] = '0' + (eabs % 10);
		eabs /= 10;
	}
	if (rpos == 1) { /* ensure at least two digits */
		rev[rpos++] = '0';
	}
	/* reverse into expbuf */
	for (int i = rpos - 1; i >= 0; --i)
		expbuf[idx++] = rev[i];
	expbuf[idx] = '\0';
	size_t len	= (negative ? 1 : 0) + strlen(leadstr)
		+ (prec > 0 ? 1 + strlen(fracstr) : 0) + 2 + strlen(expbuf) + 1;
	char *out = malloc(len);
	if (out == NULL) {
		free(leadstr);
		free(fracstr);
		return NULL;
	}
	char *p = out;
	if (negative)
		*p++ = '-';
	memcpy(p, leadstr, strlen(leadstr));
	p += strlen(leadstr);
	if (prec > 0) {
		*p++ = '.';
		memcpy(p, fracstr, strlen(fracstr));
		p += strlen(fracstr);
	}
	*p++ = conv; /* 'e' or 'E' */
	memcpy(p, expbuf, strlen(expbuf));
	p += strlen(expbuf);
	*p = '\0';
	free(leadstr);
	free(fracstr);
	out = apply_width_and_flags(out, spec, negative);
	return out;
}

char *convert_d(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	return format_signed((intmax_t)specifier->value.int_value, 10, 0,
						 specifier);
}

char *convert_i(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	return format_signed((intmax_t)specifier->value.int_value, 10, 0,
						 specifier);
}

char *convert_o(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	int add_prefix = (specifier->flags & FORMAT_FLAG_ALTERNATE) ? 1 : 0;
	return format_unsigned((uintmax_t)specifier->value.int_value, 8, 0,
						   specifier, add_prefix);
}

char *convert_u(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	return format_unsigned((uintmax_t)specifier->value.int_value, 10, 0,
						   specifier, 0);
}

char *convert_x(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	int add_prefix = (specifier->flags & FORMAT_FLAG_ALTERNATE) ? 1 : 0;
	return format_unsigned((uintmax_t)specifier->value.int_value, 16, 0,
						   specifier, add_prefix);
}

char *convert_x_upper(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	int add_prefix = (specifier->flags & FORMAT_FLAG_ALTERNATE) ? 1 : 0;
	return format_unsigned((uintmax_t)specifier->value.int_value, 16, 1,
						   specifier, add_prefix);
}

char *convert_d_upper(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	return format_signed((intmax_t)specifier->value.int_value, 10, 0,
						 specifier);
}

char *convert_o_upper(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	int add_prefix = (specifier->flags & FORMAT_FLAG_ALTERNATE) ? 1 : 0;
	return format_unsigned((uintmax_t)specifier->value.int_value, 8, 0,
						   specifier, add_prefix);
}

char *convert_u_upper(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	return format_unsigned((uintmax_t)specifier->value.int_value, 10, 0,
						   specifier, 0);
}

char *convert_e(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	return format_double_e(specifier->value.double_value, specifier, 'e');
}

char *convert_e_upper(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	return format_double_e(specifier->value.double_value, specifier, 'E');
}

char *convert_f(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	return format_double_f(specifier->value.double_value, specifier, 'f');
}

char *convert_f_upper(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	return format_double_f(specifier->value.double_value, specifier, 'F');
}

char *convert_g(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	double v = specifier->value.double_value;
	if (v == 0.0)
		return format_double_f(v, specifier, 'g');
	int prec = 6;
	if (specifier->precision.type == FORMAT_ARG_VALUE_LITERAL)
		prec = specifier->precision.value;
	int exp = (int)floor(log10(fabs(v)));
	if (exp < -4 || exp >= prec)
		return format_double_e(v, specifier, 'e');
	return format_double_f(v, specifier, 'g');
}

char *convert_g_upper(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	double v = specifier->value.double_value;
	if (v == 0.0)
		return format_double_f(v, specifier, 'G');
	int prec = 6;
	if (specifier->precision.type == FORMAT_ARG_VALUE_LITERAL)
		prec = specifier->precision.value;
	int exp = (int)floor(log10(fabs(v)));
	if (exp < -4 || exp >= prec)
		return format_double_e(v, specifier, 'E');
	return format_double_f(v, specifier, 'G');
}

char *convert_a(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	/* hex-floating not implemented; fall back to scientific */
	return format_double_e(specifier->value.double_value, specifier, 'a');
}

char *convert_a_upper(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	return format_double_e(specifier->value.double_value, specifier, 'A');
}

char *convert_c(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	char *result = calloc(2, sizeof(char));
	if (result == NULL)
		return NULL;
	result[0] = specifier->value.char_value;
	return result;
}

char *convert_c_upper(conversion_specifier_t *specifier)
{
	return convert_c(specifier);
}

char *convert_s(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	if (specifier->value.string_value == NULL)
		return strdup("(null)");
	return strdup(specifier->value.string_value);
}

char *convert_s_upper(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	char *src = specifier->value.string_value;
	if (src == NULL)
		src = "(null)";
	char *out = strdup(src);
	if (out == NULL)
		return NULL;
	for (char *p = out; *p != '\0'; ++p)
		*p = (char)toupper((unsigned char)*p);
	return out;
}

char *convert_p(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	uintptr_t ptr = (uintptr_t)specifier->value.pointer_value;
	char *digits  = uint_to_base((uintmax_t)ptr, 16, 0, 0);
	if (digits == NULL)
		return NULL;
	size_t len = 2 + strlen(digits) + 1;
	char *out  = malloc(len);
	if (out == NULL) {
		free(digits);
		return NULL;
	}
	out[0] = '0';
	out[1] = 'x';
	memcpy(out + 2, digits, strlen(digits) + 1);
	free(digits);
	out = apply_width_and_flags(out, specifier, 0);
	return out;
}

char *convert_n(conversion_specifier_t *specifier)
{
	if (specifier == NULL)
		return NULL;
	/* %n does not produce output; attempt to write zero to provided int
	 * pointer. */
	if (specifier->value.pointer_value != NULL) {
		int *p = (int *)specifier->value.pointer_value;
		*p	   = 0;
	}
	return strdup("");
}

char *convert(conversion_specifier_t *specifier)
{
	for (size_t i = 0; i < CONVERSION_TYPE_COUNT; ++i) {
		if (converters[i].type == specifier->type) {
			return converters[i].function(specifier);
		}
	}
	return NULL;
}