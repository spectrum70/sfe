/*
 * Copyright (C) 2024 Angelo Dureghello <angelo@kernel-space.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "frm_customers.hh"
#include "countries.hh"
#include "trace.hh"

static constexpr int max_fields = 9;

static char const *fields[max_fields] = {
	"e_ragione_sociale",
	"e_partita_iva",
	"f_stato",
	"e_indirizzo",
	"e_n_civico",
	"e_comune",
	"e_provincia",
	"e_email",
	"e_telefono",
};

G_DECLARE_FINAL_TYPE (CustomerItem, customer_item, CUSTOMER, ITEM, GObject)
#define CUSTOMER_TYPE_ITEM (customer_item_get_type())

struct _CustomerItem
{
	GObject parent;
	const char *name;
	const char *country;
	const char *iva;
	const char *address;
};
G_DEFINE_TYPE (CustomerItem, customer_item, G_TYPE_OBJECT)

static void customer_item_init(CustomerItem *item) {}
static void customer_item_class_init(CustomerItemClass *cl) {}

static const char* customer_item_get_name(CustomerItem *item)
{ return item->name; }
static const char* customer_item_get_country(CustomerItem *item)
{ return item->country; }
static const char* customer_item_get_iva(CustomerItem *item)
{ return item->iva; }
static const char* customer_item_get_address(CustomerItem *item)
{ return item->address; }

static void setup_cb(GtkSignalListItemFactory *factory,GObject  *listitem)
{
	GtkWidget *label = gtk_label_new(NULL);
	gtk_list_item_set_child(GTK_LIST_ITEM(listitem),label);
}

static void bind_name_cb(GtkSignalListItemFactory *factory,
			 GtkListItem *listitem)
{
	GtkWidget *label = gtk_list_item_get_child(listitem);
	GObject *item =
		(GObject *)gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
	const char *string = customer_item_get_name(CUSTOMER_ITEM(item));
	gtk_label_set_text(GTK_LABEL (label), string);
}

static void bind_country_cb(GtkSignalListItemFactory *factory,
			 GtkListItem *listitem)
{
	GtkWidget *label = gtk_list_item_get_child(listitem);
	GObject *item =
		(GObject *)gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
	const char *string = customer_item_get_country(CUSTOMER_ITEM(item));
	gtk_label_set_text(GTK_LABEL (label), string);
}

static void bind_iva_cb(GtkSignalListItemFactory *factory,
			    GtkListItem *listitem)
{
	GtkWidget *label = gtk_list_item_get_child(listitem);
	GObject *item =
		(GObject *)gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
	const char *string = customer_item_get_iva(CUSTOMER_ITEM(item));
	gtk_label_set_text(GTK_LABEL (label), string);
}

static void bind_address_cb(GtkSignalListItemFactory *factory,
			GtkListItem *listitem)
{
	GtkWidget *label = gtk_list_item_get_child(listitem);
	GObject *item =
		(GObject *)gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
	const char *string = customer_item_get_address(CUSTOMER_ITEM(item));
	gtk_label_set_text(GTK_LABEL (label), string);
}

static void activate(GtkColumnView *self, guint n, gpointer p)
{
	printf("ACTIVATE\n");
}

frm_customers::frm_customers(GtkWindow *parent, db_connector *db) :
db(db), pkey("1")
{
	GtkColumnViewColumn *column;
	GtkSingleSelection *selection;
	GtkListItemFactory *factory;
	GtkWidget *scrolled_window;
	GListStore *store;
	GtkFrame *f;
	GObject* btn_add_new, *btn_update, *btn_close;

	gb = gtk_builder_new_from_file(
		"/home/angelo/dev-kernelspace/sfe/forms/frm_customers.ui");

	window = (GtkWindow *)gtk_builder_get_object(gb, "frm_customers");
	btn_add_new = gtk_builder_get_object(gb, "btn_add_new");
	btn_update = gtk_builder_get_object(gb, "btn_update");
	btn_close = gtk_builder_get_object(gb, "btn_close");

	g_signal_connect(btn_add_new, "clicked",
				G_CALLBACK(on_button_btn_add_new), this);
	g_signal_connect(btn_update, "clicked",
				G_CALLBACK(on_button_btn_update), this);
	g_signal_connect(btn_close, "clicked",
				G_CALLBACK(on_button_btn_close), this);

	f = (GtkFrame *)gtk_builder_get_object(gb, "f_stato");
	dd_countries = (GtkDropDown *)gtk_drop_down_new_from_strings(countries);
	gtk_frame_set_child(f, GTK_WIDGET(dd_countries));

	f = (GtkFrame *)gtk_builder_get_object(gb, "f_clienti");
	store = setup_customers_model();
	selection = gtk_single_selection_new(G_LIST_MODEL(store));
	cv_clienti = (GtkColumnView *)gtk_column_view_new(
					GTK_SELECTION_MODEL(selection));

	gtk_widget_set_size_request(GTK_WIDGET(f), -1, 200);

	scrolled_window = gtk_scrolled_window_new();
	gtk_frame_set_child(f, GTK_WIDGET(scrolled_window));
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window),
					GTK_WIDGET(cv_clienti));

	gtk_column_view_set_show_column_separators(GTK_COLUMN_VIEW(cv_clienti),
						   TRUE);

	factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(setup_cb), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_name_cb), NULL);
	column = gtk_column_view_column_new("Ragione sociale", factory);
	gtk_column_view_column_set_resizable(column, 1);
	gtk_column_view_column_set_fixed_width(column, 300);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(cv_clienti), column);
	g_object_unref (column);

	factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(setup_cb), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_iva_cb), NULL);
	column = gtk_column_view_column_new("Partita IVA", factory);
	gtk_column_view_column_set_resizable(column, 1);
	gtk_column_view_column_set_fixed_width(column, 150);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(cv_clienti), column);
	g_object_unref (column);

	factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(setup_cb), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_country_cb), NULL);
	column = gtk_column_view_column_new("Paese", factory);
	gtk_column_view_column_set_resizable(column, 1);
	gtk_column_view_column_set_fixed_width(column, 150);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(cv_clienti), column);
	g_object_unref (column);

	factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(setup_cb), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_address_cb), NULL);
	column = gtk_column_view_column_new("Indirizzo", factory);
	gtk_column_view_column_set_resizable(column, 1);
	gtk_column_view_column_set_fixed_width(column, 300);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(cv_clienti), column);
	g_object_unref (column);

	g_signal_connect(cv_clienti, "activate", G_CALLBACK(activate), NULL);

	gtk_window_present(GTK_WINDOW(window));

	setup_fields();
}

frm_customers::~frm_customers()
{
}

GListStore *frm_customers::setup_customers_model()
{
	CustomerItem *item;
	GListStore *store;
	string query;
	rlist rl;
	rlist::iterator it;

	store = g_list_store_new(G_TYPE_OBJECT);

	query = "SELECT * FROM anagrafica_clienti";

	if (db->db_query_with_result(query, rl))
		err << "database query error\n";

	for (it = rl.begin(); it != rl.end(); ++it) {
		vector<string> &v = (*it);

		item = (CustomerItem *)g_object_new(CUSTOMER_TYPE_ITEM, NULL);

		item->name = g_strdup(v[1].c_str());
		item->iva = g_strdup(v[2].c_str());
		item->country = g_strdup(v[3].c_str());
		item->address = g_strdup(v[4].c_str());

		g_list_store_append(store, item);
	}

	return store;
}

void frm_customers::select_combo_item(GtkDropDown *dd, const char *item)
{
	int i;

	for (i = 0; countries[i]; ++i) {
		if (strcmp(item, countries[i]) == 0) {
			gtk_drop_down_set_selected(dd, i);
			break;
		}
	}
}

void frm_customers::setup_fields()
{
	string query;
	rlist rl;
	GObject *entry;
	string data;
	int i;

	/*
	 * Displaying the first, always
	 */
	query = "SELECT * FROM anagrafica_clienti LIMIT 1";

	if (db->db_query_with_result(query, rl))
		err << "database query error\n";

	if (rl.size() == 0)
		return;

	for (i = 0; i < max_fields; ++i) {
		data = rl.front()[i + 1];
		entry = gtk_builder_get_object(gb, fields[i]);

		if (i == 2) {
			select_combo_item(dd_countries, data.c_str());
			continue;
		}

		gtk_entry_buffer_set_text(
			gtk_entry_get_buffer(GTK_ENTRY(entry)),
					  data.c_str(), data.size());
	}
}

int frm_customers::query_from_fields(frm_customers *f, const string &key)
{
	string query;
	int i;
	GObject *entry;

	query = "INSERT OR REPLACE INTO anagrafica_clienti VALUES ("
		+ key + " ";

	for (i = 0; i < max_fields; ++i) {
		query += ", '";

		entry = gtk_builder_get_object(f->gb, fields[i]);
		if (i == 2) {
			GtkStringObject *so = (GtkStringObject *)
			gtk_drop_down_get_selected_item(
				f->dd_countries);
			query += gtk_string_object_get_string(so);
		} else {
			query += gtk_entry_buffer_get_text(
				gtk_entry_get_buffer(GTK_ENTRY(entry)));
		}

		query += "'";
	}

	query += ");";

	if (f->db->db_query_with_result(query)) {
		err << "database query error\n";
		return -1;
	}

	return 0;
}

void frm_customers::on_button_btn_add_new(GtkWidget *widget, gpointer data)
{
	frm_customers *f = (frm_customers *)data;

	if (f->query_from_fields(f, "2")) {
		f->message("SWLite query error");
		return;
	}

	string mesg("Item properly ");
	mesg += ((f->pkey == "2") ? "inserted" : "updated");

	f->message(mesg);
}

void frm_customers::on_button_btn_update(GtkWidget *widget, gpointer data)
{
}

void frm_customers::on_button_btn_close(GtkWidget *widget, gpointer data)
{
	frm_customers *f = (frm_customers *)data;

	gtk_window_close(f->window);
}
