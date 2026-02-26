import os
import platform
import subprocess
import sys

os_type = platform.system()

if os_type == "Windows":
    differ = 'C:/Program Files/WinMerge/WinMergeU.exe'
else:
    differ = 'meld'

examples = [ f.path for f in os.scandir('source') if f.is_dir() ]

example_dict = {}

for example in examples:
    name = os.path.basename(example)
    number = int(name.split('_')[0])
    example_dict[number] = os.path.join('source', name, name + '.c')

if len(sys.argv) != 2:
    print("You must supply 1 argument, the example (by number) to be diffed against its predecessor.")
    exit(0)

chosen = int(sys.argv[1])
previous = chosen - 1

prev = example_dict[int(sys.argv[1]) - 1]
curr = example_dict[int(sys.argv[1])]

subprocess.Popen([differ, prev, curr], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
