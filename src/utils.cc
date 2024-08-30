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

#include "utils.hh"
#include "trace.hh"

#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

namespace tools {

int atoi(const string &str)
{
	int value;
	stringstream ss;

	ss << str;
	ss >> value;

	return value;
}

float atof(const string &str)
{
	float value;
	stringstream ss;

	ss << str;
	ss >> fixed >> setprecision(2) >> value;

	return value;
}

string itoa(int val)
{
	string out;
	stringstream ss;

	ss << val;
	ss >> out;

	return out;
}

string ftoa(float val)
{
	char value[64];

	sprintf(value, "%.2f", val);

	return value;
}

string uppercase(const string &str)
{
	string rval = str;

	transform(rval.begin(), rval.end(), rval.begin(), ::toupper);

	return rval;
}
}
