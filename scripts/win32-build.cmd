@echo off

if "%1" == "" goto recall_me

goto base

REM #######################################################################################

:recall_me

cmd /e /v /c %0 dummy

goto terminate

REM #######################################################################################

:base

REM #######################################################################################

if not exist "win32-init_env_command.cmd" (
	echo You have to configure your `win32-init_env_command` file.
	echo `win32-init_env_command.cmd-skeleton` is a good file to start with... (I'll copy this file for you^)
	xcopy /q /s win32-init_env_command.cmd-skeleton win32-init_env_command.cmd
)

call "win32-init_env_command.cmd"

REM #######################################################################################

set saved_directory=%CD%
set target_dist=..\dist

mkdir ..\local-tmp 2> NULL
for /F %%s in (%OpenViBE_build_order%) do (
	set OpenViBE_project_name_full=%%s
	set OpenViBE_project_name_rel=!OpenViBE_project_name_full:%OpenViBE_base%\=!

	mkdir ..\local-tmp\!OpenViBE_project_name_rel! 2> NULL
	cd ..\local-tmp\!OpenViBE_project_name_rel!

	cmake -DCMAKE_INSTALL_PREFIX="%%s" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=" /DWIN32 /D_WINDOWS /W3 /Zm1000 /EHsc /GR /wd4355" -Wno-dev -DCMAKE_MODULE_PATH="%saved_directory:\=/%/../cmake-modules;${CMAKE_MODULE_PATH}" !OpenViBE_project_name_full! -G"NMake Makefiles"
	IF NOT "!ERRORLEVEL!" == "0" goto terminate_error

	nmake
	IF NOT "!ERRORLEVEL!" == "0" goto terminate_error

	REM nmake OpenViBE-documentation
	IF NOT "!ERRORLEVEL!" == "0" goto terminate_error

	nmake install
	IF NOT "!ERRORLEVEL!" == "0" goto terminate_error

	cd %saved_directory%
)

echo.
echo Building process terminated successfully !
echo.

REM #######################################################################################

echo.
echo Copying files to 'dist' folder, this can take a few seconds...
echo.

rmdir /s /q %target_dist%         > NULL 2<&1

mkdir %target_dist%               > NULL 2<&1
mkdir %target_dist%\bin           > NULL 2<&1
mkdir %target_dist%\include       > NULL 2<&1
mkdir %target_dist%\lib           > NULL 2<&1
mkdir %target_dist%\share         > NULL 2<&1
mkdir %target_dist%\etc           > NULL 2<&1
mkdir %target_dist%\doc           > NULL 2<&1
mkdir %target_dist%\log           > NULL 2<&1
mkdir %target_dist%\tmp           > NULL 2<&1

echo @echo off                                               >  %target_dist%\test-acquisition-server.cmd
echo pushd ..\scripts                                        >> %target_dist%\test-acquisition-server.cmd
echo call win32-init_env_command.cmd                         >> %target_dist%\test-acquisition-server.cmd
echo popd                                                    >> %target_dist%\test-acquisition-server.cmd
echo cd bin                                                  >> %target_dist%\test-acquisition-server.cmd
echo OpenViBE-acquisition-server-dynamic.exe                 >> %target_dist%\test-acquisition-server.cmd

echo @echo off                                               >  %target_dist%\test-designer.cmd
echo pushd ..\scripts                                        >> %target_dist%\test-designer.cmd
echo call win32-init_env_command.cmd                         >> %target_dist%\test-designer.cmd
echo popd                                                    >> %target_dist%\test-designer.cmd
echo cd bin                                                  >> %target_dist%\test-designer.cmd
echo OpenViBE-designer-dynamic.exe                           >> %target_dist%\test-designer.cmd

echo @echo off                                               >  %target_dist%\test-id-generator.cmd
echo pushd ..\scripts                                        >> %target_dist%\test-id-generator.cmd
echo call win32-init_env_command.cmd                         >> %target_dist%\test-id-generator.cmd
echo popd                                                    >> %target_dist%\test-id-generator.cmd
echo cd bin                                                  >> %target_dist%\test-id-generator.cmd
echo OpenViBE-id-generator-dynamic.exe                       >> %target_dist%\test-id-generator.cmd
echo pause                                                   >> %target_dist%\test-id-generator.cmd

echo @echo off                                               >  %target_dist%\test-plugin-inspector.cmd
echo pushd ..\scripts                                        >> %target_dist%\test-plugin-inspector.cmd
echo call win32-init_env_command.cmd                         >> %target_dist%\test-plugin-inspector.cmd
echo popd                                                    >> %target_dist%\test-plugin-inspector.cmd
echo cd bin                                                  >> %target_dist%\test-plugin-inspector.cmd
echo OpenViBE-plugin-inspector-dynamic.exe                   >> %target_dist%\test-plugin-inspector.cmd
echo pause                                                   >> %target_dist%\test-plugin-inspector.cmd

for /F %%s in (%OpenViBE_build_order%) do (
	xcopy /q /s %%s\bin\*.*            %target_dist%\bin     > NULL 2<&1
	xcopy /q /s %%s\include\*.*        %target_dist%\include > NULL 2<&1
	xcopy /q /s %%s\lib\*.*            %target_dist%\lib     > NULL 2<&1
	xcopy /q /s %%s\share\*.*          %target_dist%\share   > NULL 2<&1
	xcopy /q /s %%s\etc\*.*            %target_dist%\etc     > NULL 2<&1
	xcopy /q /s %%s\doc\*.*            %target_dist%\doc     > NULL 2<&1
)

echo.
echo Copy successfully completed !
echo.

goto terminate_success

REM #######################################################################################

:terminate_error

echo.
echo An error occured during building process !
echo.
pause

goto terminate

REM #######################################################################################

:terminate_success

pause

goto terminate

REM #######################################################################################

:terminate

del NULL
