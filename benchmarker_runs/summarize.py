import os
import csv
import re

def process_files_in_directory(directory_path='.'):
    filename_pattern = re.compile(r'N(\d+)_IT\d+_WH([\d.]+)\.csv')
    worst_value_pattern = re.compile(r'worst=([^,]+)')
    results = []
    print(f"Scanning directory: {os.path.abspath(directory_path)}")
    for filename in os.listdir(directory_path):
        match = filename_pattern.match(filename)
        
        if match:
            try:
                n_value = int(match.group(1))
                hw_value = float(match.group(2))
                
                file_path = os.path.join(directory_path, filename)
                
                with open(file_path, 'r') as f:
                    lines = f.readlines()
                
                if len(lines) >= 2:
                    summary_line = lines[-1].strip()
                    
                    worst_match = worst_value_pattern.search(summary_line)
                    if worst_match:
                        alpha_value = float(worst_match.group(1))
                        results.append([n_value, hw_value, alpha_value])
                        print(f"  - Processed {filename}: N={n_value}, H/W={hw_value}, α={alpha_value}")
                    else:
                        print(f"  - WARNING: Could not find 'worst=' value in {filename}")
                else:
                    print(f"  - WARNING: File {filename} has fewer than two lines.")

            except (ValueError, IndexError) as e:
                print(f"  - ERROR: Could not process file {filename}. Reason: {e}")

    results.sort(key=lambda x: (x[0], x[1]))
    
    return results

def write_summary_csv(results, output_filename='summary_results.csv'):
    if not results:
        return
        
    try:
        with open(output_filename, 'w', newline='', encoding='utf-8') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(['N', 'H/W', 'α (H/OPT_H)'])
            writer.writerows(results)
        print(f"\nSuccessfully created summary file: {output_filename}")
    except IOError as e:
        print(f"\nERROR: Could not write to file {output_filename}. Reason: {e}")


if __name__ == "__main__":
    all_data = process_files_in_directory()
    write_summary_csv(all_data)