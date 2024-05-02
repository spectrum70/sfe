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

	typedef void (*gtk_callback)(GtkWidget*, gpointer);

	void update_subtotal(frm_invoice *f, GtkWidget *widget, int i);
	void update_total(frm_invoice *f);
	void connect_signal_set(GtkEntry **p, gtk_callback callback);
	void normalize_name(string &name);
	int load_invoice(frm_invoice *f, const char *file_name);
	void get_date_value(frm_invoice *f, int &year, int &month, int &day);
	string get_date(frm_invoice *f);
	string get_date_xml(frm_invoice *f);
	string get_ente_emittente(const string& country);
	string try_to_set_progressivo(frm_invoice *f);
	void set_date(frm_invoice *f, const string &date);
	static void on_button_btn_load(GtkWidget *widget, gpointer data);
	static void on_button_btn_save(GtkWidget *widget, gpointer data);
	static void on_button_btn_gen_cart(GtkWidget *widget, gpointer data);
	static void on_button_btn_gen_xml(GtkWidget *widget, gpointer data);
	static void on_button_btn_close(GtkWidget *widget, gpointer data);
	static void on_key_press_qty(GtkWidget *widget, gpointer data);
	static void on_key_press_value(GtkWidget *widget, gpointer data);
	static void on_file_open(GObject *obj, GAsyncResult *ares,
				 gpointer data);
	static void on_file_save(GObject *obj, GAsyncResult *ares,
				 gpointer data);

private:
	GtkBuilder *gb;
	GtkDropDown *dd_rf;
	GtkDropDown *dd_clienti;
	GtkEntry *e_number, *e_codice_univoco;
	GtkCheckButton *cb_bollo;
	GtkLabel *l_total;

	GtkEntry *e_desc[10];
	GtkEntry *e_qty[10];
	GtkEntry *e_unit[10];
	GtkEntry *e_value[10];
	GtkLabel *l_subtotal[10];

	db_connector *db;

	int qty[10]{};
	float value[10]{};
	float subtot[10]{};
	float total{};

	string dati_iva, dati_cf, dati_iban, dati_swift;
};

#endif /* frm_invoice_hh */
