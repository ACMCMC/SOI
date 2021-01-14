FILES := 1_nums_loteria.c 2_nums_loteria_clone.c 7_hijos_zombies.c 8_ej4_4_procesos.c 9_ej4_4_hilos.c
OBJS := $(FILES:.c=.o)
OUTPUTS := $(FILES:.c=.out)

ARGS := -Wall -Wextra -pedantic

all: $(OUTPUTS)

$(OUTPUTS): $(OBJS)

$(OUTPUTS): $(OBJS)
	gcc $(@:.out=.o) -o $@ $(ARGS)

%.o: %.c
	gcc -o $@ -c $< $(ARGS)

clean:
	-find . -type f -name '*.o' -delete
	-find . -type f -name '*.out' -delete
	-rm $(OUTPUTS:.out=)