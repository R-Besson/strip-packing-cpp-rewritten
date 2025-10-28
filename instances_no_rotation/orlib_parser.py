# simple_parser.py
import os
import sys

def parse_orlib_to_simple_format(input_filename="orlib.txt"):
	"""
	Parses a file in the OR-Lib format for 2D Bin Packing Problems
	and splits it into individual problem files in simple format.

	simple output format for each generated file is:
	filename = W<strip width>_OPT<optimal height>_<i>.txt
	<box1_width> <box1_height>
	<box2_width> <box2_height>
	etc...
	"""

	try:
		with open(input_filename, 'r') as f:
			content = f.read()
	except FileNotFoundError:
		print(f"Error: The input file '{input_filename}' was not found.")
		sys.exit(1)

	try:
		numbers = [int(x) for x in content.split()]
	except ValueError:
		print("Error: The file contains non-integer values and cannot be parsed.")
		sys.exit(1)
		
	if not numbers:
		print("Error: The file is empty.")
		sys.exit(1)

	output_dir = "simple_format_problems"
	os.makedirs(output_dir, exist_ok=True)
	print(f"Output files will be saved in the '{output_dir}' directory.")

	ptr = 0

	try:
		total_problems = numbers[ptr]
		ptr += 1
		
		for i in range(1, total_problems + 1):
			num_containers = numbers[ptr]
			ptr += 1
			container_width = numbers[ptr]
			container_height = numbers[ptr + 1]
			ptr += num_containers * 2
			num_boxes = numbers[ptr]
			ptr += 1
			
			output_filename = os.path.join(output_dir, f"{i}_W{container_width}_OPTH{container_height}.txt")
			with open(output_filename, 'w') as out_f:
				for _ in range(num_boxes):
					box_width = numbers[ptr]
					box_height = numbers[ptr + 1]

					box_repetitions = numbers[ptr + 4]
					for _ in range(box_repetitions):
						out_f.write(f"{box_width} {box_height}\n")
					
					ptr += 5
			
	except IndexError:
		problem_num = locals().get('i', 'unknown')
		print(f"\nError: Ran out of data while parsing problem {problem_num}.")
		print(f"The file '{input_filename}' appears to be malformed or truncated.")
		sys.exit(1)

	if ptr == len(numbers):
		print(f"\nSuccessfully parsed all {total_problems} problems.")
	else:
		remaining = len(numbers) - ptr
		print(f"\nWarning: Parsing finished, but {remaining} numbers were left unused at the end of the file.")

# --- Main execution block ---
if __name__ == "__main__":
	if len(sys.argv) > 1:
		file_to_parse = sys.argv[1]
	else:
		file_to_parse = "list.txt"
		
	if not os.path.exists(file_to_parse):
		print(f"Error: The input file '{file_to_parse}' was not found.")
		print("Usage: python orlib_parser.py [path]")
		sys.exit(1)
		 
	print(f"Attempting to parse '{file_to_parse}'...")
	parse_orlib_to_simple_format(file_to_parse)