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
#include "trace.hh"

frm_invoice::frm_invoice(GtkWindow *parent, db_connector *db) : db(db)
{
	GObject* btn_cancel, *btn_save;
	GtkFrame *f;
	GtkDropDown *dd;
	rlist rl;
	string query;

	gb = gtk_builder_new_from_file(
		"/home/angelo/dev-kernelspace/sfe/forms/frm_invoice.ui");

	window = (GtkWindow *)gtk_builder_get_object(gb, "frm_invoice");

	f = (GtkFrame *)gtk_builder_get_object(gb, "f_cliente");

	query = "SELECT * FROM anagrafica_clienti";

	if (db->db_query_with_result(query, rl))
		err << "database query error\n";

	char **clienti = new char *[rl.size()];
	for(int i = 0; i < rl.size(); ++i) {
		clienti[i] = new char[255];
	}

	int i = 0;
	for (auto v: rl) {
		strcpy(clienti[i++], v[1].c_str());
	}

	dd = (GtkDropDown *)gtk_drop_down_new_from_strings(clienti);
	gtk_frame_set_child(f, GTK_WIDGET(dd));

	//btn_cancel = gtk_builder_get_object(gb, "btn_cancel");
	//btn_save = gtk_builder_get_object(gb, "btn_save");

	//g_signal_connect(btn_cancel, "clicked",
	//			  G_CALLBACK (on_button_btn_cancel), this);
	//g_signal_connect(btn_save, "clicked",
	//			  G_CALLBACK (on_button_btn_save), this);

	//GtkFrame *f = (GtkFrame *)gtk_builder_get_object(gb, "f_stato");

	gtk_window_present(GTK_WINDOW(window));

	setup_fields();
}

frm_invoice::~frm_invoice()
{
}

void frm_invoice::setup_fields()
{
	string query = "SELECT * FROM dati_propria_azienda LIMIT 1";
	rlist rl;
	int i;
	string dati;

	if (db->db_query_with_result(query, rl))
		err << "database query error\n";

	if (rl.size() == 0)
		return;

	GtkLabel *label = (GtkLabel *)gtk_builder_get_object
					(gb, "l_dati_aziendali");

	gtk_label_set_justify(label, GTK_JUSTIFY_LEFT);

	vector<string> vs = *rl.begin();

	for (i = 1; i < vs.size(); ++i) {
		dati += vs[i];
		dati += "\n";
	}

	gtk_label_set_text(GTK_LABEL(label), dati.c_str());


}

void frm_invoice::on_button_btn_cancel(GtkWidget *widget, gpointer data)
{
	frm_invoice *f = (frm_invoice *)data;

	gtk_window_close(f->window);
}

void frm_invoice::on_button_btn_save(GtkWidget *widget, gpointer data)
{
	frm_invoice *f = (frm_invoice *)data;
	GObject *entry;
	string query;
	int i;

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
