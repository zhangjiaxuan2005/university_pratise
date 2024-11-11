@echo off

REM this is a comment
REM echo %cd%

set ToolsPath=%cd%\tools\

set path=%ToolsPath%thirdparty\python38;%ToolsPath%thirdparty\python38\Scripts;%ToolsPath%thirdparty\Git\bin;%ToolsPath%hcc_riscv32_win\bin;%systemroot%\System32

REM %ToolsPath%thirdparty\python38\python.exe %ToolsPath%env_set.py
cd tools
python.exe env_set.py

cls
cd ..\src
cmd /K PROMPT=$E[1;32m[Hi3861] $P$G$E[1;37m