#ifndef latex_hh
#define latex_hh

#include "fs.hh"
#include <string>
#include <vector>
#include <map>

using std::string;
using std::map;

typedef map<string, string> m_fields;

struct latex {
	latex() {}

	int generate_invoice(m_fields &mf, const string &file_name);

private:
	int load_invoice_template();
	int setup_fields(m_fields &mf);
	int insert_verb(const char *rf_field, const char *latex);
	int insert_text(const char *rf_field, const char *latex);
	int insert_prod_line(const char *idx,
			     const char *desc,
			     const char *qty,
			     const char *value);

	fs f;
	string content;
};

#endif /* latex_hh */
