import csv
import numpy as np

accel_data = []
gyro_data = []

with open('data.csv') as f:
    reader = csv.reader(f)
    for row in reader:
        if row[0] == 'accel':
            data = [float(x) for x in row[1:]]
            s = sum(data)
            accel_data.append([i / s for i in data])  # need to normalise
        elif row[0] == 'gyro':
            gyro_data.append([float(x) for x in row[1:]])

accel_data = np.array(accel_data)
gyro_data = np.array(gyro_data)

accel_covariance = np.cov(accel_data[1000:].transpose())
print("Accelerometer:")
print(accel_covariance)
gyro_covariance = np.cov(gyro_data[1000:].transpose())
print("Gyro: ")
print(gyro_covariance)

def format_c_matrix(matrix, size, name):
    print(f"double {name}[3][3] = {{")
    for i in range(size):
        print("    {", end="")
        for j in range(size):
            print(f"{matrix[i,j]:.8e}", end="")
            if j < size - 1:
                print(", ", end="")
        print("}", end="")
        if i < 2:
            print(",")
        else:
            print("")
    print("};")
    print()

format_c_matrix(accel_covariance, 3,"accel_cov")
format_c_matrix(gyro_covariance, 4,"gyro_cov")