CC=gcc

OUT = build/linux
SRC = src
INTERMEDIATE = build/linux/objs

SOURCES = $(filter-out src/tokens_generated_scanners.c,$(wildcard $(SRC)/*.c))
OBJECTS = $(patsubst $(SRC)/%.c,$(INTERMEDIATE)/%.o,$(SOURCES))

.PHONY: all
all: $(OUT)/carburetta

$(INTERMEDIATE)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $<

$(OUT)/carburetta: $(OBJECTS)
	$(CC) -o $(OUT)/carburetta $(OBJECTS)

.PHONY: clean
clean:
	@rm -rf $(OUT)
