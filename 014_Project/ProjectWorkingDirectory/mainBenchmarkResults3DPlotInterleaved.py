import sys
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

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
    interleaved_benchmarks = [bench for bench in benchmarks if bench['partition_scheme'] == 'Interleaved']

    fig = plt.figure(figsize=(10, 6))
    ax = fig.add_subplot(111, projection='3d')

    threads = [bench['threads'] for bench in interleaved_benchmarks]
    block_sizes = [bench['block_size'] for bench in interleaved_benchmarks]
    compute_times = [bench['compute_time'] for bench in interleaved_benchmarks]

    ax.scatter(threads, block_sizes, compute_times, c='r', marker='o')

    # Adding labels with computation time for each point
    for i, txt in enumerate(compute_times):
        ax.text(threads[i], block_sizes[i], compute_times[i], f'{compute_times[i]:.2f}', fontsize=8)

    ax.set_xlabel('Number of Threads')
    ax.set_ylabel('Block Size')
    ax.set_zlabel('Computation Time (seconds)')
    ax.set_title('3D Plot of Interleaved Scheme')

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
