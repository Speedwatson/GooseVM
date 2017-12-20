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
    root_path = "D:\\Studying\\Информатика и ЯИПП\\GooseVM\\"
    remote_tasks_path = root_path + "remote-tasks\\rt"

    debug = '-d' in sys.argv

    asm_file = sys.argv[1]
    local_bin_name = root_path + 'bin\\' + os.path.basename(asm_file)[:os.path.basename(asm_file).rfind('.')] + ('_debug' if debug else '') + '.bin'

    sources = os.path.dirname(asm_file)
    if not sources:
        sources = os.path.dirname(sys.argv[0])

    print("Assembling", asm_file, "to", local_bin_name)

    if debug:
        guid = run_shell("\"{}\" -id -w -s AssembleDebug definitionFile ..\goose.arch \
                      archName Goose asmListing \"{}\" sourcesDir {}".format(
                remote_tasks_path,
                asm_file,
                sources,
            )).split('\n')[0][:-1]
    else:
        guid = run_shell("\"{}\" -id -w -s Assemble definitionFile ..\goose.arch \
                      archName Goose asmListing \"{}\"".format(
                remote_tasks_path,
                asm_file,
            )).split('\n')[0][:-1]

    if len(guid) == 36 and guid.count('-') == 4:
        print("Got task guid:", guid)
        print(run_shell("\"{}\" -g {}".format(remote_tasks_path, guid)))
        run_shell("\"{}\" -g {} -r out.ptptb -o \"{}\"".format(remote_tasks_path, guid, local_bin_name))
        print("Done:", local_bin_name, "written.")
    else:
        print("Could not retrieve task GUID from RemoteTasks!")
        print("Answer:", guid)
        print("Last task:")
        print(run_shell("\"{}\" -l 0".format(remote_tasks_path)))

else:
    print("No file to assemble was specified!")
    print("Usage: assemble.bat <filename> [-d]")