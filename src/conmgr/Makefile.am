# Makefile for conmgr library

AUTOMAKE_OPTIONS = foreign
CLEANFILES = core.*

AM_CPPFLAGS = -I$(top_srcdir)

noinst_PROGRAMS = libconmgr.o
noinst_LTLIBRARIES = libconmgr.la

libconmgr_la_SOURCES = \
	con.c \
	conmgr.c \
	conmgr.h \
	delayed.c \
	io.c \
	mgr.h \
	rpc.c \
	signals.c \
	watch.c \
	work.c \
	workq.c \
	workq.h

libconmgr_la_LDFLAGS = $(LIB_LDFLAGS) -module --export-dynamic

# This was made so we could export all symbols from libconmgr
# on multiple platforms
libconmgr_o_SOURCES =
libconmgr.o$(EXEEXT) :  $(libconmgr_la_OBJECTS) $(libconmgr_la_DEPENDENCIES)
	$(LINK)  $(libconmgr_la_OBJECTS)
