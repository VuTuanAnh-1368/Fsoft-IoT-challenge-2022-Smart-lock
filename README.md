# Fsoft-IoT-challenge-2022-Smart-lock
Fsoft IoT challenge 2022 – Smart lock – Team optimization


Optimization team: Vũ Tuấn Anh, Nguyễn Công, Lê Duy Quang, Nguyễn Gia Bảo

This is the source code for a smart lock system using Silicon Labs' BGM220
Bluetooth explorer kit with additional features: body temperature scanning,
monitoring and limiting number of people in the room, automatic light switching.

## Instructions

### Deployment

Import the project into Simplicity studio 5, then, with a BGM220 Bluetooth
explorer kit connected, either choose to debug or build the project then program
into the connected kit. Make sure the kit has all necessary components connected
before running.

### External connections

- Connect room light wires to the two pins to the left of the light relay to the
back, the light relay acts as a switch.
- Plug a MicroUSB cable to the corresponding port to the back to supply power
and deploy the code to the mainboard.

### Operation

1. Set the Bluetooth connection password as `passKey` in `app/app.c`, which
consits of 6 digits.
2. Using the phone app, scan for the device and connect using the connection
password.
3. Press the unlock or lock button to issue the corresponding command to the
door lock.
4. With the door open, when entering the room, stand in front of the device and
keep the forehead close to the temperature sensor until the temperature is
successfully measured and the device plays the confirmation sound.
5. When leaving the room, stand in front of the device until the device plays
the confirmation sound.
6. In case the phone app cannot be used, for testing purposes, the user button
on the mainboard can be pressed to unlock or lock the door.
