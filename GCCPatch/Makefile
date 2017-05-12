PRJ=Emu48gcc

CC=gcc
LD=gcc

# This is to make GCC not bitch at things M$ allows

CFLAGS= -x c -O3 -Wall -Wno-missing-braces -Wno-format -Wno-uninitialized \
	-Wno-strict-aliasing -Wno-unused-function -Wno-unknown-pragmas

DEFINES= -DIDC_STATIC=-1 \
         -DREGISTRY

LDFLAGS= -s -mwindows

TARGET=$(PRJ).exe

#
# Warning.. my own resources because WINDRES doesn't understand
# the FONT definition for dialogs produced by the M$ tools
#
RSRC=$(PRJ).rc
RSRCOBJ=$(PRJ).o

OBJS=cursor.o ddeserv.o debugger.o disasm.o dismem.o display.o disrpl.o \
	emu48.o engine.o external.o fetch.o files.o i28f160.o keyboard.o \
	keymacro.o kml.o lowbat.o mru.o mops.o opcodes.o redeye.o rpl.o \
	serial.o settings.o sndenum.o sound.o stack.o symbfile.o timer.o \
	udp.o \
	$(RSRCOBJ)

LIBS=-lwinmm -lcomctl32 -lws2_32 -luuid

all:	$(TARGET)

rebuild:	tidy $(TARGET)

tidy:	cleanemacs cleanobj cleanexe

cleanemacs:
	-rm -f *~

cleanobj:
	-rm -f $(OBJS)

cleanexe:
	-rm -f $(TARGET)

cursor.o: cursor.c pch.h emu48.h types.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o cursor.o cursor.c

ddeserv.o: ddeserv.c pch.h emu48.h types.h io.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o ddeserv.o ddeserv.c

debugger.o: debugger.c pch.h resource.h emu48.h \
	types.h opcodes.h ops.h color.h disrpl.h debugger.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o debugger.o debugger.c

disasm.o: disasm.c pch.h emu48.h types.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o disasm.o disasm.c

dismem.o: dismem.c pch.h emu48.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o dismem.o dismem.c

display.o: display.c pch.h resource.h emu48.h \
	types.h io.h kml.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o display.o display.c

disrpl.o: disrpl.c pch.h Emu48.h types.h disrpl.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o disrpl.o disrpl.c

emu48.o: emu48.c pch.h resource.h emu48.h types.h \
	io.h kml.h debugger.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o emu48.o emu48.c

engine.o: engine.c pch.h emu48.h types.h opcodes.h \
	io.h debugger.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o engine.o engine.c

external.o: external.c pch.h emu48.h types.h ops.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o external.o external.c

fetch.o: fetch.c pch.h opcodes.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o fetch.o fetch.c

files.o: files.c pch.h emu48.h types.h ops.h io.h \
	kml.h i28f160.h debugger.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o files.o files.c

i28f160.o: i28f160.c pch.h emu48.h types.h i28f160.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o i28f160.o i28f160.c

keyboard.o: keyboard.c pch.h emu48.h types.h io.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o keyboard.o keyboard.c

keymacro.o: keymacro.c pch.h resource.h Emu48.h types.h kml.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o keymacro.o keymacro.c

kml.o: kml.c pch.h resource.h emu48.h types.h kml.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o kml.o kml.c

lowbat.o: lowbat.c pch.h emu48.h types.h io.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o lowbat.o lowbat.c

mops.o: mops.c pch.h emu48.h types.h ops.h opcodes.h io.h \
	i28f160.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o mops.o mops.c

mru.o: mru.c pch.h resource.h emu48.h types.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o mru.o mru.c

opcodes.o: opcodes.c pch.h emu48.h types.h opcodes.h \
	io.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o opcodes.o opcodes.c

# pch.o:  pch.c pch.h
#	$(CC) $(CFLAGS) $(DEFINES) -c -o pch.o pch.c

redeye.o: redeye.c pch.h emu48.h types.h io.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o redeye.o redeye.c

rpl.o: rpl.c pch.h emu48.h types.h ops.h io.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o rpl.o rpl.c

serial.o: serial.c pch.h emu48.h types.h io.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o serial.o serial.c

settings.o: settings.c pch.h emu48.h types.h i28f160.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o settings.o settings.c

sndenum.o: sndenum.c pch.h emu48.h snddef.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o sndenum.o sndenum.c

sound.o: sound.c pch.h emu48.h types.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o sound.o sound.c

stack.o: stack.c pch.h emu48.h types.h io.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o stack.o stack.c

symbfile.o: symbfile.c pch.h emu48.h types.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o symbfile.o symbfile.c

timer.o: timer.c pch.h emu48.h types.h ops.h io.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o timer.o timer.c

udp.o: udp.c pch.h emu48.h types.h
	$(CC) $(CFLAGS) $(DEFINES) -c -o udp.o udp.c

$(RSRCOBJ): $(RSRC) resource.h emu48.ico dbgtool.bmp checkbox.bmp Emu48.xml
	windres $(DEFINES) -i $(RSRC) -o $(RSRCOBJ)

$(TARGET):	$(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)
