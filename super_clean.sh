#!/bin/bash

SCRIPTDIR=$( cd $(dirname $0); pwd)

rm -rf $SCRIPTDIR/make
rm -rf $SCRIPTDIR/dep/**/*{.git,.build}
