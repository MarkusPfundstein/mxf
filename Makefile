mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))

TARGET_NAME=mxfstar
LIB_DIRS=
INCLUDES=-I.
CC_FLAGS=
CPP_FLAGS=-Wall
LINK_FLAGS=
TEST_DIR=src/tests

COMPILER=gcc
LIBS=
OBJS=\
	utils/linked_list.o \
	klv/klv.o \
	klv/batch.o \
	types/header_partition.o \
	types/interchange_object.o \
	types/preface.o \
	types/generic_package.o \
	types/material_package.o \
	file_writer.o \
	ref_db.o \
	main.o

all : ${OBJS} 
	${COMPILER} -o ${TARGET_NAME} ${OBJS} ${LIBS} ${LINK_FLAGS} ${LIB_DIRS}

tests : 
	make -C src/tests/ColorMath

%.o : %.c
	${COMPILER} -c ${CC_FLAGS} ${CPP_FLAGS} ${INCLUDES} $< -o $@

clean : 
	rm ${TARGET_NAME} ${OBJS}