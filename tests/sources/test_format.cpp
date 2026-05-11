#include <my_printf/format.h>

#include "test_format.hpp"

TEST_F(TestFormat, Blank)
{
    const char *format = "Hello, %s! You have %d new messages.";

	format_part_t **parts =
		initialize_format_parts(format, (va_list) { 0 });
	format_part_t **iter = parts;

    std::cout << "Testing format string: " << format << std::endl;
	while (*iter) {
		std::cout << "Part type: "
				  << ((*iter)->type == FORMAT_PART_TYPE_LITERAL ? "Literal"
																: "Conversion")
				  << std::endl;
		if ((*iter)->type == FORMAT_PART_TYPE_LITERAL) {
			std::cout << "Literal data: "
					  << std::string((*iter)->literal.data,
									 (*iter)->literal.length)
					  << std::endl;
		}
		iter++;
	}

	destroy_format_parts(parts);
}