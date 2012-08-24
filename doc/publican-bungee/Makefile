all: $(brand_sources)
	-publican build --formats xml --langs all --publish

package: $(brand_sources)
	-publican package --binary --lang=en-US && find -name "*rpm" -type f

source-package: $(brand_sources)
	-publican package --lang=en-US && find -name "*rpm" -type f

clean:
	-publican clean

install:
	@if ! test -d publish ; \
	then \
		echo Please run \"make\" to compile before \"make install\". ; \
		exit 1 ; \
	fi
	@runner=`whoami` ; \
	if test $$runner != "root" ; \
	then \
		echo "You need to be root to install. Please run \"sudo make install\"" ; \
		exit 1 ; \
	fi
	@ if test -d /usr/share/publican/Common_Content/bungee ; \
	then \
		echo "Previous installation of bungee brand package found. Please run \"sudo make uninstall\"" and try again ; \
		exit 1 ; \
	fi
	-publican install_brand --path /usr/share/publican/Common_Content

uninstall:
	@runner=`whoami` ; \
	if test $$runner != "root" ; \
	then \
		echo "You need to be root to install. Please run \"sudo make install\"" ; \
		exit 1 ; \
	fi
	-rm -rf /usr/share/publican/Common_Content/bungee
