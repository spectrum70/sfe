#ifndef frm_owner_hh
#define frm_owner_hh

#include <gtk/gtk.h>
#include "frm.hh"

struct frm_owner : public frm
{
	frm_owner(GtkWindow *parent);
	~frm_owner();

	static void on_button_btn_cancel(GtkWidget *widget, gpointer data);
	static void on_button_btn_save(GtkWidget *widget, gpointer data);
};

#endif /* frm_owner */
