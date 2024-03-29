CC := gcc
CFLAGS := -O2

AR := ar

BINDIR := bin/
BIN := $(BINDIR)libspalloc.a

SRCDIR := src/
SRC := $(shell find $(SRCDIR) -name *.c)

OBJDIR := obj/
OBJ := $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.o,$(SRC))

LIB_HEADER := $(shell find $(SRCDIR) -name spalloc.h)
PKGCONFIG_FILE := spalloc.pc

#This also need to be changed in the .pc file
LIB_INSTALL_PATH := /usr/lib/
HEADER_INSTALL_PATH := /usr/include/spalloc/
PKGCONFIG_INSTALL_PATH := /usr/lib/pkgconfig/
PKGCONFIG_INSTALL_FILE := $(PKGCONFIG_INSTALL_PATH)spalloc.pc

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
	cp $(PKGCONFIG_FILE) $(PKGCONFIG_INSTALL_FILE)
	mkdir -p $(LIB_INSTALL_PATH)
	cp $(BIN) $(LIB_INSTALL_PATH)
	mkdir -p $(HEADER_INSTALL_PATH)
	cp $(LIB_HEADER) $(HEADER_INSTALL_PATH)

#HACK: The clean is done this way because sometimes, for some reasons, some files won't get removed.
.PHONY: clean
clean:
	rm -rf .$(BINDIR)*
	rm -rf $(BINDIR)
	rm -rf .$(OBJDIR)*
	rm -rf $(OBJDIR)