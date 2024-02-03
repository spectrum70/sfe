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

#include "db_connector.hh"
#include "config.hh"
#include "trace.hh"
#include "fs.hh"
#include "path.hh"

static constexpr char db_path[] = ".sfe";
static constexpr char db_name[] = "sfe.db";

db_connector::db_connector()
{
}

db_connector::~db_connector()
{
	sqlite3_close(db);
}

int db_connector::db_create()
{
	char *err_msg;
	int rc;
	string query;
	string db_dir_name;
	fs fs;
	path p;

	db_dir_name = p.get_home_path() + "/" + db_path;

	if (!fs.is_dir(db_dir_name.c_str())) {
		fs.create_directory(db_dir_name.c_str());
	}

	/*
	 * Note, don't use same name of the application or an error
	 * +++err: SQL error: file is not a database
	 * will be thrown, since file cannot be ceated over same app name.
	 */
	rc = sqlite3_open((string(db_dir_name) + "/sfe.db").c_str(), &db);
	if (rc != SQLITE_OK) {
		err << "Can't open database: " << sqlite3_errmsg(db) << "\n";
		sqlite3_close(db);
		return -1;
	}

	query = "create table if not exists dati_propria_azienda ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"ragione_sociale TEXT NOT NULL,"
		"pec TEXT NOT NULL,"
		"partita_iva TEXT NOT NULL,"
		"cf TEXT NOT NULL,"
		"stato TEXT NOT NULL,"
		"indirizzo TEXT NOT NULL,"
		"numero_civico TEXT NOT NULL,"
		"comune TEXT NOT NULL,"
		"provincia TEXT NOT NULL,"
		"cap TEXT NOT NULL,"
		"email TEXT NOT NULL,"
		"telefono TEXT NOT NULL,"
		"banca TEXT NOT NULL,"
		"iban TEXT NOT NULL,"
		"bic TEXT NOT NULL"
		");";

	rc = sqlite3_exec(db, query.c_str(), 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		err << "SQL error: " << err_msg << "\n";
		sqlite3_free(err_msg);
		return -1;
	}

	query = "create table if not exists anagrafica_clienti ("
		"id INTEGER PRIMARY KEY,"
		"ragione_sociale TEXT NOT NULL,"
		"codice_destinatario TEXT NOT NULL,"
		"pec TEXT NOT NULL,"
		"partita_iva TEXT NOT NULL,"
		"cf TEXT NOT NULL,"
		"stato TEXT NOT NULL,"
		"indirizzo TEXT NOT NULL,"
		"numero_civico TEXT NOT NULL,"
		"comune TEXT NOT NULL,"
		"cap TEXT NOT NULL,"
		"email TEXT NOT NULL,"
		"telefono TEXT NOT NULL"
		");";

	rc = sqlite3_exec(db, query.c_str(), 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		err << "SQL error: " << err_msg << "\n";
		sqlite3_free(err_msg);
		return -1;
	}

	return 0;
}

int db_connector::execl_callback(void *data_ptr, int count,
					char **data, char **columns)
{
	int i;
	rlist *rl = (rlist *)data_ptr;
	vector<string> v;

	/* count = number of columns */
	for (i = 0; i < count; i++) {
		v.push_back(data[i]);
	}

	rl->push_back(v);

	return 0;
}

int db_connector::db_query_with_result(const string &query, rlist &r_list)
{
	char *err_msg;
	int rc;

	r_list.clear();

	rc = sqlite3_exec(db, query.c_str(), execl_callback, &r_list, &err_msg);
	if (rc != SQLITE_OK) {
		err << "SQL error: " << err_msg << "\n";
		sqlite3_free(err_msg);
		return -1;
	}

	return 0;
}

int db_connector::db_query_with_result(const string &query)
{
	char *err_msg;
	int rc;

	rc = sqlite3_exec(db, query.c_str(), 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		err << "SQL error: " << err_msg << "\n";
		sqlite3_free(err_msg);
		return -1;
	}

	return 0;
}

