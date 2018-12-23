# Cont_StandardServo


This program was created as an assignment for CSCI-SHU 350 - Embedded Computer Systems at NYU Shanghai. The program uses an STM32F746NG Discovery Board microcontroller to manipulate the rotational speed and direction of a Parallax Continuous Servo Motor and Standard Servo Motor.

PWM is used to manipulate the speed and direction of rotation of a continuous rotation servo motor and change the position of the servo shaft of a standard servo motor, based on touch screen input from the Discovery Board’s LCD.

Implementation
Per the instruction manual of the Parallax Motors, rotational speed and direction are determined by the duration of a
high pulse, in the 1.3–-1.7 ms range for the continuous rotation servo and 0.75-2.25 for the
standard servo. In order for smooth rotation, the servo needs a 20 ms pause between pulses.
Therefore Tmax Continuous = 21.7 ms, and Tmax standard = 22.25. T max is used to calculate
the maximum value ARR as opposed to T min, because otherwise a potential increase in ARR
would cause it to be out of range (65535+).

The clock frequency of the LCD screen is 216 MHz.
I decided to use timers 12 and 13, both of which are connected to APB2 register. According to
the Reference Manual, this register multiplies the clock frequency by 1 if and only if the
prescaler is 1, in any other case it multiplies the clock frequency by 2. I did not set either
prescaler to be 1, so the clock frequency is multiplied by 2. However, the system clock
configuration divides clocks’ frequencies connected to APB2 by 2, so the two operations cancel
and the clock frequency remains 216 MHz.
