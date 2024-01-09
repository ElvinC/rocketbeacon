from scipy.io import wavfile
import scipy.io
import numpy as np

prefix = "audio_"

map_name = {
	"0": "zero",
	"1": "one",
	"2": "two",
	"3": "three",
	"4": "four",
	"5": "five",
	"6": "six",
	"7": "seven",
	"8": "eight",
	"9": "nine",
	"point": "point",
	"minus": "minus"
}

with open("../Firmware/Core/Inc/audio_samples" + ".h", "w") as f:

	for name in map_name:
		print(f"Encoding {name}")
		samplerate, data = wavfile.read(name + ".wav")
		print(f"number of channels = {data.shape}")
		length = data.shape[0] / samplerate
		print(f"length = {length}s {samplerate}")


		f.write(f"const uint8_t {prefix+map_name[name]}[] = " + "{")

		for i in range(data.shape[0]):
			f.write(f"{data[i]}")
			if i < (data.shape[0]-1):
				f.write(",")
			if i % 32 == 0:
				f.write("\n")
		
		f.write("};\n\n")
