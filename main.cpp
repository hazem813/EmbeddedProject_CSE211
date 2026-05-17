#include "mbed.h"

// --- Hardware Pin Definitions
#define TRIG_PIN D8
#define ECHO_PIN D11

// Motor A (Left) - ENA is hardwired HIGH
#define IN1_PIN D3  // PWM forward speed
#define IN2_PIN D4  // Digital pin for reverse

// Motor B (Right) - ENB is hardwired HIGH
#define IN3_PIN D7  // PWM forward speed
#define IN4_PIN D6  // Digital pin for reverse

// --- Mbed Objects ---
DigitalOut trigger(TRIG_PIN);
InterruptIn echo(ECHO_PIN);
Timer echo_timer;

// Motor A (Left)
PwmOut motorA_in1(IN1_PIN);
DigitalOut motorA_in2(IN2_PIN);

// Motor B (Right)
PwmOut motorB_in3(IN3_PIN);
DigitalOut motorB_in4(IN4_PIN);

// --- Global Variables ---
volatile float distance_cm = 0;
const float TARGET_DISTANCE = 15.0; // Maintain 15 cm distance

// --- Interrupt Service Routines ---
void start_timer() {
    echo_timer.reset();
    echo_timer.start();
}

void stop_timer() {
    echo_timer.stop();
    // Get the time in microseconds
    auto duration = echo_timer.elapsed_time().count(); 
    // Calculate distance (Time * Speed of Sound / 2)
    distance_cm = duration / 58.0; 
}

// --- Helper Functions ---
void set_motors(float speed) {
    
    if (speed > 1.0f) speed = 1.0f;
    if (speed < 0.0f) speed = 0.0f;

    if (speed > 0.0f) {
        // Move Forward: IN1/IN3 get PWM pulses, IN2/IN4 are 0
        motorA_in2 = 0;
        motorB_in4 = 0;
        motorA_in1.write(speed);
        motorB_in3.write(speed);
    } else {
        // Stop Safely: All IN pins to 0 acts as a fast motor brake
        motorA_in2 = 0;
        motorB_in4 = 0;
        motorA_in1.write(0.0f);
        motorB_in3.write(0.0f);
    }
}

int main() {
    // 1. Initialization

    // Attach the ISRs to the rising and falling edges of the echo pin
    echo.rise(&start_timer);
    echo.fall(&stop_timer);

    // Set PWM frequency for motors
    motorA_in1.period(0.00005f); 
    motorB_in3.period(0.00005f);

    // Ensure motors are stopped at startup
    set_motors(0.0f);

    // 2. Main Loop
    while (true) {
        // Trigger the HC-SR04
        trigger = 1;
        wait_us(10);
        trigger = 0;

        // Give the sensor time to listen for the echo
        ThisThread::sleep_for(50ms);

        // 3. Proportional Control Logic
        float error = distance_cm - TARGET_DISTANCE;
        float final_speed = 0.0f; //variable to hold the speed before sending it

        if (distance_cm == 0 || distance_cm > 100) {
            final_speed = 0.0f;
        }
        else if (error > 2.0) {
            final_speed = 0.4;
            
            // friction threshold
            if (final_speed < 0.3f) final_speed = 0.5f; 
            
            // Cap it at 1.0f
            if (final_speed > 1.0f) final_speed = 1.0f; 
        }
        // If it's too close (error < -2.0) or in the sweet spot, speed remains 0.0f

        // Command the motors
        set_motors(final_speed);

        // --- THE NEW PRINT STATEMENT ---
        // Convert the 0.0-1.0 float into a 0-100 integer percentage
        int pwm_percent = (int)(final_speed * 100);
        
        // Print both on the same line for easy reading!
        printf("Dist: %d cm | Motor PWM: %d%%\r\n", (int)distance_cm, pwm_percent);
    }
}