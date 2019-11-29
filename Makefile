LIB_DIRS=
INCLUDES=
COMP_FLAGS=
LINK_FLAGS=

#to-do: get lssl and lcrypto to link statically -> want to avoid LD_LIBRARY_PATH stuff before running the executable
LIBS=
OBJS=main.o linked_list.o klv.o batch.o file_writer.o ref_db.o

# to-do: fix linking of asdcplib so that we can link with gcc
all : ${OBJS} 
		gcc -o klv ${OBJS} ${LIBS} ${LINK_FLAGS} ${LIB_DIRS}

batch.o : batch.c
		gcc -c batch.c ${COMP_FLAGS} ${INCLUDES}

ref_db.o : ref_db.c
		gcc -c ref_db.c ${COMP_FLAGS} ${INCLUDES}

file_writer.o : file_writer.c
		gcc -c file_writer.c ${COMP_FLAGS} ${INCLUDES}

linked_list.o : linked_list.c
		gcc -c linked_list.c ${COMP_FLAGS} ${INCLUDES}

klv.o : klv.c
		gcc -c klv.c ${COMP_FLAGS} ${INCLUDES}

main.o : main.c
		gcc -c main.c ${COMP_FLAGS} ${INCLUDES}

clean : 
		rm klv *.o
