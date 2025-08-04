import os
import pathlib
import shutil
import subprocess
import zipfile


examples_dir = pathlib.Path("examples")
shutil.rmtree(examples_dir, True)

with zipfile.ZipFile("examples.zip", 'r') as zip_ref:
    zip_ref.extractall(examples_dir)

directory = os.fsencode("examples")
files = os.listdir(examples_dir)

for file in files:
    zip_path = examples_dir.joinpath(file)
    print(zip_path)
    example_dir = examples_dir.joinpath(zip_path.stem)
    
    with zipfile.ZipFile(zip_path, 'r') as zip_ref:
        zip_ref.extractall(example_dir)

    subprocess.run(["cmake", "-B", "build"], cwd=example_dir)
    subprocess.run(["cmake", "--build", "build", "--config", "Release"], cwd=example_dir)

    
    
