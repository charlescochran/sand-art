# Sand Art Mod Software

Code for adding ESP32-powered lights and spinner to sand art.

# Usage

First, need to activate `esp-idf` environment by sourcing `export.sh` wherever
it is installed.

Then, in this repo, run `idf.py add-dependency "espressif/servo^0.1.0"`.

To flash, connect ESP32 and run `idf.py set-target esp32` and `idf.py flash`.
