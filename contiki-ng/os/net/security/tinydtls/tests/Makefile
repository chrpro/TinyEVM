# Makefile for tinydtls
#
# Copyright (c) 2011, 2012, 2013, 2014, 2015 Olaf Bergmann (TZI) and others.
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# and Eclipse Distribution License v. 1.0 which accompanies this distribution.
#
# The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
# and the Eclipse Distribution License is available at
# http://www.eclipse.org/org/documents/edl-v10.php.
#
# Contributors:
#    Olaf Bergmann  - initial API and implementation
#

DTLS_SUPPORT ?= posix

# files and flags
SOURCES:= dtls-server.c ccm-test.c prf-test.c dtls-client.c
  #cbc_aes128-test.c #dsrv-test.c
PROGRAMS:= $(patsubst %.c, %, $(SOURCES))
LIB:=../libtinydtls.a

OBJECTS := $(patsubst %.c, %.o, $(SOURCES))

CFLAGS  := -I. -I.. -I../$(DTLS_SUPPORT)
LDFLAGS := -L..
LDLIBS  := -ltinydtls

all:	$(LIB) $(PROGRAMS)

$(LIB):
	(cd .. && $(MAKE))

clean:
	(cd .. && $(MAKE) clean)
	@rm -f $(OBJECTS) $(PROGRAMS)
