#!/bin/bash

SCRIPTDIR=$( cd $(dirname $0); pwd)

(
    cd $SCRIPTDIR/make
    make
)