#include <my_printf/convert.h>

#include "test_convert.hpp"

TEST_F(TestConvert, Blank)
{
	EXPECT_TRUE(true);
}

TEST_F(TestConvert, ConvertStringAndUpper)
{
	conversion_specifier_t spec = {0};
	spec.value.string_value = (char *)"hello";
	char *out = convert_s(&spec);
	ASSERT_NE(out, nullptr);
	EXPECT_STREQ(out, "hello");
	free(out);

	out = convert_s_upper(&spec);
	ASSERT_NE(out, nullptr);
	EXPECT_STREQ(out, "HELLO");
	free(out);
}

TEST_F(TestConvert, ConvertCharAndPercent)
{
	conversion_specifier_t spec = {0};
	spec.value.char_value = 'Z';
	char *out = convert_c(&spec);
	ASSERT_NE(out, nullptr);
	EXPECT_EQ(out[0], 'Z');
	free(out);

	out = convert_percent(&spec);
	ASSERT_NE(out, nullptr);
	EXPECT_STREQ(out, "%");
	free(out);
}

TEST_F(TestConvert, ConvertIntegersHexOctalUnsigned)
{
	conversion_specifier_t spec = {0};
	spec.value.int_value = 1234;
	char *out = convert_d(&spec);
	ASSERT_NE(out, nullptr);
	EXPECT_STREQ(out, "1234");
	free(out);

	spec.value.int_value = 0x2a;
	out = convert_x(&spec);
	ASSERT_NE(out, nullptr);
	EXPECT_STREQ(out, "2a");
	free(out);

	spec.value.int_value = 0777;
	out = convert_o(&spec);
	ASSERT_NE(out, nullptr);
	EXPECT_STREQ(out, "777");
	free(out);

	spec.value.int_value = -1;
	out = convert_u(&spec);
	ASSERT_NE(out, nullptr);
	/* unsigned representation of -1 should be a large number (implementation-defined width); ensure non-empty */
	EXPECT_STRNE(out, "");
	free(out);
}

TEST_F(TestConvert, ConvertPointerAndN)
{
	int x = 42;
	conversion_specifier_t spec = {0};
	spec.value.pointer_value = &x;
	char *out = convert_p(&spec);
	ASSERT_NE(out, nullptr);
	EXPECT_TRUE(out[0] == '0' && out[1] == 'x');
	free(out);

	int n = 123;
	spec.value.pointer_value = &n;
	out = convert_n(&spec);
	ASSERT_NE(out, nullptr);
	EXPECT_STREQ(out, "");
	EXPECT_EQ(n, 0);
	free(out);
}

TEST_F(TestConvert, ConvertFloat)
{
	conversion_specifier_t spec = {0};
	spec.value.double_value = 3.14;
	char *out = convert_f(&spec);
	ASSERT_NE(out, nullptr);
	EXPECT_STREQ(out, "3.140000");
	free(out);
}