APPS =	transquare

CFLAGS = -Wall -g
LDFLAGS = -g

libdir = `pkg-config --variable=libdir cairo`
CFLAGS += `pkg-config gtk+-2.0 --cflags`
LDLIBS += $(libdir)/libcairo.a \
	`pkg-config xext xrender pixman-1 gtk+-2.0 pangocairo --libs` \
	-lm -lpthread

all: $(APPS)

%: %.c

clean:
	$(RM) *.o $(APPS)
