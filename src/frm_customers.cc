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

#include "frm_customers.hh"
#include "countries.hh"
#include "trace.hh"

static char const *fields[] = {
	"e_ragione_sociale",
	"e_partita_iva",
	"f_stato",
	"e_indirizzo",
	"e_n_civico",
	"e_comune",
	"e_provincia",
	"e_email",
	"e_telefono",
};

frm_customers::frm_customers(GtkWindow *parent, db_connector *db) : db(db)
{
	gb = gtk_builder_new_from_file(
		"/home/angelo/dev-kernelspace/sfe/forms/frm_customers.ui");

	GObject* btn_add_update, *btn_close;

	window = (GtkWindow *)gtk_builder_get_object(gb, "frm_customers");
	btn_add_update = gtk_builder_get_object(gb, "btn_add_update");
	btn_close = gtk_builder_get_object(gb, "btn_close");

	g_signal_connect(btn_add_update, "clicked",
				  G_CALLBACK (on_button_btn_add_update), this);
	g_signal_connect(btn_close, "clicked",
				  G_CALLBACK (on_button_btn_close), this);

	GtkFrame *f = (GtkFrame *)gtk_builder_get_object(gb, "f_stato");
	dd_countries = (GtkDropDown *)gtk_drop_down_new_from_strings(countries);
	gtk_frame_set_child(f, GTK_WIDGET(dd_countries));

	gtk_window_present(GTK_WINDOW(window));

	setup_fields();
}

frm_customers::~frm_customers()
{
}

void frm_customers::setup_fields()
{
	string query;
	rlist rl;
	GObject *entry;
	string data;
	int i;

	query = "SELECT * FROM anagrafica_clienti LIMIT 1";

	if (db->db_query_with_result(query, rl))
		err << "database query error\n";

	if (rl.size() == 0)
		return;

	for (i = 0; i < 9; ++i) {
		data = rl.front()[i + 1];
		entry = gtk_builder_get_object(gb, fields[i]);

		if (i == 2) {
			select_combo_item(dd_countries, data.c_str());
			continue;
		}

		gtk_entry_buffer_set_text(
			gtk_entry_get_buffer(GTK_ENTRY(entry)),
					  data.c_str(), data.size());
	}
}

void frm_customers::on_button_btn_add_update(GtkWidget *widget, gpointer data)
{
	frm_customers *f = (frm_customers *)data;

	query = "INSERT OR REPLACE INTO dati_propria_azienda VALUES (1";

	if (f->db->db_query_with_result(query))
		err << "database query error\n";



	gtk_window_close(f->window);
}

void frm_customers::on_button_btn_close(GtkWidget *widget, gpointer data)
{
	frm_customers *f = (frm_customers *)data;
	GObject *entry;
	string query;
	int i;





	gtk_window_close(f->window);
}
