cd /d %~dp0 


set msbuild="C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSbuild.exe"

for %%f in (*.sln) do %msbuild% "%%f" /t:Clean



for /f "delims=" %%f in ('dir *.suo /ah/b/s') do del /f /q /ah "%%f"

for /f "delims=" %%f in ('dir *.suo /a-h/b/s') do del /f /q /a-h "%%f"



for /d /r %%d in (*debug) do (

if exist "%%d"  rd /s /q "%%d"

)


for /d /r %%d in (*release) do (

if exist "%%d" rd /s /q "%%d"

)


if exist "TestResults"  rd /s /q "TestResults"


if exist "FC_Configer.sdf"  del /s /q "FC_Configer.sdf"

cd /d FC_Configer
for %%f in (串口收发数据*) do del /f /q /a-h "%%f"

if %ERRORLEVEL% neq 0 goto pause

if [%1]==[1] goto end



:pause

echo %ERRORLEVEL%

pause

:end




