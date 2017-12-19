import sys
import os
import subprocess


def run_shell(string, out=True):
    """ Returns lines of stdout from run script """
    if out:
        return subprocess.run(string, shell=True, stdout=subprocess.PIPE).stdout.decode('cp866')
    else:
        subprocess.run(string, shell=True)


if len(sys.argv) != 1:
    asm_file = sys.argv[1]    
    local_bin_name = asm_file[:asm_file.rfind('.')] + '.bin'

    print("Assembling", asm_file, "to", local_bin_name)

    guid = run_shell("remote-tasks\\rt -id -w -s Assemble definitionFile goose.arch \
                      archName Goose asmListing {}".format(
                asm_file
            )).split('\n')[0][:-1]

    if len(guid) == 36 and guid.count('-') == 4:
        print("Got task guid:", guid)
        print(run_shell("remote-tasks\\rt -g {}".format(guid)))
        run_shell("remote-tasks\\rt -g {} -r out.ptptb -o {}".format(guid, local_bin_name))
        print("Done:", local_bin_name, "written.")
    else:
        print("Could not retrieve task GUID from RemoteTasks!")
        print("Last task:")
        print(run_shell("remote-tasks\\rt -l 0"))

else:
    print("No file to assemble was specified!")
    print("Usage: assemble.bat <filename>")