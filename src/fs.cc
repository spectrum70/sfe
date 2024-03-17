/*
 * fet
 *
 * (C) 2017 Angelo Dureghello <angelo@sysam.it>
 *
 * This file is part of fet application.
 *
 * fet app. is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * fet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with fet.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "fs.hh"

#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <cstring>

fs::fs()
{
}

string fs::get_user_home()
{
	struct passwd *pw = getpwuid(getuid());

	return pw->pw_dir;
}

bool fs::is_dir(const char *name)
{
	struct stat sb;

	if (stat(name, &sb) == 0) {
		if ((sb.st_mode & S_IFMT) == S_IFDIR)
			return true;
	}

	return false;
}

vector<string> fs::dir(const char *path)
{
	DIR *d;
	struct dirent *dir;
	vector<string> v;

	v.clear();

	d = opendir(path);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			v.push_back(dir->d_name);
		}
		closedir(d);
	}

	return v;
}

int fs::create_tmp_file(char *tmp_name, char *content)
{
	int temp = mkstemp(tmp_name);

	printf("writing on tmp file\n");

	if (temp) {
		write(temp, content, strlen(content));


		close(temp);

		return 0;
	}


	return -1;
}

