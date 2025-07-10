@echo off
setlocal enabledelayedexpansion

set SDK_PATH=..\..\..

for /f "tokens=1-3" %%i in (../project_file/makefile) do (
	if "%%i" equ "SDK_PATH" (
		set SDK_PATH=%%k
	)
)
set SDK_PATH=!SDK_PATH:/=\!
set TOOLS_PATH=!SDK_PATH!\tools

del /f /s /q  user_file\[60000]*.bin


echo make asr.bin
for %%i in (asr\*) do (
	echo %%i | findstr "\[.*\].*" > nul && set valid_file=%%i
	if "!valid_file!" neq "" (
		for /f %%j in ('%SDK_PATH%\tools\build-tools\bin\busybox head -c 8 !valid_file!') do set head_flag=%%j
		if  "!head_flag!" neq "ver-1000" (
			for /f %%a in ('echo prompt $E ^| cmd') do set "ESC=%%a"
			echo !ESC![31m�ļ�%%i��ʽ����,�����Ƿ�ʹ������ȷ�汾��ģ���ļ�!ESC![0m
			set LF=^
			cscript !TOOLS_PATH!\message_box.vbs "�����ļ��ϳ�ʧ��!LF!�ļ�%%i ��ʽ����, !LF!�����Ƿ�ʹ������ȷ�汾��ģ���ļ���" "�ϳɷ���bin�ļ�"
			pause > nul
            goto:eof
		)
	)
)
%TOOLS_PATH%\ci-tool-kit.exe merge asr-file -i asr

@REM echo make user_code.bin
@REM copy %TOOLS_PATH%\..\libs\libfbin_pro.a user_code\[1]code.bin
@REM %TOOLS_PATH%\ci-tool-kit.exe merge user-file -i user_code
@REM del user_code\[1]code.bin


echo make dnn
@REM for %%k in (dnn\*) do (
@REM	echo %%k | findstr "\[.*\].*" > nul && set valid_file2=%%k
@REM	if "!valid_file2!" neq "" (
@REM		for /f %%j in ('%SDK_PATH%\tools\build-tools\bin\busybox head -c 4 !valid_file2!') do set head_flag2=%%j
@REM		if  "!head_flag2!" neq "MOD2" (
@REM			for /f %%a in ('echo prompt $E ^| cmd') do set "ESC=%%a"
@REM			echo !ESC![31m�ļ�%%k��ʽ����,�����Ƿ�ʹ������ȷ�汾��ģ���ļ�!ESC![0m
@REM			echo !ESC![31m!head_flag2!!ESC![0m
@REM			set LF=^
@REM
@REM
@REM			cscript !TOOLS_PATH!\message_box.vbs "�����ļ��ϳ�ʧ��!LF!�ļ�%%k��ʽ����, !LF!�����Ƿ�ʹ������ȷ�汾��ģ���ļ���" "�ϳɷ���bin�ļ�"
@REM			goto:eof
@REM		)
@REM	)
@REM )
%TOOLS_PATH%\ci-tool-kit.exe merge nn-file  -i dnn -a asr

echo make user_file.bin
cd user_file\cmd_info
call cmd_info.bat
cd ..\..
copy user_file\cmd_info\*.bin user_file\
%TOOLS_PATH%\ci-tool-kit.exe merge user-file -i user_file

@REM exit

echo make voice.bin
cd voice
set PATH=..\%TOOLS_PATH%;%PATH%
::exit /b

if exist "adpcm" (
    rd /s /q adpcm >nul
)
if exist "mp3" (
    rd /s /q mp3 >nul
)
if exist "flac" (
    rd /s /q flac >nul
)
if exist "pcm" (
    rd /s /q pcm >nul
)

echo ��ѡ�񲥱�����Ƶ��ʽ:
@REM echo 1. adpcm (Ĭ�ϸ�ʽ,ѹ����4:1,����һ��,������С,����������С)
@REM echo 2. pcm   (��ѹ��,ռ��flash�ռ�ϴ�,���ʺ�,�޽�����,�޽�������)
@REM echo 3. flac  (ѹ���ȴ�Լ3:1,���ʺ�,������С,�����������Դ���adpcm)
@REM echo 4. mp3   (ѹ���ȴ�Լ10:1,���ʽϺ�,�������ϴ�,������������)

set fmt=4
@REM set /p fmt=default=4:

echo %fmt%

if %fmt% == 1 (
	if not exist "adpcm" (
		mkdir "adpcm"
	)
	for /d %%i in (*) do (
		echo %%i | findstr "\[.*\].*" > nul && call:to_adpcm adpcm %%i
	)
	..\%TOOLS_PATH%\group_merge.exe -i adpcm -o ..\voice
) else if %fmt% equ 4 (
	if not exist "mp3" (
		mkdir "mp3"
	)
	@REM for /d %%i in (*) do (
	@REM 	echo %%i | findstr "\[.*\].*" > nul && call:to_mp3 mp3 %%i
	@REM )
	call:to_mp3 mp3 src
	@REM ..\%TOOLS_PATH%\group_merge.exe -i mp3 -o ..\voice
	..\%TOOLS_PATH%\ci-tool-kit ID3-editor --input-dir mp3
	..\%TOOLS_PATH%\ci-tool-kit merge user-file -i mp3 -o mp3
	move mp3\mp3.bin .\voice.bin
) else if %fmt% equ 3 (
	if not exist "flac" (
		mkdir "flac"
	)
	for /d %%i in (*) do (
		echo %%i | findstr "\[.*\].*" > nul && call:to_flac flac %%i
	)
	..\%TOOLS_PATH%\group_merge.exe -i flac -o ..\voice
) else if %fmt% equ 2 (
	if not exist "pcm" (
		mkdir "pcm"
	)
	for /d %%i in (*) do (
		echo %%i | findstr "\[.*\].*" > nul && call:to_pcm pcm %%i
	)
	..\%TOOLS_PATH%\group_merge.exe -i pcm -o ..\voice
) else (
	echo error: ��֧�ֵ���Ƶ��ʽ
)

cd ..

@echo on

exit /b


:to_pcm
echo %1\%2
if not exist %1\%2 (
	mkdir %1\%2
)
for %%j in (%2\*.wav) do (
	echo %%j
	..\%TOOLS_PATH%\wav_to_adpcm -f wav -i "%%j" -o "%1\%%j"
)
goto:eof

:to_adpcm
echo %1\%2
if not exist %1\%2 (
	mkdir %1\%2
)
for %%j in (%2\*.wav) do (
	echo %%j
	..\%TOOLS_PATH%\wav_to_adpcm -i "%%j" -o "%1\%%j"
)
goto:eof


:to_mp3
@REM echo %1\%2
@REM if not exist %1\%2 (
@REM 	mkdir %1\%2
@REM )
for %%j in (%2\*) do (
	set ttt=%%j
	set output_name=%1\!ttt:~4,-4!.mp3
	echo !output_name!
    set num=!ttt:~5,5!
    if !num! == 65535 (
        echo !num!
        set cfg_in=%%j
        set cfg_out=%1\!ttt:~4,-4!.txt
    )
	@REM ..\!TOOLS_PATH!\wav_to_mp3_flac.exe -f mp3 -i "!ttt!" -o "!output_name!"
	@REM ..\!TOOLS_PATH!\lame --silent --cbr -b24 -t --resample 16000 --out-file "!output_name!" "!ttt!"  
	..\!TOOLS_PATH!\lame --silent --cbr -b16 -t --resample 16000 "!ttt!" "!output_name!"
	@REM ..\!TOOLS_PATH!\ci-tool-kit ID3-editor -i !output_name!
)
if exist !cfg_in! ( 
    copy "!cfg_in!" "!cfg_out!"
)
goto:eof


:to_flac
echo %1\%2
if not exist %1\%2 (
	mkdir %1\%2
)
for %%j in (%2\*.wav) do (
	set ttt=%%j
	set output_name=%1\!ttt:~0,-4!.flac
	echo !output_name!
	..\!TOOLS_PATH!\wav_to_mp3_flac.exe -f flac -r 8000 -i "!ttt!" -o "!output_name!"
)
goto:eof

:color
<nul set /p ".=%DEL%">"%~2"
findstr /v /a:%1 /R"^$""%~2" nul
del "%~2"> nul 2>&1
goto:eof

echo on
