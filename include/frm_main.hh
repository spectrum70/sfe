#ifndef frm_main_hh
#define frm_main_hh

#include <gtk/gtk.h>
#include <vector>
#include <memory>
#include "gobj.hh"

using std::vector;
using std::shared_ptr;

struct frm_main {
	frm_main(GtkApplication *app);

	static void on_draw_event(GtkDrawingArea *area, cairo_t *cr,
				  int width, int height, gpointer data);
	static void on_mouse_drag_begin(GtkGesture *gesture,
					   double x, double y, gpointer data);
	static void on_mouse_drag(GtkGesture *gesture,
				   double x, double y, gpointer data);
	static void on_property_notification(GObject* self,
					     GParamSpec* pspec, gpointer data);
public:
	int gobj_sel;
	int gobj_drag_start_x;
	int gobj_drag_start_y;
	int width, height;
private:
	GtkWidget *drawing_area;
	vector<shared_ptr<gobj>> gobjs;
};

#endif /* frm_main_hh */
