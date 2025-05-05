/*
 * Copyright (C) 2024 Angelo Dureghello <angelo@kernel-space.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "config.hh"
#include "path.hh"
#include "fs.hh"
#include "utils.hh"
#include "sections.hh"
#include "trace.hh"

static constexpr char file_cfg_name[] = "/.sferc";

static constexpr int def_app_width = 800;
static constexpr int def_app_height = 600;
static constexpr int def_app_first_x = 20;
static constexpr int def_app_first_y = 20;

using std::string;

config::config()
{
	fs fs;

	fs.setup_config_paths();

	path_resources = fs.get_path_res();

	config_init(&cfg);

	root = config_root_setting(&cfg);
	if (!root) {
		_err << "config(), root is null\n";
	}
}

config::~config()
{
	config_destroy(&cfg);
}

void config::setup_defaults()
{
	add_option("application", "width", def_app_width);
	add_option("application", "height", def_app_height);

	string name;
	int i;

	for (i = 0; i < s_id_max; ++i) {
		name = "feature_" + tools::itoa(i);
		create_group("sections", name.c_str());
	}

	name = "sections.feature_" + tools::itoa(s_id_azienda);
	add_option(name.c_str(), "icon", "icons8-user-folder-96.png");
	add_option(name.c_str(), "icon_text", "Dati aziendali");
	add_option(name.c_str(), "icon_x", 40);
	add_option(name.c_str(), "icon_y", 20);
	name = "sections.feature_" + tools::itoa(s_id_ana_cli);
	add_option(name.c_str(), "icon", "icons8-books-folder-96.png");
	add_option(name.c_str(), "icon_text", "Anagrafica clienti");
	add_option(name.c_str(), "icon_x", 200);
	add_option(name.c_str(), "icon_y", 20);
	name = "sections.feature_" + tools::itoa(s_id_em_fatt);
	add_option(name.c_str(), "icon", "icons8-documents-folder-96.png");
	add_option(name.c_str(), "icon_text", "Emissione fatture");
	add_option(name.c_str(), "icon_x", 360);
	add_option(name.c_str(), "icon_y", 20);
	name = "sections.feature_" + tools::itoa(s_id_config);
	add_option(name.c_str(), "icon", "icons8-services-96.png");
	add_option(name.c_str(), "icon_text", "Configurazione");
	add_option(name.c_str(), "icon_x", 520);
	add_option(name.c_str(), "icon_y", 20);
	name = "sections.feature_" + tools::itoa(s_id_help);
	add_option(name.c_str(), "icon", "icons8-help-96.png");
	add_option(name.c_str(), "icon_text", "Help");
	add_option(name.c_str(), "icon_x", 40);
	add_option(name.c_str(), "icon_y", 180);

	add_option("latex", "invoice_title", "Fattura ordinaria");

	if (!save_config())
		exit(-1);
}

bool config::load_config()
{
	path p;
        string default_config = p.get_home_path() + file_cfg_name;

	if(!config_read_file(&cfg, default_config.c_str())) {
		return false;
	}

	root = config_root_setting(&cfg);

	return true;
}

config_setting_t* config::create_group(const char *sect, const char *group)
{
	config_setting_t *sel;

	sel = config_lookup(&cfg, sect);
	if (!sel) {
		sel = config_setting_add(root, sect, CONFIG_TYPE_GROUP);
		if (!sel) {
			_err << "add_option(), sect " << sect << "\n";
			return 0;
		}
	}

	if (group) {
		sel = config_setting_add(sel, group, CONFIG_TYPE_GROUP);
		if (!sel) {
			_err << "add_option(), group " << sect << "\n";
			return 0;
		}
	}

	return sel;
}

config_setting_t* config::get_group(const char *group)
{
	return config_lookup(&cfg, group);
}

bool config::add_option(const char *sect, const char *name, const char *value)
{
	config_setting_t *group, *setting;

	group = get_group(sect);
	if (!group)
		group = create_group(sect);

	setting = config_setting_add(group, name, CONFIG_TYPE_STRING);
	if (!setting) {
		_err << "add_option(), name " << name << "\n";
		return false;
	}

	config_setting_set_string(setting, value);

	return true;
}

bool config::add_option(const char *sect, const char *name, int value)
{
	config_setting_t *group, *setting;

	group = get_group(sect);
	if (!group)
		group = create_group(sect);

	setting = config_setting_add(group, name, CONFIG_TYPE_INT);
	if (!setting) {
		_err << "add_option(), name " << name << "\n";
		return false;
	}

	config_setting_set_int(setting, value);

	return true;
}

bool config::add_option(const char *sect, const char *name, int* array, int len)
{
	config_setting_t *group, *setting;
	int i;

	group = get_group(sect);
	if (!group)
		group = create_group(sect);

	setting = config_setting_add(group, name, CONFIG_TYPE_ARRAY);
	if (!setting) {
		_err << "add_option(), name " << name << "\n";
		return false;
	}

	for (i = 0; i < len; ++i)
		config_setting_set_int_elem(setting, i, array[i]);

	return true;
}

bool config::update_option(
	const char *sect, const char *name, const char *value)
{
	config_setting_t *group;
	int rval;
	string path;

	path = sect;
	path += ".";
	path += name;

	group = get_group(path.c_str());
	if (!group) {
		_err << "missing group\n";
		return false;
	}

	rval = config_setting_set_string(group, value);
	if (rval != CONFIG_TRUE) {
		_err << "update_option(), name " << name << "\n";
		return false;
	}

	return true;
}

bool config::update_option(const char *sect, const char *name, int value)
{
	config_setting_t *group;
	int rval;
	string path;

	path = sect;
	path += ".";
	path += name;

	group = get_group(path.c_str());

	rval = config_setting_set_int(group, value);
	if (rval != CONFIG_TRUE) {
		_err << "update_option(), name " << name << "\n";
		return false;
	}

	return true;
}

int config::get_int(const char *path)
{
	int rval;

	if (config_lookup_int(&cfg, path, &rval) == CONFIG_TRUE)
		return rval;

	return -1;
}

string config::get_string(const char *path)
{
	const char *buff;

	if (config_lookup_string(&cfg, path, (const char **)&buff)
		== CONFIG_TRUE)
		return buff;

	return "";
}

vect_str config::get_string_list(const char *sect)
{
	config_setting_t *group;
	vect_str rvalues;
	const char *location;
	int i, count;

	rvalues.clear();

	group = get_group(sect);
	if (!group)
		_err << "get_string_list()\n";

	count = config_setting_length(group);
	for(i = 0; i < count; ++i) {
		location = config_setting_get_string_elem(group, i);
		rvalues.push_back(location);
	}

	return rvalues;
}

bool config::save_config()
{
	path p;
	string default_config = p.get_home_path() + file_cfg_name;

	if(!config_write_file(&cfg, default_config.c_str())) {
		_err << "cannot write config file " << default_config << "\n";
		config_destroy(&cfg);

		return false;
	}

	return true;
}
