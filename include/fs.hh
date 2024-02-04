#ifndef fs_hh
#define fs_hh

#include <string>
#include <vector>

using std::string;
using std::vector;

struct fs {
	fs();
	string get_user_home();
	bool is_dir(const char *name);
	vector<string> dir(const char *path);
};

#endif /* fs_hh */
