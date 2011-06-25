#!/bin/bash
aclocal && libtoolize --force && autoheader && autoconf && automake --add-missing

