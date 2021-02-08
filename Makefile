#==============================================================================
#  MAKEFILE VARIABLES
#==============================================================================

# Variables for the make help printout
PROJECT_NAME := SystemSolver
VERSION := v0.0.1

# Binary names (not path, only file name)
PRODUCTION_BINARY_NAME := SystemSolver
TEST_SUITE_BINARY_NAME := TestSystemSolver

# Entry file names (not path, only file name)
PRODUCTION_ENTRY_NAME := main.c
TEST_SUITE_ENTRY_NAME := start_test.c

# Directory definitions
INC_DIR := include
SRC_DIR := src
TST_DIR := test
OBJ_DIR := build
BIN_DIR := bin

# Testing configuration
TEST_CFLAGS  := -Wall -g -I$(INC_DIR) -DTEST
TEST_LDLIBS  := -lcheck -lm -lpthread -lrt -lsubunit -lOpenCL

# Production source preparation
PRODUCTION_ENTRY_FILE   := $(SRC_DIR)/$(PRODUCTION_ENTRY_NAME)
HEADER_FILES            := $(wildcard $(INC_DIR)/*.h)
PRODUCTION_FILES        := $(filter-out $(PRODUCTION_ENTRY_FILE),$(wildcard $(SRC_DIR)/*.c))
PRODUCTION_ENTRY_OBJECT := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(PRODUCTION_ENTRY_FILE))
PRODUCTION_OBJECTS      := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(PRODUCTION_FILES))

# Unit testing source preparation
TEST_ENTRY_FILE         := $(TST_DIR)/$(TEST_SUITE_ENTRY_NAME)
TEST_FILES              := $(filter-out $(TEST_ENTRY_FILE),$(wildcard $(TST_DIR)/*.cpp))
TEST_ENTRY_OBJECT       := $(patsubst $(TST_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(TEST_ENTRY_FILE))
TEST_OBJECTS            := $(patsubst $(TST_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(TEST_FILES))

# Compilation related flags and parameters
CC=gcc
CFLAGS=-Wall -g -I$(INC_DIR)
LDLIBS=-lOpenCL

# Makefile formatting variables
SILENCE:=@
ECHO_FLAG:=-n
RED := $(shell tput setaf 1)
GREEN := $(shell tput setaf 2)
YELLOW := $(shell tput setaf 3)
BOLD := $(shell tput bold)
RESET:= $(shell tput sgr0)


#==============================================================================
#  MAKE TARGETS
#==============================================================================

.PHONY: test clean leak_detection files help main

# First make target = help
help:
	@echo "---------------------------------------------------------------------"
	@echo " $(BOLD)$(YELLOW)$(PROJECT_NAME)$(RESET) $(BOLD)$(VERSION)$(RESET) make interface"
	@echo "---------------------------------------------------------------------"
	@echo " $(BOLD)make [help]$(RESET) - Prints out this help message."
	@echo " $(BOLD)make test$(RESET)   - Compiles the whole test suite and runs it."
	@echo " $(BOLD)make main$(RESET)   - Compiles the project."
	@echo " $(BOLD)make files$(RESET)  - Prints out the files registered by make."
	@echo " $(BOLD)make clean$(RESET)  - Cleans up the build directory."


#==============================================================================
#  PRODUCTION TARGETS
#==============================================================================

# Building prodution files
$(PRODUCTION_ENTRY_OBJECT) $(PRODUCTION_OBJECTS): $(PRODUCTION_ENTRY_FILE) $(PRODUCTION_FILES) $(HEADER_FILES)
	@echo $(ECHO_FLAG) "Compiling production code..  "
	$(SILENCE)$(CC) $(CFLAGS) $(LDLIBS) -c $^ \
	2> temp_error_file; if [ $$? -ne 0 ]; then touch _error_flag; fi; true
	$(SILENCE)if [ -f _error_flag ]; then \
	  rm -f _error_flag; \
		echo "$(RED)$(BOLD)Error$(RESET)"; \
		cat temp_error_file; \
		rm -f temp_error_file; \
		echo "$(RED)$(BOLD)Make aborted!$(RESET)"; \
		false; \
	else \
		if [ -s temp_error_file ]; then \
			echo "$(YELLOW)$(BOLD)Warning$(RESET)"; \
			cat temp_error_file; \
		else \
			echo "$(GREEN)$(BOLD)Done$(RESET)"; \
		fi \
	fi
	$(SILENCE)rm -f temp_error_file
	$(SILENCE)rm -f $(INC_DIR)/*.gch
	$(SILENCE)mv *.o $(OBJ_DIR)

# Building the whole application
main: $(OBJ_DIR) $(BIN_DIR) $(PRODUCTION_ENTRY_OBJECT)
	$(SILENCE)$(CC) -o $(BIN_DIR)/$(PRODUCTION_BINARY_NAME) $(PRODUCTION_ENTRY_OBJECT) $(PRODUCTION_OBJECTS) $(CFLAGS) $(LDLIBS)


#==============================================================================
#  TESTING TARGETS
#==============================================================================

# Main target for unit testing
test: $(OBJ_DIR) $(BIN_DIR) $(PRODUCTION_ENTRY_OBJECT)
	$(SILENCE)$(CC) $(TEST_ENTRY_FILE) $(PRODUCTION_OBJECTS) -o $(BIN_DIR)/$(TEST_SUITE_BINARY_NAME) $(TEST_CFLAGS) $(TEST_LDLIBS)
	@echo "Testing...  $(YELLOW)$(BOLD)Start$(RESET)"
	$(SILENCE)./$(BIN_DIR)/$(TEST_SUITE_BINARY_NAME) \
	2> temp_error_file; if [ $$? -ne 0 ]; then touch _error_flag; fi; true
	$(SILENCE)if [ -f _error_flag ]; then \
	  rm -f _error_flag; \
		echo "Testing...  $(RED)$(BOLD)Error$(RESET)"; \
		cat temp_error_file; \
		rm -f temp_error_file; \
		false; \
	else \
		if [ -s temp_error_file ]; then \
			echo "Testing...  $(YELLOW)$(BOLD)Warning$(RESET)"; \
			cat temp_error_file; \
		else \
			echo "Testing...  $(GREEN)$(BOLD)Success$(RESET)"; \
		fi \
	fi
	$(SILENCE)rm -f temp_error_file


#==============================================================================
#  UTILITY TARGETS
#==============================================================================

# Creating the build directory
$(OBJ_DIR):
	$(SILENCE)mkdir -p $@

# Creating the bin directory
$(BIN_DIR):
	$(SILENCE)mkdir -p $@

# Cleaning up the build directory
clean:
	@echo $(ECHO_FLAG) "Cleaning up.. "
	$(SILENCE)rm -rf $(OBJ_DIR)
	$(SILENCE)rm -rf $(BIN_DIR)
	@echo "$(GREEN)$(BOLD)Done$(RESET)"

# File and target listing
files:
	@echo ' $(YELLOW)$(BOLD)Production files$(RESET)'
	@echo '   $(BOLD)Entry$(RESET):     $(PRODUCTION_ENTRY_FILE)'
	@echo '   $(BOLD)Headers$(RESET):   $(HEADER_FILES)'
	@echo '   $(BOLD)Sources$(RESET):   $(PRODUCTION_FILES)'
	@echo ' $(YELLOW)$(BOLD)Test files$(RESET)'
	@echo '   $(BOLD)Entry$(RESET):     $(TEST_ENTRY_FILE)'
	@echo ' '
	@echo ' $(GREEN)$(BOLD)Production objects$(RESET)'
	@echo '   $(BOLD)Entry$(RESET):     $(PRODUCTION_ENTRY_OBJECT)'
	@echo '   $(BOLD)Objects$(RESET):   $(PRODUCTION_OBJECTS)'
	@echo ' $(GREEN)$(BOLD)Test objects$(RESET)'
	@echo '   $(BOLD)Entry$(RESET):     $(TEST_ENTRY_OBJECT)'

