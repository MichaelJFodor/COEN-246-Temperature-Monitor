
ifneq ($V,1)
Q ?= @
endif

DEBUG	= -O3
INCLUDE	= -I/usr/local/include -I/usr/include/mysql
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib -L/usr/lib/arm-linux-gnueabihf
LDLIBS    = -lwiringPi -lwiringPiDev -lpthread -lm -lcrypt -lrt -lz -lmariadbclient -ldl


SRC	=	rpis.c

OBJ	=	$(SRC:.c=.o)

BINS	=	$(SRC:.c=)

rpis:	rpis.o
	$Q echo [link]
	$Q $(CC) -o $@ rpis.o $(LDFLAGS) $(LDLIBS)

.c.o:
	$Q echo [CC] $<
	$Q $(CC) -c $(CFLAGS) $< -o $@

clean:
	$Q echo "[Clean]"
	$Q rm -f $(OBJ) *~ core tags $(BINS)