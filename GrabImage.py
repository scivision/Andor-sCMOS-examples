#!/usr/bin/env python
"""
Simple script to grab an image from Andor Neo/Zyla.
Inefficient yes.

this uses the "imgcam.exe" program that is compiled with Cmake as per the README.

consider pyAndorNeo if this method is too basic for you.
However be aware if you want high frame rate, I couldn't even get 10fps with pyAndorNeo.

Goal is to make a simple C++ program that does a kinetic series for N hours as configured
through the command line.
This is a first step.
"""
from pathlib import Path
from subprocess import check_call
import imageio

root = Path(__file__).parent

EXP_SEC = 1. #exposure
I_MIN = 100 # intensity minimum for scaling to 8 bit
I_MAX = 5000 # intensity maximum for scaling to 8 bit -- will saturate on bright aurora!

FIN = root/'image.bmp'
FOUT = root/'latest.jpg'

def writeJPEG(fin,fout):
    I = imageio.imread(fin)
    imageio.imsave(fout,I)

def writeJPEG2000(fin,fout):
    import glymur
    I = imageio.imread(fin)

    glymur.Jp2k(fout,I,cratios=[50])

cmd = ['imgcam',
       '-e',str(EXP_SEC),
       '-s',str(I_MIN),str(I_MAX),
       '-v']

check_call(cmd,cwd=str(root))

writeJPEG(FIN,FOUT)




