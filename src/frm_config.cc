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

#include "frm_config.hh"
#include "config.hh"
#include "trace.hh"

frm_config::frm_config(GtkWindow *parent)
{
	string path = config::get().get_path_res() + "/forms/frm_config.ui";

	gb = gtk_builder_new_from_file(path.c_str());
	if (!gb) {
		err << "cannot create gtk builder, exiting\n";
		exit(-1);
	}

	GObject* btn_close, *btn_save;
	window = (GtkWindow *)
			gtk_builder_get_object(gb, "frm_config");

	btn_save = gtk_builder_get_object(gb, "btn_save");
	btn_close = gtk_builder_get_object(gb, "btn_close");

	g_signal_connect(btn_save, "clicked",
			 G_CALLBACK(on_button_btn_save), this);
	g_signal_connect(btn_close, "clicked",
			 G_CALLBACK(on_button_btn_close), this);

	setup_fields();

	gtk_window_present(GTK_WINDOW(window));
}

frm_config::~frm_config()
{
}

void frm_config::setup_fields()
{
	GtkEntry *e = (GtkEntry *)
			gtk_builder_get_object(gb, "e_latex_invoice_title");

	string invoice_title = config::get().get_string("latex.invoice_title");

	gtk_entry_buffer_set_text(gtk_entry_get_buffer(e),
			invoice_title.c_str(), invoice_title.size());
}

void frm_config::on_button_btn_save(GtkWidget *widget, gpointer data)
{
	frm_config *f = (frm_config *)data;
	GtkEntry *e = (GtkEntry *)
		gtk_builder_get_object(f->gb, "e_latex_invoice_title");

	string invoice_title =
		gtk_entry_buffer_get_text(gtk_entry_get_buffer(e));

	config::get().update_option("latex", "invoice_title",
				    invoice_title.c_str());

	f->message("config options saved.");
}

void frm_config::on_button_btn_close(GtkWidget *widget, gpointer data)
{
	frm_config *f = (frm_config *)data;

	gtk_window_close(f->window);
}
