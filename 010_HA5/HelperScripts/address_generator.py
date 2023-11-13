import argparse
import time
import random
import math

def generate_random_number_with_seed(seed_value):
    random.seed(seed_value)  # Set the seed value
    random_number = random.randint(0, 100)  # Generate a random integer between 0 and 100
    return random_number

def generate_valid(likelihood):
    # Get the current system time in microseconds
    current_time_microseconds = int(time.time() * 1e6)
    # Seed is time in microseconds
    randomization = generate_random_number_with_seed(current_time_microseconds)
    # Check if the remainder is less than the input value
    return randomization < likelihood

def generate_valid_array(likelihood, n):
    # We want random combination of valid and invalid with given likelihood
    results = []
    for i in range(n):
        result = generate_valid(likelihood)
        results.append(result)
    return results

#Unused function because it generates physical addresses
def generate_random_address_with_seed(address_space, base, limit, growth, validity, seed_value):
    random.seed(seed_value)  # Set the seed value
    if validity:
        if growth:
            random_number = random.randint(base, base+limit-1)  #segment 0 for positive growth
        else:
            random_number = random.randint(base-limit, base)  #segment 1 for negative growth
    else:
        if growth:
            random_number = random.randint(base+limit, base+address_space-1)  #segment 0 for positive growth
        else:
            random_number = random.randint(base-address_space, base-limit)  #segment 1 for negative growth

    return random_number

def generate_random_virtual_address_with_seed(address_space, base, limit, growth, validity, seed_value):
    random.seed(seed_value)  # Set the seed value
    if validity:
        if growth:
            random_number = random.randint(0, limit-1)  #segment 0 for positive growth
        else:
            random_number = random.randint(address_space-limit, address_space-1)  #segment 1 for negative growth
    else:
        random_number = random.randint(limit, address_space-limit-1)  #Invaid

    return random_number

def generate_random_address_array(validity_array, address_space, base, limit, growth):
    # We want random combination of valid and invalid with given likelihood
    results = []
    for v in validity_array:
        current_time_microseconds = int(time.time() * 1e6)
        result = generate_random_virtual_address_with_seed(address_space, base, limit, growth, v, current_time_microseconds)
        results.append(result)
    return results

def main():
    parser = argparse.ArgumentParser(description='Parse and print arguments for segmentation.py')
    parser.add_argument('-a', type=int, help='Argument for flag -a')
    parser.add_argument('-p', type=int, help='Argument for flag -p')
    parser.add_argument('-P', type=int, help='Argument for flag -P')
    parser.add_argument('-b', type=int, help='Argument for flag -b')
    parser.add_argument('-l', type=int, help='Argument for flag -l')
    parser.add_argument('-B', type=int, help='Argument for flag -B')
    parser.add_argument('-L', type=int, help='Argument for flag -L')
    parser.add_argument('-n', type=int, help='Argument for flag -n')
    parser.add_argument('-g', type=int, help='Argument for flag -g')
    
    args = parser.parse_args()

    valid_array = generate_valid_array(args.P, args.n)
    if(args.g == 0):
        random_address_array = generate_random_address_array(valid_array, args.a, args.b, args.l, True)
    else:
        random_address_array = generate_random_address_array(valid_array, args.a, args.B, args.L, False)
    print(",".join(map(str, random_address_array)))

if __name__ == "__main__":
    main()
