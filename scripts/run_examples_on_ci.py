import os
import pathlib
import shutil
import subprocess
import zipfile


examples_dir = pathlib.Path("examples")
shutil.rmtree(examples_dir, True)
examples_dir.mkdir(parents=True, exist_ok=True)

files = os.listdir('.')

for file in files:
    filename, file_extension = os.path.splitext(file)

    if file_extension != ".zip":
        continue

    print(file)
    example_dir = examples_dir.joinpath(filename)
    
    print(file)
    with zipfile.ZipFile(file, 'r') as zip_ref:
        zip_ref.extractall(example_dir)

    ret = subprocess.run(["cmake", "-B", "build"], cwd=example_dir).returncode
    if ret != 0:
        print(f"Running cmake for {filename} failed!")
        exit(1)

    ret = subprocess.run(["cmake", "--build", "build", "--config", "Release"], cwd=example_dir).returncode
    if ret != 0:
        print(f"Running build for {filename} failed!")
        exit(1)


print("Success building all examples.")
    
    
