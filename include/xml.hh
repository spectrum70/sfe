#ifndef xml_hh
#define xml_hh

#include <map>
#include <string>
#include <libxml/xmlwriter.h>

using std::map;
using std::string;

typedef map<string, string> fmap;

struct xml {
	xml();
	int create_xml(fmap &field, const string &name);

	void add_dati_trasmissione(xmlChar *paese, xmlChar *cf,
				   xmlChar *progressivo,  xmlChar *formnato,
				   xmlChar *codice_dest);
	void add_cedente_prestatore(xmlChar *paese, xmlChar *piva,
				    xmlChar *cf, xmlChar *denom, xmlChar *rf,
				    xmlChar *via, xmlChar *num, xmlChar *cap,
				    xmlChar *comune, xmlChar *provincia,
				    xmlChar *nazione);
	void add_cessionario_committente(xmlChar *paese, xmlChar *piva,
					 xmlChar *denom,
					 xmlChar *via, xmlChar *num,
					 xmlChar *cap, xmlChar *comune,
					 xmlChar *nazione);
	void add_dati_generali(xmlChar *tipo, xmlChar *divisa, xmlChar *data,
			       xmlChar *numero, xmlChar *bollo,
			       xmlChar *importo_bollo,
			       xmlChar *importo_totale);
	void add_dettagli_linee(fmap &fm);
	void add_dati_riepilogo(fmap &fm);

	int validate_xml(const string &fname);

private:
	xmlTextWriterPtr writer;
};

#endif /* xml_hh */
