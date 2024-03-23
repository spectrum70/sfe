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

using namespace tools;

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
	GObject *btn_gen_cart, *btn_save, *btn_close;

	gb = gtk_builder_new_from_file(
		"/home/angelo/dev-kernelspace/sfe/forms/frm_invoice.ui");

	btn_gen_cart = gtk_builder_get_object(gb, "btn_gen_cart");
	btn_save = gtk_builder_get_object(gb, "btn_save");
	btn_close =  gtk_builder_get_object(gb, "btn_close");

	g_signal_connect(btn_gen_cart, "clicked",
			 G_CALLBACK(on_button_btn_gen_cart), this);
	g_signal_connect(btn_close, "clicked",
			 G_CALLBACK(on_button_btn_close), this);
	g_signal_connect(btn_save, "clicked",
			 G_CALLBACK(on_button_btn_save), this);

	window = (GtkWindow *)gtk_builder_get_object(gb, "frm_invoice");

	gtk_window_present(GTK_WINDOW(window));

	GtkFrame *f =
		(GtkFrame *)gtk_builder_get_object(gb, "f_regime_fiscale");
	dd_rf = (GtkDropDown *)gtk_drop_down_new_from_strings(reg_fis);
	gtk_frame_set_child(f, GTK_WIDGET(dd_rf));

	setup_fields();
}

frm_invoice::~frm_invoice()
{
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

void frm_invoice::on_button_btn_gen_cart(GtkWidget *widget, gpointer data)
{
	frm_invoice *f = (frm_invoice *)data;
	m_fields mf;
	GtkEntry *e;
	GtkLabel *l;
	GtkCalendar *cal;
	GDateTime *dt;
	rlist rl;
	string query;
	const char *text;

	e = GTK_ENTRY(gtk_builder_get_object(f->gb, "e_numero"));
	text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(e));
	mf["n"] = text;

	cal = GTK_CALENDAR(gtk_builder_get_object(f->gb, "cal_date"));
	dt = gtk_calendar_get_date(cal);

	int day = g_date_time_get_day_of_month(dt);
	int month =  g_date_time_get_month(dt);
	int year =  g_date_time_get_year(dt);

	string date = itoa(day) + "/" + itoa(month) + "/" + itoa(year);
	mf["data"] = date;

	l = GTK_LABEL(gtk_builder_get_object(f->gb, "l_dati_aziendali"));
	text = gtk_label_get_text(l);
	mf["dati_aziendali"] = text;

	GtkStringObject *p = (GtkStringObject *)
			gtk_drop_down_get_selected_item(f->dd_clienti);

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

	latex la;

	la.generate_invoice(mf);
}

void frm_invoice::on_button_btn_close(GtkWidget *widget, gpointer data)
{
	frm_invoice *f = (frm_invoice *)data;

	gtk_window_close(f->window);
}

void frm_invoice::on_button_btn_save(GtkWidget *widget, gpointer data)
{
	frm_invoice *f = (frm_invoice *)data;
	//GObject *entry;
	//string query;
	//int i;

	/*query = "INSERT OR REPLACE INTO dati_propria_azienda VALUES (1";

	for (i = 0; i < 12; ++i) {
		query += ", '";

		entry = gtk_builder_get_object(f->gb, fields[i]);
		if (i == 2) {
			GtkStringObject *so = (GtkStringObject *)
				gtk_drop_down_get_selected_item(
					f->dd_countries);
			query += gtk_string_object_get_string(so);
		} else {
			query += gtk_entry_buffer_get_text(
				gtk_entry_get_buffer(GTK_ENTRY(entry)));
		}

		query += "'";
	}

	query += ");";

	if (f->db->db_query_with_result(query))
		err << "database query error\n";*/

	gtk_window_close(f->window);
}
