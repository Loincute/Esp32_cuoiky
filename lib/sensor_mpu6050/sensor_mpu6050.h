#pragma once
#include <Adafruit_MPU6050.h>

void readMPU6050(Adafruit_MPU6050 &mpu, float &ax, float &ay, float &az, float &gx, float &gy, float &gz);
