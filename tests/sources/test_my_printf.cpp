#include <my_printf/my_printf.h>
#include <unistd.h>
#include <string.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#define pipe(fds) _pipe((fds), 4096, _O_BINARY)
#endif

#include "test_my_printf.hpp"

TEST_F(TestMyPrintf, Blank)
{
	EXPECT_TRUE(true);
}

TEST_F(TestMyPrintf, SprintfSimple)
{
	char buf[64] = { 0 };
	int r		 = my_sprintf(buf, "Hello %s %d", "X", 42);
	EXPECT_EQ(r, (int)strlen(buf));
	EXPECT_STREQ(buf, "Hello X 42");
}

TEST_F(TestMyPrintf, SnprintfTruncation)
{
	char buf[5];
	memset(buf, 0, sizeof(buf));
	int r = my_snprintf(buf, sizeof(buf), "ABCDEFG");
	EXPECT_EQ(r, 7);			  // intended length
	EXPECT_STREQ(buf, "ABCD");	  // truncated to size-1
}

TEST_F(TestMyPrintf, AsprintfAllocates)
{
	char *s = NULL;
	int r	= my_asprintf(&s, "%s-%d", "abc", 123);
	ASSERT_NE(s, nullptr);
	EXPECT_EQ(r, (int)strlen(s));
	EXPECT_STREQ(s, "abc-123");
	free(s);
}

TEST_F(TestMyPrintf, DprintfToPipe)
{
	int fds[2];
	ASSERT_EQ(pipe(fds), 0);
	int r = my_dprintf(fds[1], "X=%d", 7);
	EXPECT_EQ(r, 3);
	close(fds[1]);
	char buf[16] = { 0 };
	ssize_t n	 = read(fds[0], buf, sizeof(buf) - 1);
	EXPECT_GT(n, 0);
	EXPECT_STREQ(buf, "X=7");
	close(fds[0]);
}
