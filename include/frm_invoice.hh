#ifndef frm_invoice_hh
#define frm_invoice_hh

#include <gtk/gtk.h>
#include <gio/gio.h>

#include "frm.hh"
#include "db_connector.hh"

struct frm_invoice : public frm
{
	frm_invoice(GtkWindow *parent, db_connector *db);
	~frm_invoice();

private:
	void setup_fields();

	static void on_button_btn_save(GtkWidget *widget, gpointer data);
	static void on_button_btn_gen_cart(GtkWidget *widget, gpointer data);
	static void on_button_btn_close(GtkWidget *widget, gpointer data);
private:
	GtkDropDown *dd_rf;
	GtkDropDown *dd_clienti;
	GtkBuilder *gb;
	db_connector *db;
};

#endif /* frm_invoice_hh */
