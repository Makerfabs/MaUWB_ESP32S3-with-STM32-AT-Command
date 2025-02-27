import serial
import serial.tools.list_ports
import json
import time
import math

tag_list = []


def get_frist_com():
    port_list = serial.tools.list_ports.comports()

    if len(port_list) <= 0:
        print("No COM")
        return ""
    else:
        print("First COM")
        for com in port_list:
            print(com)
            return list(com)[0]


def read_data():
    # ser.reset_input_buffer()
    line = ser.readline().decode('UTF-8').replace('\n', '')
    # print(line)

    try:
        data = json.loads(line)
        print(data)
        # print(data['id'])
        # print(data['range'])

        tag_list[data['id']] = tag_list[data['id']] + 1

    except ValueError:
        print("[LOG]" + line)


def print_tag():
    # print(tag_list)
    print()
    print()
    print("------------------------------------------")
    print("------------------------------------------")
    for i in range(4):
        for j in range(8):
            print(tag_list[i * 4 + j], end=" ")
        print()
    print("------------------------------------------")
    print("------------------------------------------")
    print()
    print()
    pass


ser = serial.Serial(get_frist_com(), 115200)

runtime = time.time()

for i in range(64):
    tag_list.append(0)

while True:
    read_data()

    if (time.time() - runtime) > 3:
        print_tag()
        runtime = time.time()
        ser.reset_input_buffer()
