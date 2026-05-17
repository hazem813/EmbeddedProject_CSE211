System Logic
Sensing: The HC-SR04 measures the distance to the obstacle.

Control Loop: The system calculates the error: Error = Current Distance - Target Distance (15 cm).

Actuation:

If Error > 2cm: The object is moving away. Motor speed is calculated as Speed = Error * Kp.

If Error < -2cm: The object is too close. Motors stop safely.

Dead-band compensation: A minimum PWM threshold (e.g., 30%) is applied to overcome physical motor friction.

Troubleshooting & Deployment
If you encounter the "PyOCD deploy failed, deploying using mass storage device" or an "IDCODE error" during deployment:

Update ST-Link Firmware: Download STSW-LINK007 from the STMicroelectronics website and upgrade the board's debugger firmware to fix the 32KB virtual drive limit.

Free up the Serial Monitor: Ensure you disconnect the Mbed Studio Serial Monitor before attempting to build and flash new code to prevent communication locks.

Author
Team 1 Section 2 Robotics
Faculty of Engineering, Ain Shams University
"""