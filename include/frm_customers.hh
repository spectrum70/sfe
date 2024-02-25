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

	void setup_fields(guint n);

private:
	GListStore *setup_customers_model();

	void select_combo_item(GtkDropDown *dd, const char *item);
	int query_from_fields(frm_customers *f, const string &key);
	static void on_button_btn_add_new(GtkWidget *widget, gpointer data);
	static void on_button_btn_update(GtkWidget *widget, gpointer data);
	static void on_button_btn_close(GtkWidget *widget, gpointer data);
private:
	GtkBuilder *gb;
	GtkDropDown *dd_countries;
	GtkColumnView *cv_clienti;
	db_connector *db;
	string pkey;
};

#endif /* frm_customers */
