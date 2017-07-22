CC=g++
MOC=moc
CFLAGS=-Wall -Wextra -pedantic -O2 -fPIC -std=c++11
SOURCES=window.cpp main.cpp
MOC_HEADERS=window.h
EXECUTABLE=evo1
INCDIRS=-I/usr/include/qt5 -I/usr/include/qt5/QtCore -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtWidgets
LIBPATH=-L/usr/lib/x86_64-linux-gnu
LIBS=-lQt5Core -lQt5Gui -lQt5Widgets
# Change postfixes
MOC_SOURCES=$(MOC_HEADERS:.h=.moc.cpp)
OBJECTS=$(SOURCES:.cpp=.o) $(MOC_SOURCES:.cpp=.o)

all: $(EXECUTABLE)
	@echo Done!

$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ $(LIBPATH) $(LIBS) -o $@

# Generate object files, rule to change postfix
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@

# Generate cc from h via Qt's Meta Object Compiler, rule to change postfix
%.moc.cpp: %.h
	$(MOC) $(INCDIRS) $< -o $@

.PHONY: tags clean

clean:
	rm *.o

# Generate ctags file for all included files (autocomplete and jump to source)
tags:
	gcc -M $(INCDIRS) $(SOURCES) | \
	sed -e 's/[\\ ]/\n/g' | \
	sed -e '/^$$/d' -e '/\.o:[ \t]*$$/d' | \
	ctags -L - --c++-kinds=+p --fields=+iaS --extra=+q
