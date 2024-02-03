#ifndef frm_customers_hh
#define frm_customers_hh

#include <gtk/gtk.h>
#include <gio/gio.h>

#include "frm.hh"
#include "db_connector.hh"

constexpr int MAX_HEADER_TITLE = 128;

struct c_col {
	GCallback cb_setup;
	GCallback cb_bind;
	int width;
	char name[MAX_HEADER_TITLE];
};

struct frm_customers : public frm
{
	frm_customers(GtkWindow *parent, db_connector *db);
	~frm_customers();

	void setup_fields(guint n);

private:
	GListStore *setup_customers_model();

	void select_combo_item(GtkDropDown *dd, const char *item);
	int query_from_fields(frm_customers *f, int idx);
	static void on_button_btn_add_new(GtkWidget *widget, gpointer data);
	static void on_button_btn_update(GtkWidget *widget, gpointer data);
	static void on_button_btn_remove(GtkWidget *widget, gpointer data);
	static void on_button_btn_close(GtkWidget *widget, gpointer data);

	int add_new_column(struct c_col *c);
private:
	GtkBuilder *gb;
	GtkDropDown *dd_countries;
	GtkColumnView *cv_clienti;
	db_connector *db;
	string pkey;
	int last;
	int selected;
};

#endif /* frm_customers */
