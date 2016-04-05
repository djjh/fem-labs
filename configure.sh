#!/bin/bash

SCRIPTDIR=$( cd $(dirname $0); pwd)

mkdir -p $SCRIPTDIR/make
(
    cd $SCRIPTDIR/make
    rm -rf $SCRIPTDIR/make/*
    cmake $SCRIPTDIR
)