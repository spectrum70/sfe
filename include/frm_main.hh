#ifndef frm_main_hh
#define frm_main_hh

#include <gtk/gtk.h>
#include <vector>
#include <memory>
#include "gobj.hh"
#include "db_connector.hh"

using std::vector;
using std::shared_ptr;

struct frm_main {
	frm_main(GtkApplication *app);
	~frm_main();

	int get_object(int x, int y) {
		size_t i;

		y -= titlebar_height;

		for (i = 0; i < gobjs.size(); ++i) {
			if (x > gobjs[i]->x &&
				x < gobjs[i]->x + gobjs[i]->width &&
				y > gobjs[i]->y &&
				y < gobjs[i]->y + gobjs[i]->height) {
					return i;
			}
		}

		return -1;
	}

	static void on_draw_event(GtkDrawingArea *area, cairo_t *cr,
				  int width, int height, gpointer data);
	static void on_mouse_drag_begin(GtkGestureDrag *gesture,
					   gdouble x, gdouble y, gpointer data);
	static void on_mouse_drag(GtkGestureDrag *gesture,
				   gdouble x, gdouble y, gpointer data);
	static void on_property_notification(GObject* self,
					     GParamSpec* pspec, gpointer data);
	static void on_mouse_click(GtkGestureClick* self, gint n_press,
					gdouble x, gdouble y,
					gpointer data);
	static gboolean on_windows_close(GtkWindow* self, gpointer data);

public:
	int gobj_sel;
	int gobj_drag_start_x;
	int gobj_drag_start_y;
	int width, height;
	int titlebar_height;
private:
	GtkWidget *drawing_area;
	GtkWidget *window;
	db_connector db;
	vector<shared_ptr<gobj>> gobjs;
};

#endif /* frm_main_hh */
