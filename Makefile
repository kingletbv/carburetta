CC=gcc

OUT = build/linux
SRC = src
INTERMEDIATE = build/linux/objs

SOURCES = $(filter-out src/tokens_generated_scanners.c,$(wildcard $(SRC)/*.c))
OBJECTS = $(patsubst $(SRC)/%.c,$(INTERMEDIATE)/%.o,$(SOURCES))

.PHONY: all
all: $(OUT)/carburetta $(OUT)/calc

$(INTERMEDIATE)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $<

$(OUT)/carburetta: $(OBJECTS)
	$(CC) -o $(OUT)/carburetta $(OBJECTS)

$(INTERMEDIATE)/calc/calc.c: $(OUT)/carburetta examples/calc/calc.crbt
	mkdir -p $(@D)
	$(OUT)/carburetta examples/calc/calc.crbt --c $(INTERMEDIATE)/calc/calc.c

$(OUT)/calc: $(INTERMEDIATE)/calc/calc.c
	$(CC) -o $(OUT)/calc $(INTERMEDIATE)/calc/calc.c

.PHONY: clean
clean:
	@rm -rf $(OUT)

.PHONY: install
install: all
	install $(OUT)/carburetta /usr/local/bin

.PHONY: uninstall
uninstall:
	rm /usr/local/bin/carburetta
