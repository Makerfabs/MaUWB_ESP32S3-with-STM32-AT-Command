import pygame
import serial
import serial.tools.list_ports
import json
import csv
import os
import time
import math


# import site  
# print(site.getsitepackages())


RED = [255, 0, 0]
BLACK = [0, 0, 0]
WHITE = [255, 255, 255]


class UWB:
    def __init__(self, name, type):
        self.name = name
        self.type = type
        self.x = 0
        self.y = 0
        self.status = False
        self.list = []

        if self.type == 1:
            self.color = RED
        else:
            self.color = BLACK

    def set_location(self, x, y):
        self.x = x
        self.y = y
        self.status = True

    def cal(self):
        count = 0
        anc_id_list = []
        for range in self.list:
            if range != 0:
                anc_id_list.append(count)
                count = count + 1

        # print(anc_id_list)

        if count >= 3:
            x = 0.0
            y = 0.0

            temp_x, temp_y = self.three_point_uwb(
                anc_id_list[0], anc_id_list[1])

            x += temp_x
            y += temp_y

            temp_x, temp_y = self.three_point_uwb(
                anc_id_list[0], anc_id_list[2])

            x += temp_x
            y += temp_y

            temp_x, temp_y = self.three_point_uwb(
                anc_id_list[2], anc_id_list[1])

            x += temp_x
            y += temp_y

            x = int(x / 3)
            y = int(y / 3)

            self.set_location(x, y)
            self.status = True

    def three_point_uwb(self, a_id, b_id):
        x, y = self.three_point(anc[a_id].x, anc[a_id].y, anc[b_id].x,
                                anc[b_id].y, self.list[a_id], self.list[b_id])

        return x, y

    def three_point(self, x1, y1, x2, y2, r1, r2):

        temp_x = 0.0
        temp_y = 0.0
        # 圆心距离
        p2p = (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)
        p2p = math.sqrt(p2p)

        # 判断是否相交
        if r1 + r2 <= p2p:
            temp_x = x1 + (x2 - x1) * r1 / (r1 + r2)
            temp_y = y1 + (y2 - y1) * r1 / (r1 + r2)
        else:
            dr = p2p / 2 + (r1 * r1 - r2 * r2) / (2 * p2p)
            temp_x = x1 + (x2 - x1) * dr / p2p
            temp_y = y1 + (y2 - y1) * dr / p2p

        return temp_x, temp_y


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


def draw_uwb(uwb):

    # pixel_x = int(uwb.x * cm2p + x_offset)
    # pixel_y = int(uwb.y * cm2p + y_offset)

    # x轴镜像
    pixel_x = int(uwb.x * cm2p + x_offset)
    pixel_y = SCREEN_Y - int(uwb.y * cm2p + y_offset)

    if uwb.status:
        r = 10

        temp_str = uwb.name + " (" + str(uwb.x) + "," + str(uwb.y)+")"

        font = pygame.font.SysFont("Consola", 24)
        surf = font.render(temp_str, True, uwb.color)
        screen.blit(surf, [pixel_x, pixel_y])

        pygame.draw.circle(screen, uwb.color, [
            pixel_x + 20, pixel_y + 50], r, 0)

# {'id': 1, 'range': [0, 53, 423, 0, 0, 0, 0, 0]}


def read_data():
    # ser.reset_input_buffer()
    line = ser.readline().decode('UTF-8').replace('\n', '')
    # print(line)

    try:
        data = json.loads(line)
        print(data)
        print(data['id'])
        # print(data['range'])

        tag[data['id']].list = data['range']
        tag[data['id']].cal()

    except ValueError:
        print("[LOG]" + line)


def fresh_page():
    runtime = time.time()
    screen.fill(WHITE)
    for uwb in anc:
        draw_uwb(uwb)
    for uwb in tag:
        draw_uwb(uwb)

    pygame.draw.line(screen, BLACK, (CENTER_X_PIEXL, 0),
                     (CENTER_X_PIEXL, SCREEN_Y), 1)
    pygame.draw.line(screen, BLACK, (0, CENTER_Y_PIEXL),
                     (SCREEN_X, CENTER_Y_PIEXL), 1)

    pygame.display.flip()

    print("Fresh Over, Use Time:")
    print(time.time() - runtime)


def distance(x1, y1, x2, y2):
    return math.sqrt((x2-x1) ** 2 + (y2 - y1)**2)

# Main Function .............................................................


SCREEN_X = 800
SCREEN_Y = 800

pygame.init()
screen = pygame.display.set_mode([SCREEN_X, SCREEN_Y])
ser = serial.Serial(get_frist_com(), 115200)

anc = []
tag = []
anc_count = 4
tag_count = 8

# A0X, A0Y = 2066, 514
# A1X, A1Y = 2243, 1418
# A2X, A2Y = 427, 1726
# A3X, A3Y = 458, 112

A0X, A0Y = 0, 0
A1X, A1Y = 1000, 0
A2X, A2Y = 1000, 1000
A3X, A3Y = 0, 1000

CENTER_X = int((A0X+A1X+A2X)/3)
CENTER_Y = int((A0Y+A1Y+A2Y)/3)

r0 = distance(A0X, A0Y, CENTER_X, CENTER_Y)
r1 = distance(A1X, A1Y, CENTER_X, CENTER_Y)
r2 = distance(A2X, A2Y, CENTER_X, CENTER_Y)
r3 = distance(A3X, A3Y, CENTER_X, CENTER_Y)

r = max(r0, r1, r2, r3)

cm2p = SCREEN_X / 2 * 0.9 / r

# meter to pixel
# 1000 cm =  1000 piexl
# cm2p = 1
# 1000 cm =  500 piexl
# cm2p = 0.5

x_offset = SCREEN_X / 2 - CENTER_X * cm2p
y_offset = SCREEN_Y / 2 - CENTER_Y * cm2p

CENTER_X_PIEXL = CENTER_X * cm2p + x_offset
CENTER_Y_PIEXL = CENTER_Y * cm2p + y_offset

for i in range(anc_count):
    name = "ANC " + str(i)
    anc.append(UWB(name, 0))
for i in range(tag_count):
    name = "TAG " + str(i)
    tag.append(UWB(name, 1))
anc[0].set_location(A0X, A0Y)
anc[1].set_location(A1X, A1Y)
anc[2].set_location(A2X, A2Y)
anc[3].set_location(A3X, A3Y)

fresh_page()
ser.write("begin".encode('UTF-8'))
ser.reset_input_buffer()

runtime = time.time()

while True:
    read_data()

    if (time.time() - runtime) > 0.5:
        fresh_page()
        runtime = time.time()
        ser.reset_input_buffer()



# Package Command
# pyinstaller --onefile .\position.py