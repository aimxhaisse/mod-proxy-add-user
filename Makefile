APXS=`which apxs2`
RM=`which rm` -f

all:
	$(APXS) -c proxy_add_user.c

clean:
	$(RM) *.la *.lo *.slo

install: all
	$(APXS) -i proxy_add_user.c
