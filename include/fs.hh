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
	int create_tmp_file(char *tmp_name, char *content);
	size_t get_files_size(const char *path);
	bool load_to_memory(const char *path, string &mem);
	void setup_config_paths();
	const char *get_path_res();
	int create_directory(const char *path);
private:
	string path_resources;
};

#endif /* fs_hh */
