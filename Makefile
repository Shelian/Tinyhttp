WORK_PATH=$(shell pwd)

BIN1=sql_api
BIN2=cgi_select
BIN3=cgi_insert
CC=g++
INCLUDE= -I${WORK_PATH}/mysql_lib/include -I.
LIB_PATH= -L${WORK_PATH}/mysql_lib/lib
SRC1=sql_api.cpp
SRC2=sql_api.h cgi_select.cpp sql_api.cpp
SRC3=sql_api.h cgi_insert.cpp sql_api.cpp

LIB_NAME=-lmysqlclient

.PHONY:all
all:$(BIN1) $(BIN2) $(BIN3)

${BIN1}:${SRC1}
	${CC} -o $@ $^ ${INCLUDE} ${LIB_PATH} ${LIB_NAME}  -D_DEBUG_

${BIN2}:${SRC2}
	${CC} -o $@ $^ ${INCLUDE} ${LIB_PATH} ${LIB_NAME} 

${BIN3}:${SRC3}
	${CC} -o $@ $^ ${INCLUDE} ${LIB_PATH} ${LIB_NAME} 

.PHONY:clean
clean:
	rm -f ${BIN} ${BIN2} ${BIN3}

.PHONY:copy
copy:all
	cp -f $(BIN2) ../htodc
	cp -f $(BIN3) ../htdoc
