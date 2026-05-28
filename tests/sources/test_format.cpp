#include <my_printf/format.h>

#include <cstdarg>

#include "test_format.hpp"

static format_part_t **parse_format(const char *format, ...)
{
	va_list ap			  = { 0 };
	format_part_t **parts = NULL;

	va_start(ap, format);
	parts = initialize_format_parts(format, ap);
	va_end(ap);
	return parts;
}

TEST_F(TestFormat, Blank)
{
	format_part_t **parts = parse_format("Hello %d");

	ASSERT_NE(parts, nullptr);
	ASSERT_NE(parts[0], nullptr);
	ASSERT_NE(parts[1], nullptr);
	ASSERT_EQ(parts[2], nullptr);

	EXPECT_EQ(parts[0]->type, FORMAT_PART_TYPE_LITERAL);
	EXPECT_STREQ(parts[0]->literal.data, "Hello ");

	EXPECT_EQ(parts[1]->type, FORMAT_PART_TYPE_CONVERSION);
	EXPECT_EQ(parts[1]->conversion.argument_index, 0u);
	EXPECT_EQ(parts[1]->conversion.flags, 0u);
	EXPECT_EQ(parts[1]->conversion.vector_separator, '\0');
	EXPECT_EQ(parts[1]->conversion.width.type, FORMAT_ARG_VALUE_NONE);
	EXPECT_EQ(parts[1]->conversion.precision.type, FORMAT_ARG_VALUE_NONE);
	EXPECT_EQ(parts[1]->conversion.length, FORMAT_LENGTH_NONE);
	EXPECT_EQ(parts[1]->conversion.type, CONVERSION_TYPE_D);

	destroy_format_parts(parts);
}

TEST_F(TestFormat, ParsesFlagsWidthPrecisionAndLength)
{
	format_part_t **parts = parse_format("%#0-+ 12.3lld");

	ASSERT_NE(parts, nullptr);
	ASSERT_NE(parts[0], nullptr);
	ASSERT_EQ(parts[1], nullptr);

	EXPECT_EQ(parts[0]->type, FORMAT_PART_TYPE_CONVERSION);
	EXPECT_EQ(parts[0]->conversion.flags,
			  FORMAT_FLAG_ALTERNATE | FORMAT_FLAG_ZERO_PAD
				  | FORMAT_FLAG_LEFT_ADJUST | FORMAT_FLAG_SPACE_SIGN
				  | FORMAT_FLAG_FORCE_SIGN);
	EXPECT_EQ(parts[0]->conversion.width.type, FORMAT_ARG_VALUE_LITERAL);
	EXPECT_EQ(parts[0]->conversion.width.value, 12);
	EXPECT_EQ(parts[0]->conversion.precision.type, FORMAT_ARG_VALUE_LITERAL);
	EXPECT_EQ(parts[0]->conversion.precision.value, 3);
	EXPECT_EQ(parts[0]->conversion.length, FORMAT_LENGTH_LL);
	EXPECT_EQ(parts[0]->conversion.type, CONVERSION_TYPE_D);

	destroy_format_parts(parts);
}

TEST_F(TestFormat, ParsesLiteralAroundConversion)
{
	format_part_t **parts = parse_format("start %x end");

	ASSERT_NE(parts, nullptr);
	ASSERT_NE(parts[0], nullptr);
	ASSERT_NE(parts[1], nullptr);
	ASSERT_NE(parts[2], nullptr);
	ASSERT_EQ(parts[3], nullptr);

	EXPECT_EQ(parts[0]->type, FORMAT_PART_TYPE_LITERAL);
	EXPECT_STREQ(parts[0]->literal.data, "start ");

	EXPECT_EQ(parts[1]->type, FORMAT_PART_TYPE_CONVERSION);
	EXPECT_EQ(parts[1]->conversion.type, CONVERSION_TYPE_X);

	EXPECT_EQ(parts[2]->type, FORMAT_PART_TYPE_LITERAL);
	EXPECT_STREQ(parts[2]->literal.data, " end");

	destroy_format_parts(parts);
}