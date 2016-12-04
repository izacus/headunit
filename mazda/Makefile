#CC=gcc
#LD=ld
INCLUDES=`pkg-config --cflags gstreamer-0.10 gstreamer-app-0.10 gstreamer-video-0.10 dbus-1 libcrypto libusb-1.0 openssl`-I$(TOP)/jni -I$(TOP)/pb -I$(TOP)/mazda/gps
CFLAGS=-fPIC `sdl-config --libs` -O3
LFLAGS=`pkg-config --libs gstreamer-0.10 gstreamer-app-0.10 gstreamer-video-0.10 dbus-1 libcrypto libusb-1.0 openssl `   -lrt
TOP = ..
SRCS =	$(TOP)/jni/hu_aad.c	\
	$(TOP)/jni/hu_aap.c	\
	$(TOP)/jni/hu_ssl.c	\
	$(TOP)/jni/hu_usb.c	\
	$(TOP)/jni/hu_uti.c	\
	$(TOP)/pb/pb_common.c \
	$(TOP)/pb/pb_decode.c \
	$(TOP)/pb/pb_encode.c \
	$(TOP)/pb/gen/sensors.pb.c \
	gps/mzd_gps.c \
	gps/minmea.c \
	main.c
OBJS = $(SRCS:.c=.o)
APP = headunit

.PHONY: depend clean

# Minmea compiles in C99 mode
gps/minmea.o: CFLAGS += -std=c99 -D_POSIX_C_SOURCE=199309L -D_BSD_SOURCE -D_DEFAULT_SOURCE

all: $(APP)

$(APP): $(OBJS)
	$(CC)  -g -o $(APP) $(OBJS) $(LFLAGS)

.c.o:
	$(CC) -g $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	rm -f $(OBJS) *~ $(APP)

depend: $(SRCS)
	makedepend $^

# DO NOT DELETE
