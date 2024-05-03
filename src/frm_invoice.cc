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

#include "frm_invoice.hh"
#include "latex.hh"
#include "trace.hh"
#include "utils.hh"
#include "config.hh"
#include "fs.hh"
#include "path.hh"
#include "xml.hh"

#include <fstream>
#include <sstream>
#include <iomanip>

using namespace tools;
using namespace std;

static const char invoice_path[] = "/.sfe/invoices";
static const char signature[] ="SFEI";

static const char formato_trasmissione_pr[] = "FPR12";
static const char formato_trasmissione_pa[] = "FPA12";

static const char *reg_fis[] = {
	"RF01-Regime Ordinario.",
	"RF02-Regime Contribuenti minimi (art.1, c.96-117, L.244/07).",
	"RF04-Regime Agricoltura e attività connesse e pesca (art.34 e 34-bis, "
		"D.P.R. 633/72).",
	"RF05-Regime Vendita sali e tabacchi (art.74, c.1, D.P.R. 633/72).",
	"RF06-Regime Commercio fiammiferi (art.74, c.1, D.P.R. 633/72).",
	"RF07-Regime Editoria (art.74, c.1, D.P.R. 633/72).",
	"RF08-Regime Gestione servizi telefonia pubblica (art.74, c.1, D.P.R. "
		"633/72).",
	"RF09-Regime Rivendita documenti di trasporto pubblico e di sosta "
		"(art.74, c.1, D.P.R. 633/72).",
	"RF10-Regime Intrattenimenti, giochi e altre attività di cui alla "
		"tariffa allegata al DPR 640/72 (art.74, c.6, D.P.R. 633/72).",
	"RF11-Regime Agenzie viaggi e turismo (art.74-ter, D.P.R. 633/72).",
	"RF12-Regime Agriturismo (art.5, c.2, L. 413/91).",
	"RF13-Regime Vendite a domicilio (art.25-bis, c.6, D.P.R. 600/73).",
	"RF14-Regime Rivendita beni usati, oggetti d'arte, d'antiquariato o "
		"da collezione (art.36, D.L. 41/95).",
	"RF15-Regime Agenzie di vendite all'asta di oggetti d'arte, "
		"antiquariato o da collezione (art.40-bis, D.L. 41/95).",
	"RF16-IVA per cassa P.A. (art. 6, c.5, D.P.R. 633/1972).",
	"RF17-IVA per cassa (art. 32-bis, D.L. 83/2012).",
	"RF19-Regime forfettario.",
};

frm_invoice::frm_invoice(GtkWindow *parent, db_connector *db) : db(db)
{
	GObject *btn_load, *btn_gen_cart, *btn_save, *btn_close, *btn_gen_xml;
	string path, field;
	char idx[3];

	path = config::get().get_path_res() + "/forms/frm_invoice.ui";

	gb = gtk_builder_new_from_file(path.c_str());

	window = (GtkWindow *)gtk_builder_get_object(gb, "frm_invoice");

	btn_save = gtk_builder_get_object(gb, "btn_save");
	btn_load =  gtk_builder_get_object(gb, "btn_load");
	btn_gen_cart = gtk_builder_get_object(gb, "btn_gen_cart");
	btn_gen_xml = gtk_builder_get_object(gb, "btn_gen_xml");
	btn_close =  gtk_builder_get_object(gb, "btn_close");

	dd_rf = (GtkDropDown *)gtk_drop_down_new_from_strings(reg_fis);
	cb_bollo = (GtkCheckButton *)(gtk_builder_get_object(gb, "cb_bollo"));
	e_number = (GtkEntry *)(gtk_builder_get_object(gb, "e_numero"));
	e_codice_univoco = (GtkEntry *)
		(gtk_builder_get_object(gb, "e_codice_univoco"));
	l_total = (GtkLabel *)gtk_builder_get_object(gb, "l_total");

	for (int i = 1; i <= 10; ++i) {
		sprintf(idx, "%02d", i);
		field = string("desc_") + idx;
		e_desc[i - 1] = (GtkEntry *)gtk_builder_get_object
				(gb, field.c_str());
		field = string("qty_") + idx;
		e_qty[i - 1] = (GtkEntry *)gtk_builder_get_object
				(gb, field.c_str());
		field = string("unit_") + idx;
		e_unit[i - 1] = (GtkEntry *)gtk_builder_get_object
				(gb, field.c_str());
		field = string("value_") + idx;
		e_value[i - 1] = (GtkEntry *)gtk_builder_get_object
				(gb, field.c_str());
		field = string("l_subtot_") + idx;
		l_subtotal[i - 1] = (GtkLabel *)gtk_builder_get_object
				(gb, field.c_str());
	}
	g_signal_connect(btn_load, "clicked",
			 G_CALLBACK(on_button_btn_load), this);
	g_signal_connect(btn_save, "clicked",
			 G_CALLBACK(on_button_btn_save), this);
	g_signal_connect(btn_gen_cart, "clicked",
			 G_CALLBACK(on_button_btn_gen_cart), this);
	g_signal_connect(btn_gen_xml, "clicked",
			 G_CALLBACK(on_button_btn_gen_xml), this);
	g_signal_connect(btn_close, "clicked",
			 G_CALLBACK(on_button_btn_close), this);

	connect_signal_set(e_qty, on_key_press_qty);
	connect_signal_set(e_value, on_key_press_value);

	gtk_window_present(GTK_WINDOW(window));

	GtkFrame *f =
		(GtkFrame *)gtk_builder_get_object(gb, "f_regime_fiscale");
	gtk_frame_set_child(f, GTK_WIDGET(dd_rf));

	setup_fields();
}

frm_invoice::~frm_invoice()
{
}

void frm_invoice::connect_signal_set(GtkEntry **p, gtk_callback callback)
{
	for (int i = 1; i <= 10; ++i) {
		g_signal_connect(p[i - 1],
				 "changed", G_CALLBACK(callback), this);
	}
}

void frm_invoice::update_subtotal(frm_invoice *f, GtkWidget *widget, int i)
{
	char idx[16];

	sprintf(idx, "%02d", i);

	string name = string("l_subtot_") + idx;
	i--;
	f->subtot[i] = (float)f->qty[i] * f->value[i];

	GtkLabel *l = (GtkLabel *)gtk_builder_get_object(gb, name.c_str());

	gtk_label_set_text(l, ftoa(f->subtot[i]).c_str());
}

void frm_invoice::update_total(frm_invoice *f)
{
	int i;

	for (i = 0, f->total = 0; i < 10; ++i) {
		f->total += f->subtot[i];
	}

	gtk_label_set_text(f->l_total, ftoa(f->total).c_str());
}

void frm_invoice::on_key_press_qty(GtkWidget *widget, gpointer data)
{
	int i;
	string field;
	frm_invoice *f = (frm_invoice *)data;
	string v = gtk_entry_buffer_get_text(
		gtk_entry_get_buffer(GTK_ENTRY(widget)));

	string name = gtk_buildable_get_buildable_id(GTK_BUILDABLE(widget));

	i = atoi(&name.c_str()[4]);
	f->qty[i - 1] = v.size() ? atoi(v.c_str()) : 0;

	f->update_subtotal(f, widget, i);
	f->update_total(f);
}

void frm_invoice::on_key_press_value(GtkWidget *widget, gpointer data)
{
	int i;
	string field;
	frm_invoice *f = (frm_invoice *)data;
	string v = gtk_entry_buffer_get_text(
		gtk_entry_get_buffer(GTK_ENTRY(widget)));

	string name = gtk_buildable_get_buildable_id(GTK_BUILDABLE(widget));

	i = atoi(&name.c_str()[6]);
	f->value[i - 1] = v.size() ? atof(v.c_str()) : 0;

	f->update_subtotal(f, widget, i);
	f->update_total(f);
}

void frm_invoice::setup_fields()
{
	GtkFrame *f;
	GtkLabel *label;
	string query;
	rlist rl;
	size_t i;
	string dati;

	query = "SELECT * FROM dati_propria_azienda LIMIT 1";

	if (db->db_query_with_result(query, rl))
		err << "database query error\n";

	if (rl.size() == 0)
		return;

	label = (GtkLabel *)gtk_builder_get_object(gb, "l_dati_aziendali");

	gtk_label_set_justify(label, GTK_JUSTIFY_LEFT);

	vector<string> vs = *rl.begin();

	dati_iva = vs[3];
	dati_cf = vs[4];
	dati_iban = vs[14];
	dati_swift = vs[15];

	dati = vs[1];
	dati += "\nP.IVA: ";
	dati += vs[3];
	dati += "\nC.F: ";
	dati += vs[4];
	dati += "\n";
	dati += vs[6];
	dati += " ";
	dati += vs[7];
	dati += "\n";
	dati += vs[9];
	dati += " ";
	dati += vs[8];
	dati += " ";
	dati += vs[5];
	dati += "\n";
	dati += vs[10];
	dati += "\n";
	dati += vs[11];

	gtk_label_set_text(GTK_LABEL(label), dati.c_str());

	f = (GtkFrame *)gtk_builder_get_object(gb, "f_cliente");

	query = "SELECT * FROM anagrafica_clienti";

	if (db->db_query_with_result(query, rl))
		err << "database query error\n";

	char **clienti = new char *[rl.size() + 1];
	for (i = 0; i < rl.size(); ++i) {
		clienti[i] = new char[255];
	}
	/* 0 terminated. mandatory */
	clienti[i] = 0;

	i = 0;
	for (auto const &v: rl) {
		strcpy(clienti[i++], v[1].c_str());
	}

	dd_clienti = (GtkDropDown *)gtk_drop_down_new_from_strings(clienti);
	gtk_frame_set_child(f, GTK_WIDGET(dd_clienti));
}

string frm_invoice::get_ente_emittente(const string& country)
{
	/* As of now, this program is for Italy. */
	return "IT";
}

void frm_invoice::get_date_value(frm_invoice *f,
				 int &year, int &month, int &day)
{
	GtkCalendar *cal;
	GDateTime *dt;

	cal = GTK_CALENDAR(gtk_builder_get_object(f->gb, "cal_date"));
	dt = gtk_calendar_get_date(cal);

	day = g_date_time_get_day_of_month(dt);
	month =  g_date_time_get_month(dt);
	year =  g_date_time_get_year(dt);
}

string frm_invoice::get_date(frm_invoice *f)
{
	int day, month, year;

	get_date_value(f, year, month, day);
	string date = itoa(day) + "/" + itoa(month) + "/" + itoa(year);

	return date;
}

string frm_invoice::get_date_xml(frm_invoice *f)
{
	stringstream date;
	int day, month, year;

	get_date_value(f, year, month, day);
	date << setw(4) << setfill('0') << year << "-"
		<< setw(2) << setfill('0') << month << "-"
		<< setw(2) << setfill('0') << day;

	return date.str();
}

void frm_invoice::set_date(frm_invoice *f, const string &date)
{
	GtkCalendar *cal;
	unsigned int x, p;

	cal = GTK_CALENDAR(gtk_builder_get_object(f->gb, "cal_date"));

	if ((p = date.find('/')) == string::npos) {
		message("corrupted date");
		return;
	}

	int day = atoi(date.substr(0, p));
	if ((x = date.find('/', p + 1)) == string::npos) {
		message("corrupted date");
		return;
	}

	int month = atoi(date.substr(p + 1, x));
	int year =  atoi(date.substr(x + 1));

	gtk_calendar_set_day(cal, day);
	gtk_calendar_set_month(cal, month - 1);
	gtk_calendar_set_year(cal, year);
}

void frm_invoice::on_button_btn_gen_cart(GtkWidget *widget, gpointer data)
{
	frm_invoice *f = (frm_invoice *)data;
	m_fields mf;
	GtkStringObject *p;
	GtkLabel *l;
	rlist rl;
	string query, field;
	const char *text;
	int i;
	char idx[3];

	p = (GtkStringObject *)gtk_drop_down_get_selected_item(f->dd_rf);

	text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(f->e_number));

	mf["n"] = text;
	mf["data"] = f->get_date(f);
	mf["bollo"] = (gtk_check_button_get_active(f->cb_bollo) ? "SI" : "NO");
	mf["importo_bollo"] = "2.00";
	mf["regime_fiscale"] = gtk_string_object_get_string(p);

	text = gtk_entry_buffer_get_text(
		gtk_entry_get_buffer(f->e_codice_univoco));
	mf["codice_progressivo"] = text;

	l = GTK_LABEL(gtk_builder_get_object(f->gb, "l_dati_aziendali"));
	text = gtk_label_get_text(l);
	mf["dati_aziendali"] = text;

	mf["iban_swift"] = string("IBAN: ") + f->dati_iban +
			   string(" BIC/SWIFT: ") + f->dati_swift;

	p = (GtkStringObject *)gtk_drop_down_get_selected_item(f->dd_clienti);

	query = "SELECT * FROM anagrafica_clienti WHERE ragione_sociale = '";
	query += gtk_string_object_get_string(p);
	query += "'";

	if (f->db->db_query_with_result(query, rl))
		err << "database query error\n";

	mf["dati_cessionario"] = (*rl.begin())[1] + "\nP.IVA: " +
				 (*rl.begin())[3] + "\n" +
				 (*rl.begin())[7] + " " +
				 (*rl.begin())[8] + "\n" +
				 (*rl.begin())[10] + " " +
				 (*rl.begin())[9] + "\n" +
				 (*rl.begin())[11];

	for (i = 1; i <= 10; ++i) {
		sprintf(idx, "%02d", i);

		mf[string("desc_") + idx] = gtk_entry_buffer_get_text(
				gtk_entry_get_buffer(f->e_desc[i - 1]));
		mf[string("qty_") + idx] = gtk_entry_buffer_get_text(
				gtk_entry_get_buffer(f->e_qty[i - 1]));
		mf[string("value_") + idx] = gtk_entry_buffer_get_text(
				gtk_entry_get_buffer(f->e_value[i - 1]));
	}

	string name = "invoice_";
	name += mf["n"];
	name += "_";
	name += mf["codice_progressivo"];
	name += ".pdf";
	f->normalize_name(name);

	latex la;
	la.generate_invoice(mf, name);
}

int frm_invoice::load_invoice(frm_invoice *f, const char *file_name)
{
	string in, field;
	char idx[3];
	ifstream ifs(file_name);

	if (!ifs.is_open()) {
		message("could not open this file.");
		return -1;
	}

	getline(ifs, in);

	if (in != signature) {
		message("wrong signature");
		return -1;
	}

	getline(ifs, in);
	f->set_date(f, in);

	getline(ifs, in);
	gtk_drop_down_set_selected(f->dd_clienti, atoi(in));
	getline(ifs, in);
	gtk_drop_down_set_selected(f->dd_rf, atoi(in));
	getline(ifs, in);
	gtk_check_button_set_active(f->cb_bollo, atoi(in));
	getline(ifs, in);
	gtk_entry_buffer_set_text(
		gtk_entry_get_buffer(f->e_codice_univoco),
				  in.c_str(), in.size());
	getline(ifs, in);
	gtk_entry_buffer_set_text(
		gtk_entry_get_buffer(f->e_number), in.c_str(), in.size());

	for (int i = 1; i <= 10; ++i) {
		sprintf(idx, "%02d", i);

		getline(ifs, in);
		if (!in.size())
			break;

		field = string("desc_") + idx;
		gtk_entry_buffer_set_text(gtk_entry_get_buffer(e_desc[i - 1]),
					  in.c_str(), in.size());
		getline(ifs, in);
		field = string("qty_") + idx;
		gtk_entry_buffer_set_text(gtk_entry_get_buffer(e_qty[i - 1]),
					  in.c_str(), in.size());
		getline(ifs, in);
		field = string("unit_") + idx;
		gtk_entry_buffer_set_text(gtk_entry_get_buffer(e_unit[i - 1]),
					  in.c_str(), in.size());
		getline(ifs, in);
		field = string("value_") + idx;
		gtk_entry_buffer_set_text(gtk_entry_get_buffer(e_value[i - 1]),
					  in.c_str(), in.size());
	}

	return 0;
}

void frm_invoice::on_file_open(GObject *obj, GAsyncResult *ares, gpointer data)
{
	frm_invoice *f = (frm_invoice *)data;
	g_autoptr(GError) error = NULL;
	g_autoptr(GFile) file = gtk_file_dialog_open_finish
		(GTK_FILE_DIALOG(obj), ares, &error);

	if (error != NULL) {
		return;
	} else {
		// do something with the file
		f->load_invoice(f, g_file_get_path(file));
	}
}

void frm_invoice::on_button_btn_load(GtkWidget *widget, gpointer data)
{
	frm_invoice *f = (frm_invoice *)data;
	path p;

	GtkFileDialog * fd = gtk_file_dialog_new();
	GFile *dir = g_file_new_for_path(
		(p.get_home_path() + invoice_path).c_str());

	gtk_file_dialog_set_initial_folder(fd, dir);
	gtk_file_dialog_set_title(fd, "Select the invoice ...");
	gtk_file_dialog_set_modal(fd, 1);
	gtk_file_dialog_open(fd, (GtkWindow*)f->window, 0, on_file_open, f);
}

void frm_invoice::normalize_name(string &name)
{
	string rstring;
	size_t i;

	if ((i = name.find('/')) != string::npos) {
		name.replace(i, 1, "-");
	}
}

void frm_invoice::on_file_save(GObject *obj, GAsyncResult *ares, gpointer data)
{
	frm_invoice *f = (frm_invoice *)data;
	string inv_num, inv_name, codice_progressivo;
	g_autoptr(GError) error = NULL;
	g_autoptr(GFile) file = gtk_file_dialog_save_finish
				(GTK_FILE_DIALOG(obj), ares, &error);

	if (file == 0) {
		/* Cancel */
		return;
	}

	ofstream f_inv(g_file_get_path(file));

	int customer = gtk_drop_down_get_selected(f->dd_clienti);
	int reg_fisc = gtk_drop_down_get_selected(f->dd_rf);
	int bollo_virtuale = gtk_check_button_get_active(f->cb_bollo);

	codice_progressivo = gtk_entry_buffer_get_text(gtk_entry_get_buffer
				(f->e_codice_univoco));

	inv_num = gtk_entry_buffer_get_text(gtk_entry_get_buffer
				(f->e_number));

	f_inv << signature << "\n";
	f_inv << f->get_date(f) << "\n";
	f_inv << customer << "\n";
	f_inv << reg_fisc << "\n";
	f_inv << (bollo_virtuale ? "1": "0") << "\n";
	f_inv << codice_progressivo << "\n";
	f_inv << inv_num << "\n";

	string field, desc, qty, unit, value;

	for (int i = 1; i <= 10; ++i) {
		desc = gtk_entry_buffer_get_text
		(gtk_entry_get_buffer(f->e_desc[i - 1]));
		if (desc == "")
			break;
		qty = gtk_entry_buffer_get_text
		(gtk_entry_get_buffer(f->e_qty[i - 1]));
		unit = gtk_entry_buffer_get_text
		(gtk_entry_get_buffer(f->e_unit[i - 1]));
		value = gtk_entry_buffer_get_text
		(gtk_entry_get_buffer(f->e_value[i - 1]));

		f_inv << desc << "\n" << qty << "\n"
		<< unit << "\n" << value << "\n";
	}

	f_inv.close();

	string msg = "invoice ";
	msg += g_file_get_path(file);
	msg += " saved.";

	f->message(msg);
}

void frm_invoice::on_button_btn_save(GtkWidget *widget, gpointer data)
{
	frm_invoice *f = (frm_invoice *)data;
	path p;
	fs fs;
	string inv_num, codice_progressivo;
	GtkFileDialog * fd = gtk_file_dialog_new();
	GFile *dir = g_file_new_for_path(
		(string(p.get_home_path()) + invoice_path).c_str());

	inv_num = gtk_entry_buffer_get_text(gtk_entry_get_buffer(f->e_number));
	codice_progressivo = gtk_entry_buffer_get_text(gtk_entry_get_buffer
				(f->e_codice_univoco));

	if (inv_num == "" || codice_progressivo == "") {
		f->message("Non posso salvare, mancano codice progressivo o "
			"numero delal fattura");
		return;
	}

	string name = "invoice_";
	name += inv_num;
	name += "_";
	name += codice_progressivo;
	name += ".dat";

	f->normalize_name(name);

	gtk_file_dialog_set_initial_folder(fd, dir);
	gtk_file_dialog_set_initial_name(fd, name.c_str());
	gtk_file_dialog_set_title(fd, "Save invoice ...");
	gtk_file_dialog_set_modal(fd, 1);
	gtk_file_dialog_save(fd, (GtkWindow*)f->window, 0, on_file_save, f);
}

string frm_invoice::try_to_set_progressivo(frm_invoice *f)
{
	return gtk_entry_buffer_get_text
		(gtk_entry_get_buffer(f->e_codice_univoco));
}

void frm_invoice::on_button_btn_gen_xml(GtkWidget *widget, gpointer data)
{
	frm_invoice *f = (frm_invoice *)data;
	fmap fm;
	rlist rl;
	string query;
	xml xml;
	GtkStringObject *p;

	query = "SELECT * FROM dati_propria_azienda LIMIT 1";

	if (f->db->db_query_with_result(query, rl))
		err << "database query error\n";

	fm["paese"] = f->get_ente_emittente("");
	fm["codice"] = f->dati_cf;
	fm["progressivo"] = f->try_to_set_progressivo(f);
	fm["formato"] = formato_trasmissione_pr;
	fm["denom"] = uppercase((*rl.begin())[1]);

	p = (GtkStringObject *)gtk_drop_down_get_selected_item(f->dd_rf);

	string regime_fiscale =
		string(gtk_string_object_get_string(p)).substr(0, 4);

	fm["rf"] = regime_fiscale;

	fm["piva"] = (*rl.begin())[3];
	fm["via"] = uppercase((*rl.begin())[6]);
	fm["num"] = (*rl.begin())[7];
	fm["cap"] = (*rl.begin())[10];
	fm["comune"] = uppercase((*rl.begin())[8]);
	fm["provincia"] = uppercase((*rl.begin())[9]);
	fm["nazione"] = f->get_ente_emittente("");

	p = (GtkStringObject *)gtk_drop_down_get_selected_item(f->dd_clienti);

	query = "SELECT * FROM anagrafica_clienti WHERE ragione_sociale = '";
	query += gtk_string_object_get_string(p);
	query += "'";

	rl.clear();

	if (f->db->db_query_with_result(query, rl))
		err << "database query error\n";

	fm["codice_dest"] = (*rl.begin())[2];

	fm["cc_paese"] = ((*rl.begin())[6]).substr(0, 2);
	fm["cc_piva"] = (*rl.begin())[4];
	fm["cc_denom"] = uppercase((*rl.begin())[1]);
	fm["cc_via"] = uppercase((*rl.begin())[7]);
	fm["cc_num"] = (*rl.begin())[8];
	fm["cc_cap"] = (*rl.begin())[10];
	fm["cc_comune"] = uppercase((*rl.begin())[9]);
	fm["cc_nazione"] = ((*rl.begin())[6]).substr(0, 2);

	fm["tipo"] = "TD01";
	fm["divisa"] = "EUR";
	fm["data"] = f->get_date_xml(f);
	fm["numero"] = gtk_entry_buffer_get_text(
			gtk_entry_get_buffer(f->e_number));
	fm["bollo"] = (gtk_check_button_get_active(f->cb_bollo) ? "SI" : "NO");

	fm["importo_bollo"] = "2.00";
	fm["importo_totale"] = gtk_label_get_text(f->l_total);

	char idx[3];
	int val;
	for (int i = 1; i <= 10; ++i) {
		sprintf(idx, "%02x", i);
		fm[string("desc_") + idx] = gtk_entry_buffer_get_text(
			gtk_entry_get_buffer(f->e_desc[i - 1]));

		val = atoi(gtk_entry_buffer_get_text(
			gtk_entry_get_buffer(f->e_qty[i - 1])));
		fm[string("qty_") + idx] = ftoa(val);

		fm[string("unit_") + idx] = gtk_entry_buffer_get_text(
			gtk_entry_get_buffer(f->e_unit[i - 1]));

		val = atoi(gtk_entry_buffer_get_text(
			gtk_entry_get_buffer(f->e_value[i - 1])));
		fm[string("value_") + idx] = ftoa(val);

		fm[string("subtotal_") + idx] =
			gtk_label_get_text(f->l_subtotal[i - 1]);
	}

	string name = "invoice_";
	name += fm["numero"];
	name += "_";
	name += fm["progressivo"];
	name += ".xml";
	f->normalize_name(name);

	xml.create_xml(fm, name);

	string msg = "invoice xml saved.";
	f->message(msg);
}

void frm_invoice::on_button_btn_close(GtkWidget *widget, gpointer data)
{
	frm_invoice *f = (frm_invoice *)data;

	gtk_window_close(f->window);
}
