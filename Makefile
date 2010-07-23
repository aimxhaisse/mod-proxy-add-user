APXS=`which apxs2`
RM=`which rm` -f
MKDIR=`which mkdir`

all:
	$(APXS) -c proxy_add_user.c

clean:
	$(RM) *.la *.lo *.slo
	$(RM) -r .libs

install: all
	$(MKDIR) -p $(DESTDIR)
	$(APXS)  -S LIBEXECDIR=$(DESTDIR)/ -ci proxy_add_user.c
