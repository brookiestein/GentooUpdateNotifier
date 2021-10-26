CC := clang++
CPPFLAGS := -lpthread -lnotify -I. $(shell pkg-config --cflags --libs libnotify)
TARGET = GentooUpdateNotifier
MV = mv -vf
RM = rm -vf
MKDIR = mkdir -pv
SOURCES = main.cpp notifier.cpp
OBJECTS = ${SOURCES:.cpp=.o}
PREFIX = /usr/local/bin

all : ${OBJECTS}
	${CC} ${SOURCES} ${CPPFLAGS} -o ${TARGET}

clean :
	${RM} ${OBJECTS}

install : all
	@${MKDIR} ${PREFIX}
	@${MV} ${TARGET} ${PREFIX}

uninstall :
	@${RM} ${PREFIX}/${TARGET}

.PHONY : clean install uninstall
