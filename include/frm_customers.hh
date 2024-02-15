#ifndef frm_customers_hh
#define frm_customers_hh

#include <gtk/gtk.h>
#include <gio/gio.h>

#include "frm.hh"
#include "db_connector.hh"

struct frm_customers : public frm
{
	frm_customers(GtkWindow *parent, db_connector *db);
	~frm_customers();

private:
	void setup_fields();

	static void on_button_btn_cancel(GtkWidget *widget, gpointer data);
	static void on_button_btn_save(GtkWidget *widget, gpointer data);
private:
	GtkBuilder *gb;
	GtkDropDown *dd_countries;
	db_connector *db;
};

#endif /* frm_customers */
