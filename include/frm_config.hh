#ifndef frm_config_hh
#define frm_config_hh

#include <gtk/gtk.h>

#include "frm.hh"

struct frm_config: public frm
{
	frm_config(GtkWindow *parent);
	~frm_config();

	static void on_button_btn_save(GtkWidget *widget, gpointer data);
	static void on_button_btn_close(GtkWidget *widget, gpointer data);

private:
	void setup_fields();

private:
	GtkBuilder *gb;
};

#endif /* frm_config_hh */
