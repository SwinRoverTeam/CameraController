#!/bin/bash

DIR="$( cd "$( dirname "$0" )" && pwd )"
$DIR/camctrl & disown

return $?
