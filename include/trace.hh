#ifndef trace_hh
#define trace_hh

#include <string>
#include <iostream>
#include <iomanip>
#include <cstdarg>

using std::cout;
using std::string;

/* vt100 stuff */

static __attribute__((unused)) const char *mesg = {"\x1b[1;35m"};
static __attribute__((unused)) const char *imp = {"\x1b[1;33m"};
static __attribute__((unused)) const char *dbg = {"\x1b[1;37m"};

static __attribute__((unused)) const char *err = {"\x1b[1;31m+++err: "};
static __attribute__((unused)) const char *rst = {"\x1b[0m"};

static const char progress[5] = "|/-\\";

namespace vt100 {
static inline void cursor_off()
{
	printf("\33[?25l");
}
static inline void cursor_on()
{
	printf("\33[?25h");
}
};

struct trace {
	trace () {}
	static trace & get () {
		static trace t;
		return t;
	}

	bool verbose{};

	void update_progress(int count)
	{
		write("\x1b[93m\33[2K\r%c", progress[count % 4]);
	}

	void end_progress()
	{
		write("\33[2K\r");
	}

	void write(const char* format, ...)
	{
		va_list argp;
		va_start(argp, format);

		cout << mesg;
		vprintf(format, argp);
		cout << std::flush << rst;
		va_end(argp);
	}

	void print(const char* format, ...)
	{
		va_list argp;
		va_start(argp, format);

		printf(dbg);
		vprintf(format, argp);
		printf(rst);

		va_end(argp);
	}

	trace & operator << (const string &s)
	{
		if (verbose) {
			cout << s;

			if (s[s.size() - 1] == '\n')
				cout << rst;
		}
		return *this;
	}
	trace & operator << (int i)
	{
		if (verbose)
			cout << i;

		return *this;
	}
	trace & operator << (long int i)
	{
		if (verbose)
			cout << i;

		return *this;
	}
	trace & operator << (double i)
	{
		if (verbose)
			cout << i;

		return *this;
	}
	trace & operator << (long double i)
	{
		if (verbose)
			cout << i;

		return *this;
	}
};

struct nodbg {
	nodbg () {}
	static nodbg & get () {
		static nodbg e;
		return e;
	}

	nodbg & operator << (const string &s)
	{
		cout << s;

		if (s[s.size() - 1] == '\n')
			cout << rst;

		return *this;
	}
	nodbg & operator << (int i)
	{ cout << i; return *this; }
	nodbg & operator << (long int i)
	{ cout << i; return *this; }
	nodbg & operator << (double i)
	{ cout << i; return *this; }
	nodbg & operator << (long double i)
	{ cout << i; return *this; }
};

#define mesg nodbg::get() << mesg
#define imp nodbg::get() << imp
#define _err nodbg::get() << err

#define dbg trace::get() << dbg

#endif /* trace_hh */
