#!/bin/bash
set -e

SCRIPTDIR=$( cd $(dirname $0); pwd)

(
    cd $SCRIPTDIR/make
    make
)
$SCRIPTDIR/make/arc
