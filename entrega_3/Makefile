FILES := entregable_1.c entregable_2.c entregable_3.c
OBJS := $(FILES:.c=.o)
OUTPUTS := $(FILES:.c=.out)

ARGS := -Wall -Wextra -pedantic -lm

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
