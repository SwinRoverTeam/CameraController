#!/bin/bash

DIR="$( cd "$( dirname "$0" )" && pwd )"

mkdir $DIR/logs/
$DIR/cam_ctrl &

return $?
