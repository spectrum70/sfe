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

static char invoice[] = "\\documentclass[*]{article}\n"
	"\\usepackage{fp}\n"
	"\\usepackage{booktabs}\n"
	"\\usepackage{ragged2e}\n"
	"\\usepackage{longtable}\n"
	"\\newcounter{cnt}\n"
	"\\setcounter{cnt}{0}\n"
	"\\def\\inc{\\stepcounter{cnt}\\thecnt}\n"
	"\\gdef\\TotalHT{0}\n"
	"\\newcommand{\\product}[3]{%\n"
	"\\inc &#1  &#2   &#3  &\\FPmul\\temp{#2}{#3}\\FPround\\temp{\\temp}{2}\\temp\n"
	"%% Totalize\n"
	"\\FPadd\\total{\\TotalHT}{\\temp}%\n"
	"\\FPround\\total{\\total}{2}%\n"
	"\\global\\let\\TotalHT\\total%\n"
	"\\\\ }\n"
	"\\newcommand{\\totalttc}{\n"
	"   \\TotalHT  }\n"
	"\\begin{document}\n"
	"\\RaggedRight\n"
	"\\begin{longtable}{cp{4.2cm}rrr}\n"
	"\\toprule\n"
	"Item   &Description & Price & Qty & Total\\\\\n"
	"\\midrule\n"
	"    \\product{Computer peripherals}{1000.00}{1}\n"
	"    \\product{Harddisk 2000E}{2000}{1}\n"
	"    \\product{The \\TeX book}{100.00}{100}\n"
	"    \\product{Product Four}{5000.00}{1}\n"
	"    \\product{Product Five}{5000.00}{2}\n"
	"\\midrule\n"
	"    &&&& Total \\totalttc\\\\\n"
	"\\bottomrule\n"
	"\\end{longtable}\n"
	"\\end{document}";

int latex::generate_invoice()
{
	fs fs;
	char name[] = "/tmp/invoice_XXXXXX";
	string cmd;

	fs.create_tmp_file(name, invoice);

	cmd = "pdflatex -synctex=1 -interaction=nonstopmode ";
	cmd += name;

	system(cmd.c_str());

	return 0;
}
