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

#include "latex.hh"
#include "fs.hh"
#include "config.hh"
#include "path.hh"

#include <stdlib.h>

static const char pdf_path[] = "/.sfe/invoices/pdf";

int latex::load_invoice_template()
{
	string path = config::get().get_path_res() + "/invoice/invoice.tex";

	if (f.load_to_memory(path.c_str(), content))
		return 0;

	return -1;
}

int latex::insert_verb(const char *rf_field, const char *latex)
{
	unsigned int i, x;

	if ((i = content.find(rf_field)) == string::npos)
		return -1;
	if ((i = content.rfind("\\begin{verbatim}", i)) == string::npos)
		return -1;
	i += 15;
	if ((x = content.find("\\end{verbatim}", i)) == string::npos)
		return -1;

	content.replace(i, x - i, (string("}") + latex));

	return 0;
}

int latex::insert_text(const char *rf_field, const char *latex)
{
	unsigned int i, x;

	if ((i = content.find(rf_field)) == string::npos)
		return -1;
	if ((i = content.rfind('{', i)) == string::npos)
		return -1;
	if ((x = content.find('}', i)) == string::npos)
		return -1;

	content.replace(i, x - i, (string("{") + latex));

	return 0;
}

int latex::insert_prod_line(const char *idx,
			    const char *desc,
			    const char *qty,
			    const char *value)
{
	string fields, rf = string("rf_e") + string(idx);
	unsigned int i;

	if ((i = content.find(rf)) == string::npos)
		return -1;
	if ((i = content.rfind("{}{}{}{}", i)) == string::npos)
		return -1;

	fields = string("{") + desc + "}{" + qty + "}{" + value + "}{0.00}";

	content.replace(i, 8, fields.c_str());

	return 0;
}

int latex::setup_fields(m_fields &mf)
{
	string title;

	title = config::get().get_string("latex.invoice_title");

	if (insert_text("rf_title", title.c_str()))
		return -1;
	if (insert_text("rf_n", mf["n"].c_str()))
		return -1;
	if (insert_text("rf_data", mf["data"].c_str()))
		return -1;
	if (insert_verb("rf_issued_from", mf["dati_aziendali"].c_str()))
		return -1;
	if (insert_verb("rf_issued_to", mf["dati_cessionario"].c_str()))
		return -1;
	if (insert_text("rf_regime_fiscale", mf["regime_fiscale"].c_str()))
		return -1;
	if (insert_text("rf_iban_swift", mf["iban_swift"].c_str()))
		return -1;
	if (mf["bollo"] == "SI") {
		if (insert_text("rf_bollo_virtuale",
		    (string(mf["importo_bollo"]) + " E").c_str()))
			return 1;
	} else {
		if (insert_text("rf_bollo_virtuale", "NO"))
			return 1;
	}

	int i;
	char idx[3];

	for (i = 1; i <= 10; ++i) {
		sprintf(idx, "%02d", i);
		string desc = string("desc_") + idx;
		string qty = string("qty_") + idx;
		string value = string("value_") + idx;

		if (mf[desc] != "") {
			if (insert_prod_line(idx,
					mf[desc].c_str(),
					mf[qty].c_str(),
					mf[value].c_str()))
				return -1;
		}
	}

	return 0;
}

int latex::generate_invoice(m_fields &mf, const string &file_name)
{
	char name[] = "/tmp/invoice_XXXXXX";
	string dir, cmd;
	path p;
	fs fs;

	dir = p.get_home_path() + pdf_path;

	if (!fs.is_dir(dir.c_str())) {
		fs.create_directory(dir.c_str());
	}

	if (load_invoice_template())
		return -1;

	if (setup_fields(mf))
		return -1;

	f.create_tmp_file(name, &content[0]);

	cmd = "xelatex -output-directory=/tmp -interaction=nonstopmode ";
	cmd += name;
	cmd += " > /dev/null";

	if (system(cmd.c_str()) == 0) {

		string fname = &name[5];

		cmd = "xdg-open ";
		cmd += name;
		cmd += ".pdf";

		system(cmd.c_str());
	}

	return 0;
}
