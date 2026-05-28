#include <my_printf/my_printf.h>

int main(void)
{
	my_printf("Hello, World!\n");
	my_printf("This is a simple print example using my_printf.\n");
	my_printf("The value of PI is approximately: %.2f\n", 3.14159);
	my_printf("Hexadecimal representation of 255 is: %x\n", 255);
	my_printf("Octal representation of 255 is: %o\n", 255);
	return 0;
}