import evdev
from evdev import ecodes

# import chassis


device = evdev.InputDevice("/dev/input/event13")
print(device)


# print(evdev.ecodes.ecodes)


for event in device.read_loop():
    if event.type == evdev.ecodes.EV_KEY:
        if event.code == ecodes.ecodes["BTN_TR2"] and event.value == 1:
            print("TR2 DOWN")
            # chassis.forward(50)
        if event.code == ecodes.ecodes["BTN_TR2"] and event.value == 0:
            print("TR2 UP")
            # chassis.motorStop()
