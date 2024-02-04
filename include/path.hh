#ifndef path_hh
#define path_hh

#include "fs.hh"

#include <string>
#include <filesystem>

using namespace std;

struct path : public fs {
	path();
	path(const string &);

	void update_path(const string &name);

	string get_home_path() { return user_home; }
	string get_cur_path() { return fs_path; }
	string get_cur_folder();
	string ls_normalize(const string &path);
	string ls_real(const string &path);
	string delimit_if_spaces(const string &path);
	bool remove_folder(const string &path);

protected:
	string user_home;
	filesystem::path fs_path;
};

#endif /* path_hh */
