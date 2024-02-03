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

#include "frm_main.hh"
#include "frm_owner.hh"
#include "frm_customers.hh"
#include "frm_invoice.hh"
#include "frm_config.hh"
#include "frm_help.hh"
#include "config.hh"
#include "gobj_icon.hh"
#include "sections.hh"
#include "utils.hh"
#include "trace.hh"
#include "fs.hh"

using std::make_shared;
using std::iter_swap;

frm_main::frm_main(GtkApplication *app) : gobj_sel(-1), titlebar_height(-1)
{
	GtkGesture *gc, *gd;
	gobj *go;
	string path, icon, icon_text;
	int i, icon_x, icon_y;
	config &cfg = config::get();

	if (db.db_create()) {
		err << "cannot open/create database, exiting.\n";
		exit(1);
	}

	if (!cfg.load_config())
		cfg.setup_defaults();

	width = cfg.get_int("application.width");
	height = cfg.get_int("application.height");

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW (window),
			     "Simple FE - kernelspace (C) 2024");
	gtk_window_set_default_size(GTK_WINDOW (window), width, height);

	drawing_area = gtk_drawing_area_new();
	gtk_window_set_child(GTK_WINDOW(window), drawing_area);

	gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area),
				       on_draw_event, this, NULL);

	for (i = 0; i < s_id_max; ++i) {
		path = "sections.feature_" + tools::itoa(i);
		icon = cfg.get_path_res() + "/icons/" +
			cfg.get_string(string(path + ".icon").c_str());
		icon_text = cfg.get_string(string(path + ".icon_text").c_str());
		icon_x = cfg.get_int(string(path + ".icon_x").c_str());
		icon_y = cfg.get_int(string(path + ".icon_y").c_str());
		go = (gobj *)new gobj_icon(icon, icon_text, icon_x, icon_y, i);
		gobjs.push_back(shared_ptr<gobj>(go));
	}

	gc = gtk_gesture_click_new();
	g_signal_connect(gc, "pressed",
			 G_CALLBACK(on_mouse_click), this);

	gd = gtk_gesture_drag_new();
	g_signal_connect(gd, "drag-begin",
			 G_CALLBACK(on_mouse_drag_begin), this);
	g_signal_connect(gd, "drag-update",
			 G_CALLBACK(on_mouse_drag), this);
	g_signal_connect(G_OBJECT(window), "notify::default-width",
			 G_CALLBACK(on_property_notification), this);
	g_signal_connect(G_OBJECT(window), "notify::default-height",
			 G_CALLBACK(on_property_notification), this);
	g_signal_connect(G_OBJECT(window), "notify::is-active",
			 G_CALLBACK(on_property_notification), this);
	g_signal_connect(G_OBJECT(window), "close-request",
			 G_CALLBACK(on_windows_close), this);

	gtk_widget_add_controller(window, GTK_EVENT_CONTROLLER (gc));
	gtk_widget_add_controller(window, GTK_EVENT_CONTROLLER (gd));

	gtk_window_present(GTK_WINDOW(window));
}

frm_main::~frm_main()
{
}

gboolean frm_main::on_windows_close(GtkWindow* self, gpointer data)
{
	frm_main *f = (frm_main *)data;
	config &cfg = config::get();
	size_t i;
	string name;

	for (i = 0; i < f->gobjs.size(); ++i) {
		int id = f->gobjs[i]->user_data;
		name = "sections.feature_" + tools::itoa(id);
		cfg.update_option(name.c_str(), "icon_x", f->gobjs[i]->x);
		cfg.update_option(name.c_str(), "icon_y", f->gobjs[i]->y);
	}
	cfg.save_config();

	return false;
}

void frm_main::on_draw_event(GtkDrawingArea *area,
			     cairo_t *cr, int width, int height, gpointer data)
{
	size_t i;
	frm_main *f = (frm_main *)data;

	for (i = 0; i < f->gobjs.size(); ++i) {
		f->gobjs[i]->draw(cr);
	}
}

void frm_main::on_mouse_drag_begin(GtkGestureDrag *gesture,
				   gdouble x, gdouble y, gpointer data)
{
	/* Selecting object to move, if any, in the stack order they are now */
	int id, last;
	frm_main *f = (frm_main *)data;

	id = f->get_object(x, y);

	if (id != -1) {
		last = f->gobjs.size() - 1;

		if (id != last) {
			iter_swap(f->gobjs.begin() + id,
				  f->gobjs.begin() + last);
		}

		f->gobj_sel = last;
		f->gobj_drag_start_x = f->gobjs[last]->x;
		f->gobj_drag_start_y = f->gobjs[last]->y;

		return;
	}

	f->gobj_sel = -1;
}

void frm_main::on_mouse_drag(GtkGestureDrag *gesture,
			     gdouble x, gdouble y, gpointer data)
{
	frm_main *f = (frm_main *)data;

	if (f->gobj_sel != -1) {
		if (f->gobj_drag_start_x + x < 10)
			return;
		if (f->gobj_drag_start_y + y < 10)
			return;
		if (f->gobj_drag_start_x + x +
			f->gobjs[f->gobj_sel]->width + 20
			> f->width)
			return;
		if (f->gobj_drag_start_y + y +
			f->gobjs[f->gobj_sel]->height + 40
			> f->height)
			return;

		f->gobjs[f->gobj_sel]->x = f->gobj_drag_start_x + x;
		f->gobjs[f->gobj_sel]->y = f->gobj_drag_start_y + y;

		gtk_widget_queue_draw(GTK_WIDGET(f->drawing_area));
	}
}

void frm_main::on_property_notification(GObject* self,
					GParamSpec* pspec,
					gpointer data)
{
	frm_main *f = (frm_main *)data;

	if (f->titlebar_height == -1)
		f->titlebar_height =
			f->height - gtk_widget_get_height(f->drawing_area);

	f->width = gtk_widget_get_size(GTK_WIDGET(self),
				       GTK_ORIENTATION_HORIZONTAL);
	f->height = gtk_widget_get_size(GTK_WIDGET(self),
					GTK_ORIENTATION_VERTICAL);
}

void frm_main::on_mouse_click(GtkGestureClick* self,
				gint n_press,
				gdouble x,
				gdouble y,
				gpointer data)
{
	frm_main *f = (frm_main *)data;

	if (n_press == 2) {
		int id = f->get_object(x, y);
		int i = f->gobjs[id]->user_data;

		switch(i) {
		case s_id_azienda:
			new frm_owner(GTK_WINDOW(f->window), &f->db);
			break;
		case s_id_ana_cli:
			new frm_customers(GTK_WINDOW(f->window), &f->db);
			break;
		case s_id_em_fatt:
			new frm_invoice(GTK_WINDOW(f->window), &f->db);
			break;
		case s_id_config:
			new frm_config(GTK_WINDOW(f->window));
			break;
		case s_id_help:
			new frm_help(GTK_WINDOW(f->window));
			break;
		}
	}
}
