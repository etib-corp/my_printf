#include <my_printf/my_printf.h>
#include <unistd.h>
#include <string.h>

#include <chrono>
#include <functional>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

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

static double benchmark(const std::function<void()> &fn, int iterations)
{
	using namespace std::chrono;
	auto t0 = high_resolution_clock::now();
	for (int i = 0; i < iterations; ++i)
		fn();
	auto t1 = high_resolution_clock::now();
	return duration_cast<duration<double, std::micro>>(t1 - t0).count();
}

TEST_F(TestMyPrintf, CompareSprintfAndTime)
{
	char buf_my[128] = {0};
	char buf_lib[128] = {0};

	int r_my  = my_sprintf(buf_my, "Hello %s %d", "X", 42);
	int r_lib = sprintf(buf_lib, "Hello %s %d", "X", 42);
	EXPECT_EQ(r_my, r_lib);
	EXPECT_STREQ(buf_my, buf_lib);

	const int ITERS = 100000;
	double t_my  = benchmark([&]{ my_sprintf(buf_my, "Hello %s %d", "X", 42); }, ITERS);
	double t_lib = benchmark([&]{ sprintf(buf_lib, "Hello %s %d", "X", 42); }, ITERS);

	printf("sprintf benchmark (total microseconds for %d iters): my=%.3f libc=%.3f\n", ITERS, t_my, t_lib);
}

TEST_F(TestMyPrintf, CompareSnprintfAndTime)
{
	char buf_my[8];
	char buf_lib[8];

	memset(buf_my, 0, sizeof(buf_my));
	memset(buf_lib, 0, sizeof(buf_lib));
	int r_my  = my_snprintf(buf_my, sizeof(buf_my), "ABCDEFG");
	int r_lib = snprintf(buf_lib, sizeof(buf_lib), "ABCDEFG");
	EXPECT_EQ(r_my, r_lib);
	EXPECT_STREQ(buf_my, buf_lib);

	const int ITERS = 100000;
	double t_my  = benchmark([&]{ my_snprintf(buf_my, sizeof(buf_my), "ABCDEFG"); }, ITERS);
	double t_lib = benchmark([&]{ snprintf(buf_lib, sizeof(buf_lib), "ABCDEFG"); }, ITERS);

	printf("snprintf benchmark (total microseconds for %d iters): my=%.3f libc=%.3f\n", ITERS, t_my, t_lib);
}

TEST_F(TestMyPrintf, CompareAsprintfAndTime)
{
	char *s_my = NULL;
	char *s_lib = NULL;

	int r_my  = my_asprintf(&s_my, "%s-%d", "abc", 123);
	int r_lib = asprintf(&s_lib, "%s-%d", "abc", 123);
	ASSERT_NE(s_my, nullptr);
	ASSERT_NE(s_lib, nullptr);
	EXPECT_EQ(r_my, r_lib);
	EXPECT_STREQ(s_my, s_lib);
	free(s_my);
	free(s_lib);

	const int ITERS = 10000; // allocations are heavier
	double t_my  = benchmark([&]{ char *t = NULL; my_asprintf(&t, "%s-%d", "abc", 123); free(t); }, ITERS);
	double t_lib = benchmark([&]{ char *t = NULL; asprintf(&t, "%s-%d", "abc", 123); free(t); }, ITERS);

	printf("asprintf benchmark (total microseconds for %d iters): my=%.3f libc=%.3f\n", ITERS, t_my, t_lib);
}

TEST_F(TestMyPrintf, CompareDprintfAndTime)
{
	int fd = open("/dev/null", O_WRONLY);
	ASSERT_GE(fd, 0);

	int r_my  = my_dprintf(fd, "X=%d", 7);
	int r_lib = dprintf(fd, "X=%d", 7);
	EXPECT_EQ(r_my, r_lib);

	const int ITERS = 10000;
	double t_my  = benchmark([&]{ my_dprintf(fd, "X=%d", 7); }, ITERS);
	double t_lib = benchmark([&]{ dprintf(fd, "X=%d", 7); }, ITERS);

	printf("dprintf benchmark (total microseconds for %d iters): my=%.3f libc=%.3f\n", ITERS, t_my, t_lib);

	close(fd);
}
