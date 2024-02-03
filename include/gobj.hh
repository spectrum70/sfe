#ifndef gobj_hh
#define gobj_hh

#include <gtk/gtk.h>

struct gobj {
	gobj(int x, int y);
	virtual ~gobj() {}

	virtual void draw(cairo_t *cr) = 0;

	int x;
	int y;
	int width;
	int height;

protected:
	cairo_surface_t *cs;
};

#endif /* gobj_hh */
