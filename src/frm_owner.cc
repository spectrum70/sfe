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

#include "frm_owner.hh"
#include "countries.hh"
#include "trace.hh"
#include "config.hh"

static char const *fields[] = {
	"e_ragione_sociale",
	"e_pec",
	"e_partita_iva",
	"e_cf",
	"f_stato",
	"e_indirizzo",
	"e_n_civico",
	"e_comune",
	"e_provincia",
	"e_cap",
	"e_email",
	"e_telefono",
	"e_banca",
	"e_iban",
	"e_bic_swift",
};

static constexpr int max_fields = 15;
static constexpr int pos_stato = 4;

frm_owner::frm_owner(GtkWindow *parent, db_connector *db) : db(db)
{
	string path = config::get().get_path_res() + "/forms/frm_owner.ui";

	gb = gtk_builder_new_from_file(path.c_str());
	if (!gb) {
		err << "cannot create gtk builder, exiting\n";
		exit(-1);
	}

	GObject* btn_cancel, *btn_save;

	window = (GtkWindow *)gtk_builder_get_object(gb, "frm_owner");
	btn_cancel = gtk_builder_get_object(gb, "btn_cancel");
	btn_save = gtk_builder_get_object(gb, "btn_save");

	g_signal_connect(btn_cancel, "clicked",
				  G_CALLBACK (on_button_btn_cancel), this);
	g_signal_connect(btn_save, "clicked",
				  G_CALLBACK (on_button_btn_save), this);

	GtkFrame *f = (GtkFrame *)gtk_builder_get_object(gb, "f_stato");
	dd_countries = (GtkDropDown *)gtk_drop_down_new_from_strings(countries);
	gtk_frame_set_child(f, GTK_WIDGET(dd_countries));

	gtk_window_present(GTK_WINDOW(window));

	setup_fields();
}

frm_owner::~frm_owner()
{
}

void frm_owner::select_combo_item(GtkDropDown *dd, const char *item)
{
	int i;

	for (i = 0; countries[i]; ++i) {
		if (strcmp(item, countries[i]) == 0) {
			gtk_drop_down_set_selected(dd, i);
			break;
		}
	}
}

void frm_owner::setup_fields()
{
	string query;
	rlist rl;
	GObject *entry;
	string data;
	int i;

	query = "SELECT * FROM dati_propria_azienda LIMIT 1";

	if (db->db_query_with_result(query, rl))
		err << "database query error\n";

	if (rl.size() == 0)
		return;

	for (i = 0; i < max_fields; ++i) {
		data = rl.front()[i + 1];
		entry = gtk_builder_get_object(gb, fields[i]);

		if (i == pos_stato) {
			select_combo_item(dd_countries, data.c_str());
			continue;
		}

		gtk_entry_buffer_set_text(
			gtk_entry_get_buffer(GTK_ENTRY(entry)),
			data.c_str(), data.size());
	}
}

void frm_owner::on_button_btn_cancel(GtkWidget *widget, gpointer data)
{
	frm_owner *f = (frm_owner *)data;

	gtk_window_close(f->window);
}

void frm_owner::on_button_btn_save(GtkWidget *widget, gpointer data)
{
	frm_owner *f = (frm_owner *)data;
	GObject *entry;
	string query;
	int i;

	query = "INSERT OR REPLACE INTO dati_propria_azienda VALUES (1";

	for (i = 0; i < max_fields; ++i) {
		query += ", '";

		entry = gtk_builder_get_object(f->gb, fields[i]);
		if (i == pos_stato) {
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
		err << "database query error\n";

	gtk_window_close(f->window);
}
