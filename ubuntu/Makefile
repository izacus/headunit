INCLUDES=`pkg-config --cflags gstreamer-0.10 gstreamer-app-0.10 gstreamer-video-0.10 gstreamer-interfaces-0.10 libcrypto libusb-1.0 openssl`-I$(TOP)/jni -I$(TOP)/pb
CFLAGS= -g -DDEBUG
LFLAGS= -g `pkg-config --libs gstreamer-0.10 gstreamer-app-0.10 gstreamer-video-0.10 gstreamer-interfaces-0.10 libcrypto libusb-1.0 openssl` -lSDL

TOP = ..
SRCS =	$(TOP)/jni/hu_aad.c	\
	$(TOP)/jni/hu_aap.c	\
	$(TOP)/jni/hu_ssl.c	\
	$(TOP)/jni/hu_usb.c	\
	$(TOP)/jni/hu_uti.c	\
	$(TOP)/jni/c_utils.c \
	$(TOP)/pb/pb_common.c \
	$(TOP)/pb/pb_decode.c \
	$(TOP)/pb/pb_encode.c \
	$(TOP)/pb/gen/sensors.pb.c \
	main.c
OBJS = $(SRCS:.c=.o)
APP = headunit

.PHONY: depend clean

all: $(APP)

$(APP): $(OBJS)
	$(CC) -o $(APP) $(OBJS) $(LFLAGS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	rm -f $(OBJS) *~ $(APP)

depend: $(SRCS)
	makedepend $^

release: CFLAGS += -g0 -O3 -ffunction-sections -fdata-sections -fvisibility=hidden
release: LFLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections
release: $(APP)

# DO NOT DELETE
