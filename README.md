# AMIwrist

The AMIwrist is a controller for musical instrument augmentation (AMI).
It is part of the [Puara](https://github.com/Puara) framework.

The AMIwrist currently uses the [M5StickC](https://shop.m5stack.com/products/stick-c). Unfortunately, the embedded battery can only last around 30 min with the current firmware. Be advised to use a power bank or other USB-C power supply for longer performances.

## OSC namespaces

- continuous:
  - /AMIwrist_XXX/raw/accl [float] [float] [float] (in m/s^2)
  - /AMIwrist_XXX/raw/gyro [float] [float] [float] (in radians per second)
- discrete:
  - /AMIwrist_XXX/instrument/buttonA [0 or 1]
  - /AMIwrist_XXX/instrument/buttonB [0 or 1]
  - /AMIwrist_XXX/battery [int] (percentage)
  - /AMIwrist_XXX/vbattery [float] (in Volts)
- Instrument:
  - /AMIwrist_XXX/instrument/ypr [float] [float] [float] (in degrees)
  - /AMIwrist_XXX/instrument/jabxyz [float] [float] [float]
  - /AMIwrist_XXX/instrument/shakexyz [float] [float] [float]

### More Info on the related [GuitarAMI](https://github.com/Puara/GuitarAMI) and [Puara](https://github.com/Puara) research

[https://www.edumeneses.com](https://www.edumeneses.com)

[http://www-new.idmil.org/project/guitarami/](http://www-new.idmil.org/project/guitarami/)

## Licensing

The code in this project is licensed under the MIT license, unless otherwise specified within the file.
