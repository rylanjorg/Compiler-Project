# Specify the name of your Bison file
MAIN_BISON_FILE = Parser/parser.y
CONCAT_BISON_FILE = Parser/parser_with_included.y

# Specify the directory of the included Bison files
INCLUDED_BISON_FILES_DIR = ProductionRuleDefinitions

# Use a wildcard to get all .y files in the specified directory
INCLUDED_BISON_FILES := $(wildcard $(INCLUDED_BISON_FILES_DIR)/*.y)

# Bison flags
BISON_FLAGS = -t -v -d

# Target for inserting the included files
insert_included_files: $(MAIN_BISON_FILE) $(INCLUDED_BISON_FILES)
	@echo "Inserting content from included Bison files into $(MAIN_BISON_FILE)"
	@cp $(MAIN_BISON_FILE) $(CONCAT_BISON_FILE)
	@for file in $(INCLUDED_BISON_FILES); do \
		pattern=$$(basename $$file .y); \
		echo "Inserting content from $$file instead of '$$pattern: {/*insert*/};' in $(CONCAT_BISON_FILE)"; \
		sed -i -e "/^$$pattern: {\/\*insert\*\/}/r $$file" -e "/^$$pattern: {\/\*insert\*\/}/d" $(CONCAT_BISON_FILE); \
	done
	@echo "Content after insertion:"
	@cat $(CONCAT_BISON_FILE)

# Target for generating lexer
lex.yy.c: Lexer/lexer2.l parser.tab.h
	flex Lexer/lexer2.l

# Target for generating the parser
parser.tab.c parser.tab.h: $(MAIN_BISON_FILE)
	bison -t -v -d $(CONCAT_BISON_FILE)

# Target for building the parser
parser: lex.yy.c parser.tab.c parser.tab.h 
	gcc -o parser parser_with_included.tab.c lex.yy.c
	./parser InputCode/testProg.cmm

# Clean target
clean:
	@echo "Cleaning..."
	rm -f parser lexer parser_with_included.tab.c lex.yy.c parser_with_included.tab.h parser.output $(CONCAT_BISON_FILE)
	@for file in $(INCLUDED_BISON_FILES); do \
		echo "Cleaning $$file..."; \
		sed -i '/^\/\* Start of .* Zone \*\//,/^\/\* End of .* Zone \*\//d' $$file; \
	done
	ls -l

# Phony targets to avoid conflicts with file names
.PHONY: clean
