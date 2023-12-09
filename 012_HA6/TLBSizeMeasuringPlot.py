import sys
import matplotlib.pyplot as plt

def read_data(file_name):
    with open(file_name, 'r') as file:
        lines = file.readlines()

    pages = []
    time_us = []

    for line in lines:
        if 'us took with' in line:
            parts = line.split()
            pages.append(int(parts[3]))
            time_us.append(float(parts[-1]))

    return pages, time_us

def plot_data(pages, time_us):
    plt.figure(figsize=(10, 6))
    plt.plot(pages, time_us, marker='o', linestyle='-', color='b')
    plt.xscale('log', base=2)
    plt.title('Time vs Pages')
    plt.xlabel('Number of Pages (log scale)')
    plt.ylabel('Time (us) - log scale')
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py <file_name>")
        sys.exit(1)

    file_name = sys.argv[1]
    pages, time_us = read_data(file_name)
    plot_data(pages, time_us)
