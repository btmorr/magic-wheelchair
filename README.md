# magic-wheelchair

Controls for the Pusheen build for [Magic Wheelchair 2019](https://www.magicwheelchair.org/)

## Circuit

Wiring description:

The [LED panels used for the eyes](https://www.adafruit.com/product/607)
have a power wiring diagram found [here](https://learn.adafruit.com/32x16-32x32-rgb-led-matrix/powering)
and a data wiring diagram found [here](https://learn.adafruit.com/32x16-32x32-rgb-led-matrix/connecting-with-jumper-wires).

For this build, we are only using the blue LEDs, so R1, R2, G1, and G2 are wired
to ground instead of to the pins specified in the instructions. Using only the
blue channels has the advantage of not using (and therefore blocking) any of the
[Arduino](https://www.arduino.cc/en/Guide/ArduinoMega2560)'s interrupt-capable
input pins, and also using only one of the color channels reduces the max amount
of current draw used by the panels and makes us less likely to suffer from a
power shortage.

The data signal is wired into both of the eyes (simple 3 way join) so that the
animation is shown on both eyes in coordination. This means we can't do one-eye
animations, such as winking, but makes the whole thing much simpler to program
and debug.

A circular potentiometer is attached to the steering wheel to measure its
position.

Aside from the pins specified in the Adafruit instructions, these pins are used
for the buttons:

| Button               |  Pin  |
| -------------------- | ----: |
| circle eyes button   |     2 |
| star eyes button     |     3 |
| heart eyes button    |    18 |
| blink eyes button    |    19 |
| steering wheel       |    A8 |

We are using the [DFPlayer Mini](https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299) for playing audio files. It takes serial commands as input and plays mono audio. Connections required as simply Vcc, Gnd, Rx (level shifted), Tx and speaker connections. The audio files are stored as .mp3 on the microSD card as described in the [manual](https://github.com/Arduinolibrary/DFPlayer_Mini_mp3/raw/master/DFPlayer%20Mini%20Manual.pdf)

Wiring:
| DFPlayer pin          | Mega pin  |
| --------------------- | --------- |
| Rx (via 1k res)       |       A11 |
| Tx                    |       A12 |
| Vcc                   |       +5V |
| Gnd                   |       gnd |

Other connections:
Volume control potentiometer --> Mega Pin A5
DFPlayer SPK+ & SPK- pins    --> Speaker + & - terminals (8ohm, 3W speaker)

Requirements for input and behaviors:

- The steering wheel should cause the pupils to move back and forth in the same direction as the steering
- A button should turn the pupils into circles, which should be steerable
- A button should turn the pupils into stars, which should be steerable
- A button should turn the pupils into hearts, which should be steerable
- A button should start a blink-eyes animation (circular pupils)
- After blink-eyes or rolling-eyes, the pupils should go back to their previous shape (circles, stars, or hearts)
- Each button press should trigger a distinct sound.
