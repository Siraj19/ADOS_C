import sys
import matplotlib.pyplot as plt

def parse_and_extract_info(file_name):
    with open(file_name, 'r') as file:
        benchmarks = []
        current_benchmark = None

        for line in file:
            line = line.strip()

            if line.startswith("Number of threads are:"):
                if current_benchmark is not None:
                    benchmarks.append(current_benchmark)
                current_benchmark = {'threads': 0, 'partition_scheme': '', 'block_size': None, 'compute_time': 0}

                threads = int(line.split()[-1])
                current_benchmark['threads'] = threads
            elif line.startswith("Partioning scheme is:"):
                partition_scheme = line.split()[-1]
                current_benchmark['partition_scheme'] = partition_scheme
                if partition_scheme == "Interleaved":
                    block_size_line = next(file).strip()
                    block_size = int(block_size_line.split()[-1])
                    current_benchmark['block_size'] = block_size
            elif line.startswith("Time taken to compute"):
                compute_time = float(line.split()[-2])
                current_benchmark['compute_time'] = compute_time

        if current_benchmark is not None:
            benchmarks.append(current_benchmark)

        return benchmarks

def plot_results(benchmarks):
    interleaved = [bench['compute_time'] for bench in benchmarks if bench['partition_scheme'] == 'Interleaved']
    sectioned = [bench['compute_time'] for bench in benchmarks if bench['partition_scheme'] == 'Sectioned']
    interleaved_threads = [bench['threads'] for bench in benchmarks if bench['partition_scheme'] == 'Interleaved']
    sectioned_threads = [bench['threads'] for bench in benchmarks if bench['partition_scheme'] == 'Sectioned']
    interleaved_block_sizes = [bench['block_size'] for bench in benchmarks if bench['partition_scheme'] == 'Interleaved']

    plt.figure(figsize=(12, 8))

    plt.scatter(interleaved_threads, interleaved, label='Interleaved', marker='o', c='blue')
    plt.scatter(sectioned_threads, sectioned, label='Sectioned', marker='o', c='orange')
    
    # Highlight values in the interleaved scheme
    for i, txt in enumerate(interleaved):
        plt.annotate(f'{txt:.2f}', (interleaved_threads[i], interleaved[i]), fontsize=5, color='blue')

    # Highlight block sizes in the interleaved scheme
    for i, txt in enumerate(interleaved_block_sizes):
        plt.annotate(f'Block Size: {txt}', (interleaved_threads[i], interleaved[i]), fontsize=8, color='red')

    plt.title('Computation Time vs. Number of Threads')
    plt.xlabel('Number of Threads (log base 2)')
    plt.ylabel('Computation Time (seconds)')
    plt.xscale('log', base=2)  # Set x-axis to log scale
    plt.legend()


    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py input_file.txt")
        sys.exit(1)

    input_file_name = sys.argv[1]
    results = parse_and_extract_info(input_file_name)

    for result in results:
        print(result)

    plot_results(results)
