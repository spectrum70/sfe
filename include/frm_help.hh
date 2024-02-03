#ifndef frm_help_hh
#define frm_help_hh

#include <gtk/gtk.h>

#include "frm.hh"

struct frm_help : public frm
{
	frm_help(GtkWindow *parent);
	~frm_help();

private:
	void setup_fields();
	static void on_button_btn_close(GtkWidget *widget, gpointer data);

private:
	GtkBuilder *gb;
};

#endif /* frm_help_hh */
