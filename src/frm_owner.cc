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

frm_owner::frm_owner(GtkWindow *parent)
{
	GtkBuilder *gb = gtk_builder_new_from_file(
			"/home/angelo/dev-kernelspace/sfe/forms/frm_owner.ui");

	GObject* btn_cancel, *btn_save;

	window = (GtkWindow *)gtk_builder_get_object(gb, "frm_owner");
	btn_cancel = gtk_builder_get_object(gb, "btn_cancel");
	btn_save = gtk_builder_get_object(gb, "btn_save");

	g_signal_connect(btn_cancel, "clicked",
				  G_CALLBACK (on_button_btn_cancel), this);
	g_signal_connect(btn_save, "clicked",
				  G_CALLBACK (on_button_btn_save), this);

	gtk_window_present(GTK_WINDOW(window));
}

frm_owner::~frm_owner()
{
}

void frm_owner::on_button_btn_cancel(GtkWidget *widget, gpointer data)
{
	frm_owner *f = (frm_owner *)data;

	gtk_window_close(f->window);
}

void frm_owner::on_button_btn_save(GtkWidget *widget, gpointer data)
{
	frm_owner *f = (frm_owner *)data;

	gtk_window_close(f->window);
}
