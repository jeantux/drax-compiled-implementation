APP=drax
CC=gcc

LIBS= 

ASM_LINKS=

FILES= ./src/dflags.c \
       ./src/dio.c \
       ./src/dlex.c \
       ./src/dlowcode.c \
       ./src/dparser.c \
       ./src/dcpu.c \
       ./src/dcompiler.c \
       ./src/dgen.c \
       ./src/arch/gcc.c \
       ./src/arch/i386.c \
       ./src/arch/backend.c \
       ./src/drax.c

DEBUGF= \
		-ggdb \
		-g

OUTDIR=./bin/

OUTBIN= -o $(OUTDIR)$(APP)

WARNING= \
		-Wextra \
		-Wundef \
		-Wshadow \
		-Wfatal-errors \
		-Wsign-compare \
		-Wwrite-strings \
		-Wredundant-decls \
		-Wdouble-promotion \
		-Wmissing-declarations \
		-Wdisabled-optimization \

FLAGS= \
		-std=c99 \
		-Wall  \
		$(WARNING) \
		$(DWN_CCFLAGS) \
		$(LIBS) \
		$(OUTBIN)

# DRAX_BACKEND will define which backend will be used
# __DRAX_BACKEND_ASM = ASM, native code
# __DRAX_BACKEND_GCC = GCC (Unix)

ifdef BACKEND
	DRAX_BACKEND=-D__DRAX_BACKEND_$(BACKEND)
else
	DRAX_BACKEND=-D__DRAX_BACKEND_GCC
endif

DEFAULT_BUILD = \
		$(CC) \
		$(FLAGS) \
		$(FILES) \
		$(ASM_LINKS) \
		-D_GNU_SOURCE \
		$(DRAX_BACKEND) \

all: corelibs
	$(DEFAULT_BUILD)

debug:
	$(DEFAULT_BUILD) $(DEBUGF)

inspect:
	$(DEFAULT_BUILD) $(DEBUGF) -D__DRAX_INSPECT

run:
	./bin/$(APP)

config:
	mkdir -p bin

corelibs:
	as --32 src/lib/dlibio.s -o bin/libdrax.so

clean:
	rm -f ./bin/$(APP)
	rm -f ./bin/*.so
