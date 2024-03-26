#ifndef config_hh
#define config_hh

#include <libconfig.h>
#include <vector>
#include <string>

using std::vector;
using std::string;

typedef vector<string> vect_str;

class config {
	config();
	~config();
public:
	static config &get() {
		static config c;
		return c;
	}

	bool add_option(const char *sect, const char *name, const char *value);
	bool add_option(const char *sect, const char *name, int value);
	bool add_option(
		const char *sect, const char *name, int* array, int len);
	bool update_option(
		const char *sect, const char *name, const char *value);
	bool update_option(
		const char *sect, const char *name, const int value);
	bool load_config();
	bool save_config();
	void setup_defaults();

	int get_int(const char *path);
	string get_string(const char *path);
	string get_path_res() { return path_resources; }

	vect_str get_string_list(const char *sect);

private:
	config_setting_t *get_group(const char *group);
	config_setting_t* create_group(
				const char *sect, const char *group = NULL);
private:
	config_t cfg;
	config_setting_t *root;
	string path_resources;
};

#endif /* config_hh */
