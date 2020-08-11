@echo off
set curpath=%~dp0

cd ..
set KBE_ROOT=%cd%
set KBE_RES_PATH=%KBE_ROOT%/kbe/res/;%curpath%/;%curpath%/scripts/;%curpath%/res/
set KBE_BIN_PATH=%KBE_ROOT%/kbe/bin/server/

if defined uid (echo UID = %uid%)

cd %curpath%
call "kill_server.bat"

echo KBE_ROOT = %KBE_ROOT%
echo KBE_RES_PATH = %KBE_RES_PATH%
echo KBE_BIN_PATH = %KBE_BIN_PATH%

start /B %KBE_BIN_PATH%/machine.exe --cid=172166541000 --gus=1
start /B %KBE_BIN_PATH%/logger.exe --cid=172166542000 --gus=2
start /B %KBE_BIN_PATH%/interfaces.exe --cid=172166543000 --gus=3
start /B %KBE_BIN_PATH%/dbmgr.exe --cid=172166544000 --gus=4
start /B %KBE_BIN_PATH%/baseappmgr.exe --cid=172166545000 --gus=5
start /B %KBE_BIN_PATH%/cellappmgr.exe --cid=172166546000 --gus=6
start /B %KBE_BIN_PATH%/baseapp.exe --cid=172166547001 --gus=7
start /B %KBE_BIN_PATH%/cellapp.exe --cid=172166548001 --gus=9
start /b %KBE_BIN_PATH%/loginapp.exe --cid=172166549000 --gus=11
