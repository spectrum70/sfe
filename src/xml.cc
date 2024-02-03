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

#include <libxml/xmlreader.h>
#include <libxml/encoding.h>

#include "xml.hh"
#include "config.hh"
#include "fs.hh"
#include "path.hh"
#include "utils.hh"
#include "trace.hh"

static const char xml_path[] = "/.sfe/invoices/xml";

using namespace tools;

xml::xml()
{
	xmlInitParser();
}

void xml::add_dati_riepilogo(fmap &fm)
{
	xmlTextWriterStartElement(writer, (xmlChar *)"DatiRiepilogo");

	xmlTextWriterStartElement(writer, (xmlChar *)"AliquotaIVA");
	xmlTextWriterWriteString(writer, (xmlChar *)"0.00");
	xmlTextWriterEndElement(writer);
	xmlTextWriterStartElement(writer, (xmlChar *)"Natura");
	xmlTextWriterWriteString(writer, (xmlChar *)"N2.1");
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"ImponibileImporto");
	xmlTextWriterWriteString(writer,
				 (xmlChar *)fm["importo_totale"].c_str());
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"Imposta");
	xmlTextWriterWriteString(writer, (xmlChar *)"0.00");
	xmlTextWriterEndElement(writer);

	xmlTextWriterEndElement(writer);
}

void xml::add_dettagli_linee(fmap &fm)
{
	char idx[3];

	for (int i = 1; i <= 10; ++i) {

		sprintf(idx, "%02x", i);

		if (fm[string("desc_") + idx] == "")
			break;

		xmlTextWriterStartElement(writer, (xmlChar *)"DettaglioLinee");

		xmlTextWriterStartElement(writer, (xmlChar *)"NumeroLinea");
		xmlTextWriterWriteString(writer, (xmlChar *)(itoa(i).c_str()));
		xmlTextWriterEndElement(writer);
		xmlTextWriterStartElement(writer, (xmlChar *)"Descrizione");
		xmlTextWriterWriteString(writer, (xmlChar *)
			(fm[string("desc_") + idx]).c_str());
		xmlTextWriterEndElement(writer);
		xmlTextWriterStartElement(writer, (xmlChar *)"Quantita");
		xmlTextWriterWriteString(writer, (xmlChar *)
			(fm[string("qty_") + idx]).c_str());
		xmlTextWriterEndElement(writer);
		xmlTextWriterStartElement(writer, (xmlChar *)"UnitaMisura");
		xmlTextWriterWriteString(writer, (xmlChar *)
			(fm[string("unit_") + idx]).c_str());
		xmlTextWriterEndElement(writer);
		xmlTextWriterStartElement(writer, (xmlChar *)"PrezzoUnitario");
		xmlTextWriterWriteString(writer, (xmlChar *)
			(fm[string("value_") + idx]).c_str());
		xmlTextWriterEndElement(writer);
		xmlTextWriterStartElement(writer, (xmlChar *)"PrezzoTotale");
		xmlTextWriterWriteString(writer, (xmlChar *)
			(fm[string("subtotal_") + idx]).c_str());
		xmlTextWriterEndElement(writer);
		xmlTextWriterStartElement(writer, (xmlChar *)"AliquotaIVA");
		xmlTextWriterWriteString(writer, (xmlChar *)"0.00");
		xmlTextWriterEndElement(writer);
		xmlTextWriterStartElement(writer, (xmlChar *)"Natura");
		xmlTextWriterWriteString(writer, (xmlChar *)"N2.1");
		xmlTextWriterEndElement(writer);

		xmlTextWriterEndElement(writer);
	}
}

void xml::add_dati_generali(xmlChar *tipo, xmlChar *divisa, xmlChar *data,
		       xmlChar *numero, xmlChar *bollo,
		       xmlChar *importo_bollo, xmlChar *importo_totale)
{
	xmlTextWriterStartElement(writer, (xmlChar *)"DatiGenerali");
	xmlTextWriterStartElement(writer, (xmlChar *)"DatiGeneraliDocumento");

	xmlTextWriterStartElement(writer, (xmlChar *)"TipoDocumento");
	xmlTextWriterWriteString(writer, tipo);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"Divisa");
	xmlTextWriterWriteString(writer, divisa);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"Data");
	xmlTextWriterWriteString(writer, data);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"Numero");
	xmlTextWriterWriteString(writer, numero);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"DatiBollo");

	xmlTextWriterStartElement(writer, (xmlChar *)"BolloVirtuale");
	xmlTextWriterWriteString(writer, bollo);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"ImportoBollo");
	xmlTextWriterWriteString(writer, importo_bollo);
	xmlTextWriterEndElement(writer);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"ImportoTotaleDocumento");
	xmlTextWriterWriteString(writer, importo_totale);
	xmlTextWriterEndElement(writer);

	xmlTextWriterEndElement(writer);
	xmlTextWriterEndElement(writer);
}

void xml::add_cessionario_committente(xmlChar *paese, xmlChar *piva,
				      xmlChar *denom,
				      xmlChar *via, xmlChar *num,
				      xmlChar *cap, xmlChar *comune,
				      xmlChar *nazione)
{
	xmlTextWriterStartElement(writer, (xmlChar *)"CessionarioCommittente");
	xmlTextWriterStartElement(writer, (xmlChar *)"DatiAnagrafici");

	xmlTextWriterStartElement(writer, (xmlChar *)"IdFiscaleIVA");

	xmlTextWriterStartElement(writer, (xmlChar *)"IdPaese");
	xmlTextWriterWriteString(writer, paese);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"IdCodice");
	xmlTextWriterWriteString(writer, piva);
	xmlTextWriterEndElement(writer);

	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"Anagrafica");

	xmlTextWriterStartElement(writer, (xmlChar *)"Denominazione");
	xmlTextWriterWriteString(writer, denom);
	xmlTextWriterEndElement(writer);

	xmlTextWriterEndElement(writer);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"Sede");

	xmlTextWriterStartElement(writer, (xmlChar *)"Indirizzo");
	xmlTextWriterWriteString(writer, via);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"NumeroCivico");
	xmlTextWriterWriteString(writer, num);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"CAP");
	xmlTextWriterWriteString(writer, cap);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"Comune");
	xmlTextWriterWriteString(writer, comune);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"Nazione");
	xmlTextWriterWriteString(writer, nazione);
	xmlTextWriterEndElement(writer);

	xmlTextWriterEndElement(writer);
	xmlTextWriterEndElement(writer);
}

void xml::add_cedente_prestatore(xmlChar *paese, xmlChar *piva,
				 xmlChar *cf, xmlChar *denom, xmlChar *rf,
				 xmlChar *via, xmlChar *num, xmlChar *cap,
				 xmlChar *comune, xmlChar *provincia,
				 xmlChar *nazione)
{
	xmlTextWriterStartElement(writer, (xmlChar *)"CedentePrestatore");
	xmlTextWriterStartElement(writer, (xmlChar *)"DatiAnagrafici");

	xmlTextWriterStartElement(writer, (xmlChar *)"IdFiscaleIVA");

	xmlTextWriterStartElement(writer, (xmlChar *)"IdPaese");
	xmlTextWriterWriteString(writer, paese);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"IdCodice");
	xmlTextWriterWriteString(writer, piva);
	xmlTextWriterEndElement(writer);

	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"CodiceFiscale");
	xmlTextWriterWriteString(writer, cf);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"Anagrafica");

	xmlTextWriterStartElement(writer, (xmlChar *)"Denominazione");
	xmlTextWriterWriteString(writer, denom);
	xmlTextWriterEndElement(writer);

	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"RegimeFiscale");
	xmlTextWriterWriteString(writer, rf);
	xmlTextWriterEndElement(writer);

	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"Sede");

	xmlTextWriterStartElement(writer, (xmlChar *)"Indirizzo");
	xmlTextWriterWriteString(writer, via);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"NumeroCivico");
	xmlTextWriterWriteString(writer, num);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"CAP");
	xmlTextWriterWriteString(writer, cap);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"Comune");
	xmlTextWriterWriteString(writer, comune);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"Provincia");
	xmlTextWriterWriteString(writer, provincia);
	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"Nazione");
	xmlTextWriterWriteString(writer, nazione);
	xmlTextWriterEndElement(writer);

	xmlTextWriterEndElement(writer);

	xmlTextWriterEndElement(writer);
}

void xml::add_dati_trasmissione(xmlChar *paese, xmlChar *cf,
				xmlChar *progressivo,  xmlChar *formato,
				xmlChar *codice_dest)
{
	xmlTextWriterStartElement(writer, (xmlChar *)"DatiTrasmissione");
	xmlTextWriterStartElement(writer, (xmlChar *)"IdTrasmittente");
	xmlTextWriterStartElement(writer, (xmlChar *)"IdPaese");
	xmlTextWriterWriteString(writer, paese);
	xmlTextWriterEndElement(writer);
	xmlTextWriterStartElement(writer, (xmlChar *)"IdCodice");
	xmlTextWriterWriteString(writer, cf);
	xmlTextWriterEndElement(writer);
	xmlTextWriterEndElement(writer);
	xmlTextWriterStartElement(writer, (xmlChar *)"ProgressivoInvio");
	xmlTextWriterWriteString(writer, progressivo);
	xmlTextWriterEndElement(writer);
	xmlTextWriterStartElement(writer, (xmlChar *)"FormatoTrasmissione");
	xmlTextWriterWriteString(writer, formato);
	xmlTextWriterEndElement(writer);
	xmlTextWriterStartElement(writer, (xmlChar *)"CodiceDestinatario");
	xmlTextWriterWriteString(writer, codice_dest);
	xmlTextWriterEndElement(writer);
	xmlTextWriterEndElement(writer);
}

int xml::validate_xml()
{
	xmlDocPtr doc;
	xmlSchemaPtr schema;
	xmlSchemaParserCtxtPtr schema_parser_ctxt;
	string path = config::get().get_path_res() +
			"/xml/schema-fatturapa-1.2.2.xsd";
	int ret = -1;
	xmlSchemaValidCtxtPtr valid_ctxt;

	if ((schema_parser_ctxt = xmlSchemaNewParserCtxt(path.c_str()))) {
		schema = xmlSchemaParse(schema_parser_ctxt);
		xmlSchemaFreeParserCtxt(schema_parser_ctxt);

		if (schema) {
			valid_ctxt = xmlSchemaNewValidCtxt(schema);

			doc = xmlReadFile("test.xml", NULL, 0);

			if (doc == NULL) {
				err << "Could not parse file, exiting\n";
				return -1;
			}

			ret = xmlSchemaValidateDoc(valid_ctxt, doc);

			if (ret == 0) {
				mesg << "file validated !\n";
			}

			xmlFreeDoc(doc);
		}
	}

	return ret;
}

int xml::create_xml(fmap &fm, const string &name)
{
	fs fs;
	path p;
	string dir, fname;

	dir = p.get_home_path() + xml_path;

	if (!fs.is_dir(dir.c_str())) {
		fs.create_directory(dir.c_str());
	}

	fname = dir + "/" + name;

	writer = xmlNewTextWriterFilename(fname.c_str(), 0);
	if (writer == NULL) {
		err << "testXmlwriterFilename: Error creating the xml writer\n";
		return -1;
	}

	xmlTextWriterSetIndent(writer, 1);
	xmlTextWriterSetIndentString(writer, (xmlChar *)"    ");

	xmlTextWriterStartDocument(writer, NULL, "UTF-8", "yes");

	xmlTextWriterStartElementNS(writer,
				    (xmlChar *)"p",
				    (xmlChar *)"FatturaElettronica",
				    (xmlChar *)
	"http://ivaservizi.agenziaentrate.gov.it/docs/xsd/fatture/v1.2");

	xmlTextWriterWriteAttribute(writer, (xmlChar *)"xmlns:ds",
			(xmlChar *)"http://www.w3.org/2000/09/xmldsig#");

	xmlTextWriterWriteAttribute(writer, (xmlChar *)"versione",
				    (xmlChar *)"FPR12");
	xmlTextWriterWriteAttribute(writer, (xmlChar *)"SistemaEmittente",
				    (xmlChar *)"FEL18 v213");

	xmlTextWriterStartElement(writer,
				  (xmlChar *)"FatturaElettronicaHeader");

	if (!fm.count("paese") ||
		!fm.count("codice") ||
		!fm.count("progressivo") ||
		!fm.count("formato") ||
		!fm.count("codice_dest"))
	{
		err << "dati trasmissione: wrong input params, exiting.\n";
		return -1;
	}

	add_dati_trasmissione((xmlChar *)fm["paese"].c_str(),
			      (xmlChar *)fm["codice"].c_str(),
			      (xmlChar *)fm["progressivo"].c_str(),
			      (xmlChar *)fm["formato"].c_str(),
			      (xmlChar *)fm["codice_dest"].c_str());

	if (!fm.count("paese") ||
		!fm.count("piva") ||
		!fm.count("codice") ||
		!fm.count("denom") ||
		!fm.count("rf") ||
		!fm.count("via") ||
		!fm.count("num") ||
		!fm.count("cap") ||
		!fm.count("comune") ||
		!fm.count("provincia") ||
		!fm.count("nazione"))
	{
		err << "cedente prestatore: wrong input params, exiting.\n";
		return -1;
	}

	add_cedente_prestatore((xmlChar *)fm["paese"].c_str(),
			       (xmlChar *)fm["piva"].c_str(),
			       (xmlChar *)fm["codice"].c_str(),
			       (xmlChar *)fm["denom"].c_str(),
			       (xmlChar *)fm["rf"].c_str(),
			       (xmlChar *)fm["via"].c_str(),
			       (xmlChar *)fm["num"].c_str(),
			       (xmlChar *)fm["cap"].c_str(),
			       (xmlChar *)fm["comune"].c_str(),
			       (xmlChar *)fm["provincia"].c_str(),
			       (xmlChar *)fm["nazione"].c_str());

	if (!fm.count("cc_paese") ||
		!fm.count("cc_piva") ||
		!fm.count("cc_denom") ||
		!fm.count("cc_via") ||
		!fm.count("cc_num") ||
		!fm.count("cc_cap") ||
		!fm.count("cc_comune") ||
		!fm.count("cc_nazione"))
	{
		err << "cedente prestatore: wrong input params, exiting.\n";
		return -1;
	}

	add_cessionario_committente((xmlChar *)fm["cc_paese"].c_str(),
				    (xmlChar *)fm["cc_piva"].c_str(),
				    (xmlChar *)fm["cc_denom"].c_str(),
				    (xmlChar *)fm["cc_via"].c_str(),
				    (xmlChar *)fm["cc_num"].c_str(),
				    (xmlChar *)fm["cc_cap"].c_str(),
				    (xmlChar *)fm["cc_comune"].c_str(),
				    (xmlChar *)fm["cc_nazione"].c_str());

	xmlTextWriterEndElement(writer);

	xmlTextWriterStartElement(writer, (xmlChar *)"FatturaElettronicaBody");

	if (!fm.count("tipo") ||
		!fm.count("divisa") ||
		!fm.count("data") ||
		!fm.count("numero") ||
		!fm.count("bollo") ||
		!fm.count("importo_bollo") ||
		!fm.count("importo_totale"))
	{
		err << "cedente prestatore: wrong input params, exiting.\n";
		return -1;
	}

	add_dati_generali((xmlChar *)fm["tipo"].c_str(),
			  (xmlChar *)fm["divisa"].c_str(),
			  (xmlChar *)fm["data"].c_str(),
			  (xmlChar *)fm["numero"].c_str(),
			  (xmlChar *)fm["bollo"].c_str(),
			  (xmlChar *)fm["importo_bollo"].c_str(),
			  (xmlChar *)fm["importo_totale"].c_str());


	xmlTextWriterStartElement(writer, (xmlChar *)"DatiBeniServizi");
	add_dettagli_linee(fm);
	add_dati_riepilogo(fm);
	xmlTextWriterEndElement(writer);

	xmlTextWriterEndElement(writer);

	xmlTextWriterEndElement(writer);

	xmlTextWriterEndElement(writer);

	xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);

	validate_xml();

	return 0;
}
