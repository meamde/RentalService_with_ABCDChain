# 
# Makefile
# Created by Seung Hyun Wang on 2018. 6. 19..
# Copyright © 2018년 Seung Hyun Wang. All rights reserved.
#
#
#
OBJECTS = RaspServer.o cJSON.o 
SOURCES = RaspServer.c

RaspServer.exe :$(OBJECTS)
			gcc -o  RaspServer $(OBJECTS) -D_REENTRANT -lpthread  -lcjson


RaspServer.o : $(SOURCES)
			   gcc -c  $(SOURCES) -D_REENTRANT -lpthread -lcjson

cJSON.o : cJSON.c
			gcc -c  cJSON.c -D_REENTRANT -lpthread -lcjson
			   

# end of Makefile
