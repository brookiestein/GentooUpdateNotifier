CC := clang++
CPPFLAGS := -lpthread -lnotify -I. $(shell pkg-config --cflags --libs libnotify)
TARGET = GentooUpdateNotifier
MV = mv -vf
RM = rm -vf
MKDIR = mkdir -vf
SOURCES = main.cpp notifier.cpp
OBJECTS = ${SOURCES:.cpp=.o}
PREFIX = /usr/local/bin

all : ${OBJECTS}
	${CC} ${SOURCES} ${CPPFLAGS} -o ${TARGET}

sync : ${OBJECTS}
	${CC} ${SOURCES} ${CPPFLAGS} -DSYNCHRONIZE -o ${TARGET}

clean :
	${RM} ${OBJECTS}

install : all
	@${MKDIR} ${PREFIX}
	@${MV} ${TARGET} ${PREFIX}

uninstall :
	@${RM} ${PREFIX}/${TARGET}

.PHONY : sync install uninstall
