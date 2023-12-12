import numpy as np
import cv2 as cv

cam_0 = cv.VideoCapture(0)
cam_1 = cv.VideoCapture(1)
cam_2 = cv.VideoCapture(2)
cam_3 = cv.VideoCapture(3)
cam_4 = cv.VideoCapture(4)


while True:
    # Capture frame-by-frame
    ret0, frame0 = cam_0.read()
    ret1, frame1 = cam_1.read()
    ret2, frame2 = cam_2.read()
    ret3, frame3 = cam_3.read()
    ret4, frame4 = cam_4.read()

    if (ret0):
        cv.imshow('Cam-0', frame0)
    
    if (ret1):
        cv.imshow('Cam-1', frame1)

    if (ret2):
        cv.imshow('Cam-2', frame2)

    if (ret3):
        cv.imshow('Cam-3', frame3)

    if (ret4):
        cv.imshow('Cam-4', frame4)

    if cv.waitKey(1) == ord('q'):
        break

cam_0.release()
cam_1.release()
cam_2.release()
cam_3.release()
cam_4.release()
cv.destroyAllWindows()