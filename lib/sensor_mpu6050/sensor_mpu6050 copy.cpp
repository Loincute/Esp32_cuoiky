#include "sensor_mpu6050.h"
#include <Adafruit_Sensor.h>
#include <math.h>

void readMPU6050(Adafruit_MPU6050 &mpu, float &ax, float &ay, float &az, float &gx, float &gy, float &gz) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    ax = a.acceleration.x / 9.81;
    ay = a.acceleration.y / 9.81;
    az = a.acceleration.z / 9.81;
    gx = g.gyro.x * 180.0 / M_PI;
    gy = g.gyro.y * 180.0 / M_PI;
    gz = g.gyro.z * 180.0 / M_PI;
}
