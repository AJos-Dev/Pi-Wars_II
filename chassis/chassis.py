from time import sleep
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BOARD)  # Set GPIO mode to BCM
GPIO.setwarnings(False)

# PWM Frequency
pwmFreq = 100

# Setup Pins for motor controller - LHS
GPIO.setup(12, GPIO.OUT)  # PWMA
GPIO.setup(18, GPIO.OUT)  # AIN2
GPIO.setup(16, GPIO.OUT)  # AIN1
GPIO.setup(22, GPIO.OUT)  # STBY
GPIO.setup(15, GPIO.OUT)  # BIN1
GPIO.setup(13, GPIO.OUT)  # BIN2
GPIO.setup(11, GPIO.OUT)  # PWMB

# Setup Pins for motor controller - RHS
GPIO.setup(19, GPIO.OUT)  # PWMA
GPIO.setup(23, GPIO.OUT)  # AIN2
GPIO.setup(24, GPIO.OUT)  # AIN1
GPIO.setup(36, GPIO.OUT)  # STBY
GPIO.setup(29, GPIO.OUT)  # BIN1
GPIO.setup(37, GPIO.OUT)  # BIN2
GPIO.setup(21, GPIO.OUT)  # PWMB

pwma_lhs = GPIO.PWM(12, pwmFreq)  # pin 18 to PWM
pwmb_lhs = GPIO.PWM(11, pwmFreq)  # pin 13 to PWM
pwma_lhs.start(100)
pwmb_lhs.start(100)

pwma_rhs = GPIO.PWM(19, pwmFreq)  # pin 23 to PWM
pwmb_rhs = GPIO.PWM(21, pwmFreq)  # pin 27 to PWM
pwma_rhs.start(100)
pwmb_rhs.start(100)

## Functions
###############################################################################


def forward(spd):
    runMotor(0, spd, 0)
    runMotor(1, spd, 0)
    runMotor(2, spd, 0)
    runMotor(3, spd, 0)


def reverse(spd, duration):
    runMotor(0, spd, 1)
    runMotor(1, spd, 1)
    runMotor(2, spd, 1)
    runMotor(3, spd, 1)
    sleep(duration)
    motorStop()


def left(spd, duration):
    runMotor(0, spd, 1)
    runMotor(1, spd, 0)
    runMotor(2, spd, 0)
    runMotor(3, spd, 1)
    sleep(duration)
    motorStop()


def right(spd, duration):
    runMotor(0, spd, 0)
    runMotor(1, spd, 1)
    runMotor(2, spd, 1)
    runMotor(3, spd, 0)
    sleep(duration)
    motorStop()


def forward_left(spd, duration):
    runMotor(1, spd, 0)
    runMotor(2, spd, 0)
    sleep(duration)
    motorStop()


def forward_right(spd, duration):
    runMotor(0, spd, 0)
    runMotor(3, spd, 0)
    sleep(duration)
    motorStop()


def backward_left(spd, duration):
    runMotor(0, spd, 1)
    runMotor(3, spd, 1)
    sleep(duration)
    motorStop()


def backward_right(spd, duration):
    runMotor(1, spd, 1)
    runMotor(2, spd, 0)
    sleep(duration)
    motorStop()


def rotate_left(spd, duration):
    runMotor(0, spd, 1)
    runMotor(1, spd, 0)
    runMotor(2, spd, 1)
    runMotor(3, spd, 0)
    sleep(duration)
    motorStop()


def rotate_right(spd, duration):
    runMotor(0, spd, 0)
    runMotor(1, spd, 1)
    runMotor(2, spd, 0)
    runMotor(3, spd, 1)
    sleep(duration)
    motorStop()


def runMotor(motor, spd, direction):
    GPIO.output(22, GPIO.HIGH)
    GPIO.output(36, GPIO.HIGH)
    in1 = GPIO.HIGH
    in2 = GPIO.LOW

    if direction == 1:
        in1 = GPIO.LOW
        in2 = GPIO.HIGH

    if motor == 0:
        GPIO.output(16, in1)
        GPIO.output(18, in2)
        pwma_lhs.ChangeDutyCycle(spd)
    elif motor == 1:
        GPIO.output(24, in1)
        GPIO.output(23, in2)
        pwma_rhs.ChangeDutyCycle(spd)
    elif motor == 2:
        GPIO.output(15, in1)
        GPIO.output(13, in2)
        pwmb_lhs.ChangeDutyCycle(spd)
    elif motor == 3:
        GPIO.output(29, in1)
        GPIO.output(37, in2)
        pwmb_rhs.ChangeDutyCycle(spd)


def motorStop():
    GPIO.output(22, GPIO.LOW)
    GPIO.output(36, GPIO.LOW)

    GPIO.output(16, GPIO.LOW)
    GPIO.output(18, GPIO.LOW)

    GPIO.output(24, GPIO.LOW)
    GPIO.output(23, GPIO.LOW)

    GPIO.output(15, GPIO.LOW)
    GPIO.output(13, GPIO.LOW)

    GPIO.output(29, GPIO.LOW)
    GPIO.output(37, GPIO.LOW)
