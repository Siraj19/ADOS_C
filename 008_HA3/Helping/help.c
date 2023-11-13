#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main() {
    FILE *file = fopen("log.txt", "r"); // Open the log file for reading
    if (file == NULL) {
        perror("Error opening the file");
        return 1;
    }

    char line[100]; // Buffer to read each line
    long child_time[14];
    long parent_time[14];
    int count = 0;
    long total_difference = 0;
    long highest_difference = LONG_MIN; // Initialize with the minimum possible value
    long lowest_difference = LONG_MAX;  // Initialize with the maximum possible value

    // Read and parse the log file
    while (fgets(line, sizeof(line), file) != NULL) {
        int index;
        long value;

        if (sscanf(line, "ChildTime[%d] -> %ld", &index, &value) == 2) {
            child_time[index] = value;
        } else if (sscanf(line, "ParentTime[%d] -> %ld", &index, &value) == 2) {
            parent_time[index] = value;
        }
    }

    fclose(file);

    // Calculate differences and statistics
    for (int i = 0; i < 14; i++) {
        long diff = parent_time[i]-child_time[i];
        total_difference += diff;
        printf("Diff is %d \n", diff);
        if (diff > highest_difference) {
            highest_difference = diff;
        }

        if (diff < lowest_difference) {
            lowest_difference = diff;
        }
    }

    // Calculate average difference
    double average_difference = (double)total_difference / 14;

    // Print the results
    printf("Total Difference: %ld\n", total_difference);
    printf("Highest Difference: %ld\n", highest_difference);
    printf("Lowest Difference: %ld\n", lowest_difference);
    printf("Average Difference: %.2lf\n", average_difference);

    return 0;
}
