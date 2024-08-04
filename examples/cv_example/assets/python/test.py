import numpy
from numpy.random import *
import time
from scipy.spatial import Voronoi
import os
import cv2


im = cv2.imread("data/lena.jpg")
im = cv2.cvtColor(im, cv2.COLOR_BGR2RGB)

print(type(im))
# <class 'numpy.ndarray'>

print(im.shape)
# (225, 400, 3)

print(im.dtype)
# uint8


def test():
    return im


def input_test(arr):
    arr = cv2.cvtColor(arr, cv2.COLOR_RGB2BGR)
    arr = numpy.transpose(arr, (1, 0, 2))
    return arr
