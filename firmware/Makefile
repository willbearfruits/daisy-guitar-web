# Project Name
TARGET = DaisyGuitar

# Sources
CPP_SOURCES = DaisyGuitar.cpp

# Library Locations
# Using environment variable or default to ~/DaisyExamples/
# Override with: make LIBDAISY_DIR=/path/to/libDaisy
LIBDAISY_DIR ?= $(HOME)/DaisyExamples/libDaisy
DAISYSP_DIR ?= $(HOME)/DaisyExamples/DaisySP

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
