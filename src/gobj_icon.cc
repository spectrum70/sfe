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

#include "gobj_icon.hh"

gobj_icon::gobj_icon(const string &name, const string &text, int x, int y,
		     uint32_t user_data)
: gobj(x, y, user_data), icon(name), text(text)
{
	cs = cairo_image_surface_create_from_png(icon.c_str());

	width = cairo_image_surface_get_width(cs);
	height = cairo_image_surface_get_height(cs);
}

void gobj_icon::draw(cairo_t *cr)
{
	cairo_text_extents_t extents;

	cairo_set_source_surface(cr, cs, x, y);
	cairo_paint(cr);

	cairo_set_source_rgb(cr, 1, 1, 0.1);
	cairo_select_font_face(cr, "Pursia",
				CAIRO_FONT_SLANT_NORMAL,
				CAIRO_FONT_WEIGHT_BOLD);

	cairo_set_font_size(cr, 13);
	cairo_text_extents(cr, text.c_str(), &extents);
	cairo_move_to(cr, x + (width >> 1) - (extents.width / 2),
			y + height + 12);
	cairo_show_text(cr, text.c_str());
}
