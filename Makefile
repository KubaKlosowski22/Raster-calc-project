
CC = gcc
CFLAGS = -Wall -I/usr/include/gdal -lgdal

all: gdalRasterCalc

test: test.c
	$(CC) $(CFLAGS) -o gdalRasterCalc gdalRasterCalc.c

clean:
	rm -f gdalRasterCalc


