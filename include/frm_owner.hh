#ifndef frm_owner_hh
#define frm_owner_hh

#include <gtk/gtk.h>
#include <gio/gio.h>

#include "frm.hh"
#include "db_connector.hh"

struct frm_owner : public frm
{
	frm_owner(GtkWindow *parent, db_connector *db);
	~frm_owner();

private:
	void setup_fields();
	void select_combo_item(GtkDropDown *dd, const char *item);

	static void on_button_btn_cancel(GtkWidget *widget, gpointer data);
	static void on_button_btn_save(GtkWidget *widget, gpointer data);
private:
	GtkBuilder *gb;
	GtkDropDown *dd_countries;
	db_connector *db;
};

#endif /* frm_owner */
