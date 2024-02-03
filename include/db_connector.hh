#ifndef db_connector_hh
#define db_connector_hh

#include <sqlite3.h>
#include <list>
#include <vector>
#include <string>

using std::list;
using std::vector;
using std::string;

typedef list<vector<string>> rlist;

class db_connector {
public:
	db_connector();
	~db_connector();

	int db_create();
	int db_query_with_result(const string &query, rlist &r_list);
	int db_query_with_result(const string &query);

	static int execl_callback(void *data_ptr, int count,
					 char **data, char **columns);


private:
	sqlite3 *db;
};

#endif /* db_connector_hh */
