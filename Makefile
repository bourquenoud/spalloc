CC := gcc
CFLAGS := -O2

AR := ar

BINDIR := bin/
BIN := $(BINDIR)libspalloc.a

SRCDIR := src/
SRC := $(shell find $(SRCDIR) -name *.c)

OBJDIR := obj/
OBJ := $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.o,$(SRC))

.PHONY: all
all: $(BIN)

#Build the library and create directories if required
$(BIN): $(OBJ)
	mkdir -p $$(dirname $@)
	$(AR) rcs $@ $^

#Build the objects and create directories if required (C)
$(OBJ): $(OBJDIR)%.o : $(SRCDIR)%.c
	mkdir -p $$(dirname $@)
	$(CC) $(CFLAGS) -c -o $@ $^

.PHONY: test
test:
	./test/run

.PHONY: install
install:
	@echo "Installation process not implemented yet"

#HACK: The clean is done this way because sometimes, for some reasons, some files won't get removed.
.PHONY: clean
clean:
	rm -rf .$(BINDIR)*
	rm -rf $(BINDIR)
	rm -rf .$(OBJDIR)*
	rm -rf $(OBJDIR)