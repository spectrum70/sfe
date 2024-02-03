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
#include "gobj_icon.hh"

static constexpr int def_width = 800;
static constexpr int def_height = 600;

using std::make_shared;
using std::iter_swap;

enum {
	COL_DISPLAY_NAME,
	COL_PIXBUF,
};

void frm_main::on_draw_event(GtkDrawingArea *area,
			      cairo_t        *cr,
			      int             width,
			      int             height,
			      gpointer        data)
{
	size_t i;
	frm_main *f = (frm_main *)data;

	for (i = 0; i < f->gobjs.size(); ++i) {
		f->gobjs[i]->draw(cr);
	}
}

void frm_main::on_mouse_drag_begin(GtkGesture *gesture,
				   double x, double y, gpointer data)
{
	/* Selecting object to move, if any, in the stack order they are now */
	size_t i, last;
	frm_main *f = (frm_main *)data;

	for (i = 0; i < f->gobjs.size(); ++i) {
		if (x > f->gobjs[i]->x &&
			x < f->gobjs[i]->x + f->gobjs[i]->width &&
			y > f->gobjs[i]->y &&
			y < f->gobjs[i]->y + f->gobjs[i]->height) {

			last = f->gobjs.size() - 1;

			if (i != last) {
				iter_swap(f->gobjs.begin() + i,
					  f->gobjs.begin() + last);
			}

			f->gobj_sel = last;
			f->gobj_drag_start_x = f->gobjs[last]->x;
			f->gobj_drag_start_y = f->gobjs[last]->y;

			return;
		}
	}

	f->gobj_sel = -1;
}

void frm_main::on_mouse_drag(GtkGesture *gesture,
			     double x, double y, gpointer data)
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

	f->width = gtk_widget_get_size(GTK_WIDGET(self),
				    GTK_ORIENTATION_HORIZONTAL);

	f->height = gtk_widget_get_size(GTK_WIDGET(self),
				     GTK_ORIENTATION_VERTICAL);
}

frm_main::frm_main(GtkApplication *app) :
gobj_sel(-1), width(def_width), height(def_height)
{
	GtkWidget *window;
	GtkGesture *gd;
	gobj *go;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW (window), "Simple FE");
	gtk_window_set_default_size(GTK_WINDOW (window), def_width, def_height);

	drawing_area = gtk_drawing_area_new();
	gtk_window_set_child(GTK_WINDOW(window), drawing_area);

	gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area),
					on_draw_event, this, NULL);

	go = (gobj *)new gobj_icon(
		"./icons/icons8-documents-folder-96.png",
		"Anagrafica clienti",
		40, 20);
	gobjs.push_back(shared_ptr<gobj>(go));

	go = (gobj *)new gobj_icon(
		"./icons/icons8-documents-folder-96.png",
		"Emissione fatture",
		200, 20);
	gobjs.push_back(shared_ptr<gobj>(go));

	go = (gobj *)new gobj_icon(
		"./icons/icons8-documents-folder-96.png",
		"Settings",
		360, 20);
	gobjs.push_back(shared_ptr<gobj>(go));

	gd = gtk_gesture_drag_new();
	g_signal_connect(gd, "drag-begin",
			 G_CALLBACK(on_mouse_drag_begin), this);
	g_signal_connect(gd, "drag-update",
			 G_CALLBACK(on_mouse_drag), this);
	g_signal_connect(G_OBJECT(window), "notify::default-width",
			 G_CALLBACK(on_property_notification), this);
	g_signal_connect(G_OBJECT(window), "notify::default-height",
			 G_CALLBACK(on_property_notification), this);

	gtk_widget_add_controller(window, GTK_EVENT_CONTROLLER (gd));

	gtk_window_present(GTK_WINDOW(window));
}
