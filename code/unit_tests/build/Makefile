MAKEFILE      = Makefile

####### Compiler, tools and options

CC            = g++
CXX           = g++
DEFINES       = -DUSER_PROGRAM -DFILESYS_NEEDED -DNETWORK -DLINUX -DHOST_i386 -DFILESYS -DUNIT_TESTS -Dprivate=public
CFLAGS        = -pipe -g -Wall -W -fPIC $(DEFINES)
CXXFLAGS      = -pipe -fpermissive -g -std=gnu++11 -Wall -W -fPIC $(DEFINES)
INCPATH       = -I../synchconsole_test -I. -I../../bin -I../../machine -I../../filesys -I../../network -I../../threads -I../../userprog -I../mocks -I../common 
DEL_FILE      = rm -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
COPY          = cp -f
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
INSTALL_FILE  = install -m 644 -p
INSTALL_PROGRAM = install -m 755 -p
INSTALL_DIR   = cp -f -R
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
TAR           = tar -cf
COMPRESS      = gzip -9f
LINK          = g++
LFLAGS        = 
LIBS          = $(SUBLIBS)  
AR            = ar cqs
RANLIB        = 
SED           = sed
STRIP         = strip

####### Output directory

OBJECTS_DIR   = ../bin

####### Files

SOURCES       = ../synchconsole_test/synchconsole_test.cc \
		../../machine/console.cc \
		../../machine/disk.cc \
		../../machine/interrupt.cc \
		../../machine/machine.cc \
		../../machine/mipssim.cc \
		../../machine/network.cc \
		../../machine/stats.cc \
		../../machine/sysdep.cc \
		../../machine/timer.cc \
		../../machine/translate.cc \
		../../filesys/directory.cc \
		../../filesys/filehdr.cc \
		../../filesys/filesys.cc \
		../../filesys/fstest.cc \
		../../filesys/openfile.cc \
		../../filesys/synchdisk.cc \
		../../network/nettest.cc \
		../../network/post.cc \
		../../threads/list.cc \
		../../threads/scheduler.cc \
		../../threads/synch.cc \
		../../threads/synchlist.cc \
		../../threads/system.cc \
		../../threads/thread.cc \
		../../threads/threadtest.cc \
		../../threads/utility.cc \
		../../threads/switch.S \
		../../userprog/addrspace.cc \
		../../userprog/bitmap.cc \
		../../userprog/exception.cc \
		../../userprog/progtest.cc \
		../../userprog/synchconsole.cc 
OBJECTS       = \
		synchconsole_test.o \
		console.o \
		disk.o \
		interrupt.o \
		machine.o \
		mipssim.o \
		network.o \
		stats.o \
		sysdep.o \
		timer.o \
		translate.o \
		directory.o \
		filehdr.o \
		filesys.o \
		fstest.o \
		openfile.o \
		synchdisk.o \
		nettest.o \
		post.o \
		list.o \
		scheduler.o \
		synch.o \
		synchlist.o \
		system.o \
		thread.o \
		threadtest.o \
		utility.o \
		switch.o \
		addrspace.o \
		bitmap.o \
		exception.o \
		progtest.o

TARGET        = synchconsole_test


####### Build rules

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: 
all: Makefile $(TARGET)

clean:  
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) *~ core *.core





####### Compile
synchconsole_test.o: ../synchconsole_test/synchconsole_test.cc ../common/test_utils.h \
		../mocks/console_mock.h \
		../../threads/utility.h \
		../../threads/copyright.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../userprog/synchconsole.cc \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/list.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o synchconsole_test.o ../synchconsole_test/synchconsole_test.cc

console.o: ../../machine/console.cc ../../threads/copyright.h \
		../../machine/console.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/system.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o console.o ../../machine/console.cc

disk.o: ../../machine/disk.cc ../../threads/copyright.h \
		../../machine/disk.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/system.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o disk.o ../../machine/disk.cc

interrupt.o: ../../machine/interrupt.cc ../../threads/copyright.h \
		../../machine/interrupt.h \
		../../threads/list.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/system.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/scheduler.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o interrupt.o ../../machine/interrupt.cc

machine.o: ../../machine/machine.cc ../../threads/copyright.h \
		../../machine/machine.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../threads/system.h \
		../../threads/thread.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o machine.o ../../machine/machine.cc

mipssim.o: ../../machine/mipssim.cc ../../threads/copyright.h \
		../../machine/machine.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../machine/mipssim.h \
		../../threads/system.h \
		../../threads/thread.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o mipssim.o ../../machine/mipssim.cc

network.o: ../../machine/network.cc ../../threads/copyright.h \
		../../threads/system.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o network.o ../../machine/network.cc

stats.o: ../../machine/stats.cc ../../threads/copyright.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../machine/stats.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o stats.o ../../machine/stats.cc

sysdep.o: ../../machine/sysdep.cc ../../threads/copyright.h \
		../../machine/interrupt.h \
		../../threads/list.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/system.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/scheduler.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o sysdep.o ../../machine/sysdep.cc

timer.o: ../../machine/timer.cc ../../threads/copyright.h \
		../../machine/timer.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/system.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o timer.o ../../machine/timer.cc

translate.o: ../../machine/translate.cc ../../threads/copyright.h \
		../../machine/machine.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/system.h \
		../../threads/thread.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o translate.o ../../machine/translate.cc

directory.o: ../../filesys/directory.cc ../../threads/copyright.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../filesys/filehdr.h \
		../../machine/disk.h \
		../../userprog/bitmap.h \
		../../filesys/openfile.h \
		../../filesys/directory.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o directory.o ../../filesys/directory.cc

filehdr.o: ../../filesys/filehdr.cc ../../threads/copyright.h \
		../../threads/system.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h \
		../../filesys/filehdr.h \
		../../userprog/bitmap.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o filehdr.o ../../filesys/filehdr.cc

filesys.o: ../../filesys/filesys.cc ../../threads/copyright.h \
		../../machine/disk.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../userprog/bitmap.h \
		../../filesys/openfile.h \
		../../filesys/directory.h \
		../../filesys/filehdr.h \
		../../filesys/filesys.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o filesys.o ../../filesys/filesys.cc

fstest.o: ../../filesys/fstest.cc ../../threads/copyright.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/system.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o fstest.o ../../filesys/fstest.cc

openfile.o: ../../filesys/openfile.cc ../../threads/copyright.h \
		../../filesys/filehdr.h \
		../../machine/disk.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../userprog/bitmap.h \
		../../filesys/openfile.h \
		../../threads/system.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o openfile.o ../../filesys/openfile.cc

synchdisk.o: ../../filesys/synchdisk.cc ../../threads/copyright.h \
		../../filesys/synchdisk.h \
		../../machine/disk.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/synch.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/list.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o synchdisk.o ../../filesys/synchdisk.cc

nettest.o: ../../network/nettest.cc ../../threads/copyright.h \
		../../threads/system.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o nettest.o ../../network/nettest.cc

post.o: ../../network/post.cc ../../threads/copyright.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/synchlist.h \
		../../threads/list.h \
		../../threads/synch.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o post.o ../../network/post.cc

list.o: ../../threads/list.cc ../../threads/copyright.h \
		../../threads/list.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o list.o ../../threads/list.cc

scheduler.o: ../../threads/scheduler.cc ../../threads/copyright.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/system.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o scheduler.o ../../threads/scheduler.cc

synch.o: ../../threads/synch.cc ../../threads/copyright.h \
		../../threads/synch.h \
		../../threads/thread.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/list.h \
		../../threads/system.h \
		../../threads/scheduler.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o synch.o ../../threads/synch.cc

synchlist.o: ../../threads/synchlist.cc ../../threads/copyright.h \
		../../threads/synchlist.h \
		../../threads/list.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/synch.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o synchlist.o ../../threads/synchlist.cc

system.o: ../../threads/system.cc ../../threads/copyright.h \
		../../threads/system.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o system.o ../../threads/system.cc

thread.o: ../../threads/thread.cc ../../threads/copyright.h \
		../../threads/thread.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/switch.h \
		../../threads/synch.h \
		../../threads/list.h \
		../../threads/system.h \
		../../threads/scheduler.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o thread.o ../../threads/thread.cc

threadtest.o: ../../threads/threadtest.cc ../../threads/copyright.h \
		../../threads/system.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o threadtest.o ../../threads/threadtest.cc

utility.o: ../../threads/utility.cc ../../threads/copyright.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o utility.o ../../threads/utility.cc

switch.o: ../../threads/switch.S ../../threads/copyright.h \
		../../threads/switch.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o switch.o ../../threads/switch.S

addrspace.o: ../../userprog/addrspace.cc ../../threads/copyright.h \
		../../threads/system.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h \
		../../bin/noff.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o addrspace.o ../../userprog/addrspace.cc

bitmap.o: ../../userprog/bitmap.cc ../../threads/copyright.h \
		../../userprog/bitmap.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../filesys/openfile.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o bitmap.o ../../userprog/bitmap.cc

exception.o: ../../userprog/exception.cc ../../threads/copyright.h \
		../../threads/system.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h \
		../../userprog/syscall.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o exception.o ../../userprog/exception.cc

progtest.o: ../../userprog/progtest.cc ../../threads/copyright.h \
		../../threads/system.h \
		../../threads/utility.h \
		../../threads/bool.h \
		../../machine/sysdep.h \
		../../threads/thread.h \
		../../machine/machine.h \
		../../machine/translate.h \
		../../machine/disk.h \
		../../userprog/addrspace.h \
		../../filesys/filesys.h \
		../../filesys/openfile.h \
		../../threads/scheduler.h \
		../../threads/list.h \
		../../machine/interrupt.h \
		../../machine/stats.h \
		../../machine/timer.h \
		../../userprog/synchconsole.h \
		../../machine/console.h \
		../../threads/synch.h \
		../../filesys/synchdisk.h \
		../../network/post.h \
		../../machine/network.h \
		../../threads/synchlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o progtest.o ../../userprog/progtest.cc
