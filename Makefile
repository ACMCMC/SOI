SUBDIRS_ENTREGAS := ./entrega_2 ./entrega_3 ./entrega_4 ./practicas_optativas

SUBDIRS_PRACTICAS := ./prac_1 ./tests ./foro_hilos

$(SUBDIRS_ENTREGAS):
	$(MAKE) -C $@ -s

$(SUBDIRS_PRACTICAS):
	-$(MAKE) -C $@ -s

.PHONY: $(SUBDIRS_ENTREGAS) $(SUBDIRS_PRACTICAS)
