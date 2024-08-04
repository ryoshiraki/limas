import numpy
from numpy.random import *
import time
from scipy.spatial import Voronoi
import os
import cv2


print(cv2.getBuildInformation())

im = cv2.imread("data/lena.jpg")
im = cv2.cvtColor(im, cv2.COLOR_BGR2RGB)

print(type(im))
# <class 'numpy.ndarray'>

print(im.shape)
# (225, 400, 3)

print(im.dtype)
# uint8

video = cv2.VideoCapture("data/needit.mp4")
print(type(video))
print(video.isOpened())
print(video.get(cv2.CAP_PROP_FRAME_WIDTH))
print(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
print(video.get(cv2.CAP_PROP_FPS))
print(video.get(cv2.CAP_PROP_FRAME_COUNT))


def hello():
    print("hello")


def echo(a):
    return a


def test_glm():
    vec = [1.0, 2.0, 3.0, 4.0]
    return vec


def test_tuple():
    tup = [1, 2.0, "3"]
    return tup


def test_dict():
    dic = {"apple": 1, "orange": 2, "banana": 3}
    return dic


def test_vector2d():
    list2 = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
    return list2


def test_vector3d():
    list3 = [
        [[1, 2], [2, 3], [3, 4]],
        [[4, 5], [5, 6], [6, 7]],
        [[7, 8], [8, 9], [9, 0]],
    ]
    return list3


def test_np_rand():
    return rand(100)


def test_np_rand_2d():
    return rand(10, 4)


def test_sleep(t):
    time.sleep(t)
    print("wake up")
    return 0


def voronoi(points):
    vor = Voronoi(points)
    return vor.vertices


def ridge_vertices(points):
    vor = Voronoi(points)
    return vor.ridge_vertices


def add(a, b):
    return a + b * 2


def test_cv2():
    return im


def test_cv2_video():
    ret, frame = video.read()
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    if ret:
        return frame


def test_cv2_video_frame(t):
    i = video.get(cv2.CAP_PROP_FRAME_COUNT) * t
    video.set(cv2.CAP_PROP_POS_FRAMES, i)
    ret, frame = video.read()
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    if ret:
        return frame


def cv_input_test(arr):
    arr = cv2.cvtColor(arr, cv2.COLOR_RGB2BGR)
    arr = numpy.transpose(arr, (1, 0, 2))
    return arr


hello()
