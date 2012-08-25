EXTRA_DIST = en-US
pdf_guide = publish/en-US/$(PACKAGE_NAME)/$(PACKAGE_VERSION)/pdf/User_Guide/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-User_Guide-en-US.pdf
html_guide = publish/desktop

all: publish

publish:
	-publican build --formats=pdf,html-desktop --langs=en-US --publish

clean-local:
	-publican clean

install-data-local: publish
	$(INSTALL) -m 755 -d $(docdir)/PDF/en-US
	$(INSTALL) -m 755 $(pdf_guide) $(docdir)/PDF/en-US
	$(INSTALL) -m 755 -d $(docdir)/HTML/en-US
	-cp -r $(html_guide)/en-US/* $(docdir)/HTML/en-US

uninstall-local:
	$(RM) -r $(docdir)
