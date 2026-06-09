# =============================================================================
# Projet Sudoku - ISEN3 2025-2026
# Encadré par Antoine Pirog et Frédéric Chatrie-Roudier
# =============================================================================

# --- Compilateur et flags ---
CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Werror -pedantic -O2 -g
LDFLAGS =

# --- Répertoires ---
OBJ_DIR  = obj
BIN_DIR  = bin
TEST_DIR = tests

# --- Exécutables ---
TARGET      = $(BIN_DIR)/sudoku
TEST_TARGET = $(BIN_DIR)/tests

# --- Sources principales (hors main.c) ---
SRCS = entity/grid.c          \
       entity/subset.c        \
       entity/sudoku_types.c  \
       entity/generator.c     \
       repository/io.c        \
       service/service_solver.c \
       console/cli.c

MAIN_SRC = main.c

# --- Sources de tests ---
TEST_SRCS = $(TEST_DIR)/test_main.c         \
            $(TEST_DIR)/test_grid.c         \
            $(TEST_DIR)/test_subset.c       \
            $(TEST_DIR)/test_solver.c       \
            $(TEST_DIR)/test_io.c

# --- Objets ---
OBJS      = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))
MAIN_OBJ  = $(OBJ_DIR)/main.o
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/test_%.o, $(TEST_SRCS))

# --- Include dirs ---
INCLUDES = -Ientity -Irepository -Iservice -Iconsole

# =============================================================================
# Cibles principales
# =============================================================================

.PHONY: all clean test valgrind benchmark help

## all : compile l'exécutable principal
all: $(TARGET)

$(TARGET): $(OBJS) $(MAIN_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "[OK] Exécutable : $@"

# Compilation des objets sources
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/main.o: main.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compilation des objets de test
$(OBJ_DIR)/test_%.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Création des répertoires si absents
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# =============================================================================
# Cible test
# =============================================================================

## test : compile et lance la suite de tests unitaires
test: $(TEST_TARGET)
	@echo "[TEST] Lancement des tests unitaires..."
	./$(TEST_TARGET)

$(TEST_TARGET): $(OBJS) $(TEST_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "[OK] Exécutable de tests : $@"

# =============================================================================
# Cible valgrind
# =============================================================================

## valgrind : vérifie les fuites mémoire sur les tests
valgrind: $(TEST_TARGET)
	@echo "[VALGRIND] Vérification mémoire..."
	valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
	         ./$(TEST_TARGET)

# =============================================================================
# Cible benchmark
# =============================================================================

## benchmark : résout toutes les grilles du répertoire tables/
benchmark: $(TARGET)
	@echo "[BENCHMARK] Résolution de toutes les grilles..."
	./$(TARGET) -benchmark

# =============================================================================
# Cible clean
# =============================================================================

## clean : supprime les fichiers générés
clean:
	@echo "[CLEAN] Suppression des fichiers générés..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "[CLEAN] Terminé."

# =============================================================================
# Aide
# =============================================================================

## help : affiche cette aide
help:
	@echo ""
	@echo "Usage : make [cible]"
	@echo ""
	@grep -E '^## ' $(MAKEFILE_LIST) | sed 's/## /  /'
	@echo ""

CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Werror -pedantic -O2 -g
SRC     = entity/grid.c entity/sudoku_types.c repository/io.c
TEST_SRC = test/test_phase1.c

all: sudoku

sudoku: main.c $(SRC) console/cli.c
	$(CC) $(CFLAGS) -o $@ $^

test: $(TEST_SRC) $(SRC)
	$(CC) $(CFLAGS) -Iservice -Ientity -Irepository \
	      -o test/test_phase1 $(TEST_SRC) $(SRC)
	./test/test_phase1

valgrind-test: test
	valgrind --leak-check=full --show-leak-kinds=all \
	         --error-exitcode=1 ./test/test_phase1

clean:
	rm -f sudoku test/test_phase1 /tmp/test_save.txt