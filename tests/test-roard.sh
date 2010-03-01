#!/bin/sh

TEST_NAME='roard test'
TEST_DESC='Do tests on RoarAudio'\''s Sound Daemon'

. ./common.sh

c=roard
FORMAT="%-42s : "

printf "$FORMAT" 'Testing for general runnability'
if $BIN_PATH$c --no-listen
then
 _ok
 echo ok
else
 _failed
 echo failed
fi

disp_sum
ret

#ll
