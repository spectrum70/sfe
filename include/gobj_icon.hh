#ifndef gobj_icon_hh
#define gobj_icon_hh

#include <gtk/gtk.h>
#include <string>
#include "gobj.hh"

using std::string;

struct gobj_icon: public gobj {
	gobj_icon(const string &icon, const string &text, int x, int y,
		  uint32_t user_data);

	virtual void draw(cairo_t *cr);
private:
	string icon;
	string text;
};

#endif /* gobj_icon_hh */
