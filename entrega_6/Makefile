FILES := entregable1.c proyeccion1.c proyeccion2.c
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