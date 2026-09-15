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

if len(sys.argv) < 2:
    print("You must supply one or more examples (by number) to be diffed against their predecessors.")
    exit(0)

comparisons = []

for value in sys.argv[1:]:
    chosen = int(value)

    if chosen not in example_dict:
        print(f"Example {chosen} does not exist.")
        exit(1)

    previous_examples = [number for number in example_dict if number < chosen]
    if not previous_examples:
        print(f"Example {chosen} has no previous example to diff against.")
        exit(1)

    previous = max(previous_examples)
    prev = example_dict[previous]
    curr = example_dict[chosen]
    comparisons.append((prev, curr))

if os_type == "Windows":
    for prev, curr in comparisons:
        subprocess.Popen([differ, prev, curr])
else:
    command = [differ]
    for prev, curr in comparisons:
        command.extend(['--diff', prev, curr])
    subprocess.Popen(command)
