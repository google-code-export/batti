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
	$(CABWIZ) $(INF) /cpu ARM || sleep 1

zip: $(PRODUCT)-$(VERSION).zip

$(PRODUCT)-$(VERSION).zip:
	$(ZIP) $(PRODUCT)-$(VERSION).zip $(PRODUCT)-$(VERSION).cab

dist:
	$(PRODUCT)-$(VERSION).cab

# localiztion

cs:
	@cp -f ../lang/czech.lng default.lng
	$(CABWIZ) $(INF) /cpu ARM.Localized || sleep 1
	@mv $(PRODUCT).ARM.Localized.CAB $(PRODUCT)-$(VERSION)-$@.cab
	$(ZIP) -m $(PRODUCT)-$(VERSION)-$@.zip $(PRODUCT)-$(VERSION)-$@.cab
	@rm -f default.lng

pl:
	@cp -f ../lang/polish.lng default.lng
	$(CABWIZ) $(INF) /cpu ARM.Localized || sleep 1
	@mv $(PRODUCT).ARM.Localized.CAB $(PRODUCT)-$(VERSION)-$@.cab
	$(ZIP) -m $(PRODUCT)-$(VERSION)-$@.zip $(PRODUCT)-$(VERSION)-$@.cab
	@rm -f default.lng

de:
	@cp -f ../lang/german.lng default.lng
	$(CABWIZ) $(INF) /cpu ARM.Localized || sleep 1
	@mv $(PRODUCT).ARM.Localized.CAB $(PRODUCT)-$(VERSION)-$@.cab
	$(ZIP) -m $(PRODUCT)-$(VERSION)-$@.zip $(PRODUCT)-$(VERSION)-$@.cab
	@rm -f default.lng

se:
	@cp -f ../lang/swedish.lng default.lng
	$(CABWIZ) $(INF) /cpu ARM.Localized || sleep 1
	@mv $(PRODUCT).ARM.Localized.CAB $(PRODUCT)-$(VERSION)-$@.cab
	$(ZIP) -m $(PRODUCT)-$(VERSION)-$@.zip $(PRODUCT)-$(VERSION)-$@.cab
	@rm -f default.lng

clean:
	rm -f *.DAT
	rm -f *.CAB
	rm -f *.zip
