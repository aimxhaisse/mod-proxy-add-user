#!/bin/sh
aclocal
autoconf
automake -a
./configure --with-apache=/etc/apache2/
make clean all
sudo cp .libs/libproxy_add_user.so /usr/lib/apache2/modules/
sudo apache2ctl stop
sudo apache2ctl stop
sudo apache2ctl start
sudo apache2ctl start
