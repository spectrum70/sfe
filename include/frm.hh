#ifndef frm_hh
#define frm_hh

#include <gtk/gtk.h>
#include <string>

using std::string;

struct frm {
	frm();

protected:
	void message(const string &msg);

protected:
	GtkWindow* window;
};

#endif /* frm_hh */
