pdf_doc = tmp/en-US/pdf/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-User_Guide-en-US.pdf
publican_sources = en-US/User_Guide.ent en-US/Book_Info.xml en-US/User_Guide.xml en-US/Author_Group.xml en-US/Revision_History.xml en-US/Preface.xml en-US/Chapter.xml
pdfdir = $(docdir)
dist_pdf_DATA = $(pdf_doc)

$(pdf_doc): $(publican_sources)
	-publican build --formats=pdf --langs=en-US

clean-local:
	-publican clean

install-data-local:
	$(INSTALL) -m 755 -d $(docdir)
	$(INSTALL) -m 755 $(pdf_doc) $(docdir)

#uninstall-local:
#	$(RM) -r $(docdir)
