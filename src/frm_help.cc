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

#include "frm_help.hh"
#include "trace.hh"
#include "config.hh"

frm_help::frm_help(GtkWindow *parent)
{
	string path = config::get().get_path_res() + "/forms/frm_help.ui";

	gb = gtk_builder_new_from_file(path.c_str());
	if (!gb) {
		err << "cannot create gtk builder, exiting\n";
		exit(-1);
	}

	GObject *btn_close;
	window = (GtkWindow *)gtk_builder_get_object(gb, "frm_help");

	btn_close = gtk_builder_get_object(gb, "btn_close");

	g_signal_connect(btn_close, "clicked",
				  G_CALLBACK (on_button_btn_close), this);

	GtkLabel *l = (GtkLabel *)gtk_builder_get_object(gb, "l_version");

	string version = "v." VERSION;
	version += " ";
	version += GIT_VERSION;

	gtk_label_set_text(l, version.c_str());

	gtk_window_present(GTK_WINDOW(window));
}

frm_help::~frm_help()
{
}

void frm_help::on_button_btn_close(GtkWidget *widget, gpointer data)
{
	frm_help *f = (frm_help *)data;

	gtk_window_close(f->window);
}
