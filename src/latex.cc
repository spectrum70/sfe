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
#include <stdlib.h>

int latex::load_invoice_template()
{
	 f.load_to_memory("invoice/invoice.tex", content);

	 return 0;
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
	string rf = string("rf_e") + string(idx);
	unsigned int i, x;

	if ((i = content.find(rf)) == string::npos)
		return -1;
	if ((i = content.rfind("\\product{", i)) == string::npos)
		return -1;
	i += 8;
	if ((x = content.find('}', i)) == string::npos)
		return -1;
	content.replace(i, x - i, (string("{") + desc));

	if ((i = content.find('{', i + 1)) == string::npos)
		return -1;
	if ((x = content.find('}', i)) == string::npos)
		return -1;
	content.replace(i, x - i, (string("{") + value));

	if ((i = content.find('{', i + 1)) == string::npos)
		return -1;
	if ((x = content.find('}', i)) == string::npos)
		return -1;
	content.replace(i, x - i, (string("{") + qty));

	/* Set IVA to 0, TODO, calculate it */
	if ((i = content.find('{', i + 1)) == string::npos)
		return -1;
	if ((x = content.find('}', i)) == string::npos)
		return -1;
	content.replace(i, x - i, (string("{0.00")));

	return 0;
}

int latex::setup_fields(m_fields &mf)
{
	if (insert_text("rf_n", mf["n"].c_str()))
		return -1;
	if (insert_text("rf_data", mf["data"].c_str()))
		return -1;
	if (insert_verb("rf_issued_from", mf["dati_aziendali"].c_str()))
		return -1;
	if (insert_verb("rf_issued_to", mf["dati_cessionario"].c_str()))
		return -1;

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

int latex::generate_invoice(m_fields &mf)
{
	char name[] = "/tmp/invoice_XXXXXX";
	string cmd;

	if (load_invoice_template())
		return -1;

	if (setup_fields(mf))
		return -1;

	f.create_tmp_file(name, &content[0]);

	cmd = "pdflatex -synctex=1 -interaction=nonstopmode ";
	cmd += name;

	system(cmd.c_str());

	return 0;
}
