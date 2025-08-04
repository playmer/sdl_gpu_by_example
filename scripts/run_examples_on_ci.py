import os
import pathlib
import zipfile
import zstandard

def decompress_zstandard_to_folder(input_file, destination_dir):
    input_file = pathlib.Path(input_file)
    with open(input_file, 'rb') as compressed:
        decomp = zstandard.ZstdDecompressor()
        output_path = pathlib.Path(destination_dir) / input_file.stem
        with open(output_path, 'wb') as destination:
            decomp.copy_stream(compressed, destination)



with zipfile.ZipFile("examples.zip", 'r') as zip_ref:
    zip_ref.extractall("examples")


directory = os.fsencode("examples")


examples_dir = pathlib.Path("examples")
for file in os.listdir(examples_dir):
    ztd_path = examples_dir.joinpath(file)
    decompress_zstandard_to_folder(ztd_path, examples_dir)
    
