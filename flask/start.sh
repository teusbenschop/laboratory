#!/bin/bash

SCRIPTDIR=`dirname $0`
cd $SCRIPTDIR

PATH=$PATH:~/Library/Python/3.8/bin

gunicorn3 wsgi:app --bind=0.0.0.0:5000 --timeout=20
# This class hangs for a long time, it looks initially --worker-class=gthread
