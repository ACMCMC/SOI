SUBDIRS_ENTREGAS := ./entrega_2

SUBDIRS_PRACTICAS := ./prac_1

$(SUBDIRS_ENTREGAS):
	$(MAKE) -C $@ -s

$(SUBDIRS_PRACTICAS):
	-$(MAKE) -C $@ -s

.PHONY: $(SUBDIRS_ENTREGAS) $(SUBDIRS_PRACTICAS)