@echo off

set ToolsPath=%~dp0

set path=%ToolsPath%thirdparty\python38;%ToolsPath%thirdparty\python38\Scripts;%ToolsPath%thirdparty\Git\bin;%ToolsPath%hcc_riscv32_win\bin;%systemroot%\System32

%ToolsPath%thirdparty\python38\python.exe %ToolsPath%env_set.py

cls

cmd /K PROMPT=$E[1;32m[DevTools] $P$G$E[1;37m