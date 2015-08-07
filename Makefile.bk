# the name of the compiled binary
TARGET  := 3dmotion

# list files to compile and link together
FILES   := 3dmotion SimpleImage stb


#################################################################


# The following Makefile rules should work for Linux or Cygwin

CC		 := g++
LD		 := g++
OBJSUFFIX	 := .o
LIBPREFIX	 := lib
STATIC_LIBSUFFIX := .a
CFLAGS 		 := -g -Wno-deprecated-declarations
CFLAGS_PLATFORM  := 
LDFLAGS		 := 
FRAMEWORKS	 := OpenGL GLUT
LIBS		 :=

ARCH=$(shell uname | sed -e 's/-.*//g')

ifeq ($(ARCH), CYGWIN_NT)

EXESUFFIX  := .exe

else

ifeq ($(ARCH),Darwin)

# we're building on the mac
#
# Mac users need to point to the libpng directories
EXTRA_LIB_DIRS := /opt/local/lib
EXTRA_INC_DIRS  := /opt/local/include

# 
# The default location of a fink install is given below.
# 
#EXTRA_LIB_DIRS := /sw/lib /opt/local/lib
#EXTRA_INC_DIRS  := /sw/include /opt/local/include

else

# building on Linux

#
# hack for myth machines.  Add /usr/lib as an explicit lib dir so
# it gets picked up instead of /usr/pubsw/lib.
# 

EXTRA_LIB_DIRS := /usr/lib

endif
endif


TARGET     := $(addsuffix $(EXESUFFIX), $(TARGET))

INCDIRS    := .  $(EXTRA_INC_DIRS)
LIBDIRS    := $(EXTRA_LIB_DIRS)

CFLAGS     += $(addprefix -I, $(INCDIRS))
CFLAGS	   += $(CFLAGS_PLATFORM)
LDFLAGS    += $(addprefix -L, $(LIBDIRS))

LDLIBS	   := $(addprefix -l, $(LIBS))
LDFRAMEWORKS := $(addprefix -framework , $(FRAMEWORKS))

OBJS       :=  $(addsuffix $(OBJSUFFIX), $(FILES))


.SUFFIXES : .cpp $(OBJSUFFIX)

.PHONY : clean release all

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LDLIBS) $(LDFRAMEWORKS)

%.o : %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf *$(OBJSUFFIX) $(TARGET) *~ .#* #*

release:
	@make --no-print-directory RELEASE=1


