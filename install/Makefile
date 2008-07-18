#
# Create installation files for Batti
# Copyright (c) 2008, David Andrs
#

PRODUCT=Batti
VERSION=2.3
INF=batti.inf

CABWIZ="c:/program files/windows ce tools/wce420/pocket pc 2003/tools/cabwiz.exe"
ZIP="c:/program files/unxutils/usr/local/wbin/zip.exe"

all: cab

cab: $(PRODUCT).ARM.CAB

$(PRODUCT).ARM.CAB: $(INF)
	$(CABWIZ) $(INF) /cpu ARM

zip: $(PRODUCT).ARM.CAB
	cp $(PRODUCT).ARM.CAB $(PRODUCT)-$(VERSION).ARM.cab
	$(ZIP) -m $(PRODUCT)-$(VERSION).zip $(PRODUCT)-$(VERSION).ARM.cab

dist:

clean:
	rm -f *.DAT
	rm -f *.CAB
	rm -f *.zip
