
@echo off
setlocal enabledelayedexpansion

set SDK_PATH=..\..\..

for /f "tokens=1-3" %%i in (../../../project_file/makefile) do (
	if "%%i" equ "SDK_PATH" (
		set SDK_PATH=%%k
	)
)
set SDK_PATH=!SDK_PATH:/=\!
set TOOLS_PATH=!SDK_PATH!\..\..\tools


if exist *.bin (del *.bin)
!TOOLS_PATH!\ci-tool-kit.exe "cmd-info" "-V2"

if exist *.bin goto label_ok
echo cmd_info failed
cscript !TOOLS_PATH!\message_box.vbs "命令词信息表有错误！" "合成分区bin文件"
goto end

:label_ok
echo cmd_info ok

:end
set TOOLS_PATH=!SDK_PATH!\tools

