:: Initial author: yegorf1
@echo off
 
set Portable.RemoteTasks.Manager.Password=
set Portable.RemoteTasks.Manager.Login=
 
setlocal
set asm=..\asm_listing.als
set bin_name=out.ptptb
set arh=..\yafn.arch
 
IF NOT "%1"=="" (
    set asm=%1
)
 
IF NOT "%2"=="" (
    set bin_name=%2
)
 
IF NOT "%3"=="" (
    set arh=%3
)
 
 
 
set guid=
FOR /F %%I IN ('.\Portable.RemoteTasks.Manager.exe -id -v -w -s AssembleDebug definitionFile %arh% archName yafn asmListing %asm% sourcesDir .') DO  set guid=%%I
 
Portable.RemoteTasks.Manager.exe -g %guid%
Portable.RemoteTasks.Manager.exe -g %guid% -r out.ptptb -o %bin_name%
 
 
copy /b/v/y %bin_name% ..\..\..\Dev\yafn\yafn\content
copy /b/v/y %asm% ..\..\..\Dev\yafn\yafn\content
copy /b/v/y %arh% ..\..\..\Dev\yafn\yafn\content
 
endlocal
