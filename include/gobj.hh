#ifndef gobj_hh
#define gobj_hh

#include <gtk/gtk.h>

struct gobj {
	gobj(int x, int y, uint32_t user_data = 0);
	virtual ~gobj() {}

	virtual void draw(cairo_t *cr) = 0;

	int x;
	int y;
	int width;
	int height;

	uint32_t user_data;
protected:
	cairo_surface_t *cs;
};

#endif /* gobj_hh */
