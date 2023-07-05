CXX := g++
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror -Wno-error=unused-function -Wfatal-errors -std=c++20 -O3 -g
LDFLAGS := -L /usr/lib -lstdc++ -lm
BUILD := ./build
OBJ_DIR := $(BUILD)/objects
GEN_DIR := $(BUILD)/generated
APP_DIR := $(BUILD)/apps

BASE_NAME := libghoti.io-util.so
MAJOR_VERSION := 0
MINOR_VERSION := 0.0
SO_NAME := $(BASE_NAME).$(MAJOR_VERSION)
TARGET := $(SO_NAME).$(MINOR_VERSION)

INCLUDE := -I include/
LIBOBJECTS := $(OBJ_DIR)/errorCode.o \
							$(OBJ_DIR)/file.o \
							$(OBJ_DIR)/shared_string_view.o

TESTFLAGS := `pkg-config --libs --cflags gtest`


UTILLIBRARY := -L $(APP_DIR) -lghoti.io-util


all: $(APP_DIR)/$(TARGET) ## Build the shared library

####################################################################
# Dependency Variables
####################################################################
DEP_ERRORCODE = \
	include/util/errorCode.hpp
DEP_ERROROR = \
	include/util/errorOr.hpp
DEP_FILE = \
	$(DEP_ERRORCODE) \
	include/util/file.hpp
DEP_HASPARAMETERS = \
	$(DEP_ERRORCODE) \
	$(DEP_ERROROR) \
	include/util/hasParameters.hpp
DEP_SSV = \
	include/util/shared_string_view.hpp

####################################################################
# Object Files
####################################################################

$(LIBOBJECTS) :
	@echo "\n### Compiling $@ ###"
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@ -fPIC

$(OBJ_DIR)/errorCode.o: \
				src/errorCode.cpp \
				$(DEP_ERRORCODE)

$(OBJ_DIR)/shared_string_view.o: \
				src/shared_string_view.cpp \
				$(DEP_SSV)

$(OBJ_DIR)/file.o: \
				src/file.cpp \
				$(DEP_FILE)

####################################################################
# Shared Library
####################################################################

$(APP_DIR)/$(TARGET): \
				$(LIBOBJECTS)
	@echo "\n### Compiling Ghoti.io Util Shared Library ###"
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(LDFLAGS) -Wl,-soname,$(SO_NAME)
	@ln -f -s $(TARGET) $(APP_DIR)/$(SO_NAME)
	@ln -f -s $(SO_NAME) $(APP_DIR)/$(BASE_NAME)

####################################################################
# Helper data files
####################################################################
$(APP_DIR)/fileExists.txt:
	@mkdir -p $(@D)
	echo "Hello World" > $(APP_DIR)/fileExists.txt

####################################################################
# Unit Tests
####################################################################

OBJDEP_ERRORCODE = \
	$(OBJ_DIR)/errorCode.o

$(APP_DIR)/test-errorOr: \
				test/test-errorOr.cpp \
				$(DEP_ERROROR)
	@echo "\n### Compiling ErrorOr Test ###"
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $< $(LDFLAGS) $(TESTFLAGS)

OBJDEP_HASPARAMETERS = \
	$(OBJDEP_ERRORCODE)

$(APP_DIR)/test-hasParameters: \
				test/test-hasParameters.cpp \
				$(DEP_HASPARAMETERS) \
				$(OBJDEP_HASPARAMETERS)
	@echo "\n### Compiling HasParameters Test ###"
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $< $(LDFLAGS) $(TESTFLAGS) $(OBJDEP_HASPARAMETERS)

OBJDEP_FILE = \
	$(OBJDEP_ERRORCODE) \
	$(OBJ_DIR)/file.o

$(APP_DIR)/test-file: \
				test/test-file.cpp \
				$(APP_DIR)/fileExists.txt \
				$(OBJDEP_FILE)
	@echo "\n### Compiling File Test ###"
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $< $(LDFLAGS) $(TESTFLAGS) $(OBJDEP_FILE)

OBJDEP_SSV = \
	$(OBJ_DIR)/shared_string_view.o

$(APP_DIR)/test-ssv: \
				test/test-ssv.cpp \
				$(OBJDEP_SSV)
	@echo "\n### Compiling Shared String View Test ###"
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $< $(LDFLAGS) $(TESTFLAGS) $(OBJDEP_SSV)

####################################################################
# Commands
####################################################################

.PHONY: all clean cloc docs docs-pdf install test test-watch watch

watch: ## Watch the file directory for changes and compile the target
	@while true; do \
					make all; \
					echo "\033[0;32m"; \
					echo "#########################"; \
					echo "# Waiting for changes.. #"; \
					echo "#########################"; \
					echo "\033[0m"; \
					inotifywait -qr -e modify -e create -e delete -e move src include test Makefile --exclude '/\.'; \
					done

test-watch: ## Watch the file directory for changes and run the unit tests
	@while true; do \
					make test; \
					echo "\033[0;32m"; \
					echo "#########################"; \
					echo "# Waiting for changes.. #"; \
					echo "#########################"; \
					echo "\033[0m"; \
					inotifywait -qr -e modify -e create -e delete -e move src include test Makefile --exclude '/\.'; \
					done

test: ## Make and run the Unit tests
test: \
				$(APP_DIR)/test-errorOr \
				$(APP_DIR)/test-hasParameters \
				$(APP_DIR)/test-file \
				$(APP_DIR)/test-ssv
	@echo "\033[0;32m"
	@echo "############################"
	@echo "### Running normal tests ###"
	@echo "############################"
	@echo "\033[0m"
	env LD_LIBRARY_PATH="$(APP_DIR)" $(APP_DIR)/test-errorOr --gtest_brief=1
	env LD_LIBRARY_PATH="$(APP_DIR)" $(APP_DIR)/test-hasParameters --gtest_brief=1
	env LD_LIBRARY_PATH="$(APP_DIR)" $(APP_DIR)/test-file --gtest_brief=1
	env LD_LIBRARY_PATH="$(APP_DIR)" $(APP_DIR)/test-ssv --gtest_brief=1

clean: ## Remove all contents of the build directories.
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*
	-@rm -rvf $(GEN_DIR)/*

install: ## Install the library globally, requires sudo
	# Install the Shared Library
	@mkdir -p /usr/local/lib/ghoti.io
	@cp $(APP_DIR)/$(TARGET) /usr/local/lib/ghoti.io/
	@ln -f -s $(TARGET) /usr/local/lib/ghoti.io/$(SO_NAME)
	@ln -f -s $(SO_NAME) /usr/local/lib/ghoti.io/$(BASE_NAME)
	@echo "/usr/local/lib/ghoti.io" > /etc/ld.so.conf.d/ghoti.io-util.conf
	# Install the headers
	@mkdir -p /usr/local/include/ghoti.io/util
	@cp include/util/*.hpp /usr/local/include/ghoti.io/util
	# Install the pkgconfig files
	@mkdir -p /usr/local/share/pkgconfig
	@cp pkgconfig/ghoti.io-util.pc /usr/local/share/pkgconfig/
	# Run ldconfig
	@ldconfig >> /dev/null 2>&1
	@echo "Ghoti.io Util installed"

docs: ## Generate the documentation in the ./docs subdirectory
	doxygen

docs-pdf: docs ## Generate the documentation as a pdf, in ./docs/shared_string_view-docs.pdf
	cd ./docs/latex/ && make
	mv -f ./docs/latex/refman.pdf ./docs/util-docs.pdf

cloc: ## Count the lines of code used in the project
	cloc src include test Makefile

help: ## Display this help
	@grep -E '^[ a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "%-15s %s\n", $$1, $$2}'

