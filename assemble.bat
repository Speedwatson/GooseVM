@echo off
setlocal

IF NOT "%1"=="" (
	set asm_file=%1
	set local_bin_name=%1.bin

	set guid=
	FOR /F %%I IN ('..\RemoteTasks\rt -id -v -w -s Assemble definitionFile goose.arch archName Goose asmListing %asm_file%') DO set guid=%%I
	 
	IF NOT "%guid%"=="" (
		..\RemoteTasks\rt -g %guid%
		..\RemoteTasks\rt -g %guid% -r out.ptptb -o %local_bin_name%
	) ELSE (
		echo Could not retrieve task GUID from RemoteTasks!
		echo Last task:
		..\RemoteTasks\rt -l 0
	)
) ELSE (
	echo No file to assemble was specified!
	echo Usage: assemble.bat ^<filename^>
)