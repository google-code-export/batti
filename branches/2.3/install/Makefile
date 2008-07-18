#
# Create installation files for Batti
# Copyright (c) 2008, David Andrs
#

PRODUCT=Batti
VERSION=2.3
INF=Batti.inf

CABWIZ="c:/program files/windows ce tools/wce420/pocket pc 2003/tools/cabwiz.exe"
ZIP="c:/program files/unxutils/usr/local/wbin/zip.exe"

all: cab zip

cab: $(PRODUCT)-$(VERSION).cab

$(PRODUCT)-$(VERSION).cab: $(PRODUCT).ARM.CAB
	mv -f $(PRODUCT).ARM.CAB $(PRODUCT)-$(VERSION).cab

$(PRODUCT).ARM.CAB: $(INF)
	$(CABWIZ) $(INF) /cpu ARM

zip: $(PRODUCT)-$(VERSION).cab
	$(ZIP) $(PRODUCT)-$(VERSION).zip $(PRODUCT)-$(VERSION).cab

dist:
	$(PRODUCT)-$(VERSION).cab

clean:
	rm -f *.DAT
	rm -f *.CAB
	rm -f *.zip
