AUTOMAKE_OPTIONS = -Wno-portability

GIT_VERSION = $(shell git describe --abbrev=8 --dirty --always --tags)
AM_CXXFLAGS = -DGIT_VERSION='"$(GIT_VERSION)"'

bin_PROGRAMS = sfe

invoicedir=$(datadir)/sfe/invoice
formsdir=$(datadir)/sfe/forms
iconsdir=$(datadir)/sfe/icons
themedir=$(datadir)/sfe/theme
xmldir=$(datadir)/sfe/xml

sfe_CPPFLAGS=-Iinclude
sfe_SOURCES = src/main.cc \
		src/frm.cc \
		src/frm_main.cc \
		src/frm_owner.cc \
		src/frm_customers.cc \
		src/frm_invoice.cc \
		src/frm_config.cc \
		src/frm_help.cc \
		src/gobj.cc \
		src/gobj_icon.cc \
		src/path.cc \
		src/config.cc \
		src/utils.cc \
		src/fs.cc \
		src/db_connector.cc \
		src/latex.cc \
		src/xml.cc

invoice_DATA = invoice/invoice.tex

forms_DATA = forms/frm_customers.ui \
		forms/frm_invoice.ui \
		forms/frm_owner.ui \
		forms/frm_config.ui \
		forms/frm_help.ui

icons_DATA = icons/icons8-books-folder-96.png \
		icons/icons8-documents-folder-96.png \
		icons/icons8-services-96.png \
		icons/icons8-user-folder-96.png \
		icons/icons8-help-96.png

theme_DATA = theme/theme.css

xml_DATA = xml/schema-fatturapa-1.2.2.xsd
