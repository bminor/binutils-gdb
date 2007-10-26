#!/bin/bash
source_em()
{
  local current_script="$em_script"
  em_script=$1
  . $em_script
  em_script=$current_script
}
fragment()
{
  local lineno=$[${BASH_LINENO[0]} + 1]
  echo >> e${EMULATION_NAME}.c "#line $lineno \"$em_script\""
  cat >> e${EMULATION_NAME}.c
}
