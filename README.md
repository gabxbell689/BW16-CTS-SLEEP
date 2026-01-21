# BW16-CTS-SLEEP
CTS and Sleep frame attack use BW16 (RTL8720DN) board <br>
This attacks fall down the internet connection to be interrupted on the selected channel
or for the selected client.

# Hardware Requirements
- Ai-Thinker BW16 RTL8720DN Development Board

# Setup
1. Download Arduino IDE from [here](https://www.arduino.cc/en/software) according to your Operating System.
2. Install it.
3. Go to `File` → `Preferences` → `Additional Boards Manager URLs`.
4. Paste the following link :
   
   ```
   https://github.com/ambiot/ambd_arduino/raw/master/Arduino_package/package_realtek_amebad_index.json
   ```
5. Click on `OK`.
6. Go to `Tools` → `Board` → `Board Manager`.
7. Search `Realtek Ameba Boards (32-Bits ARM Cortex-M33@200MHz)` by `Realtek`. <br>
!!! CODE WORK WITH 3.1.7 pack, other versions may not be work !!!
9. Install it.
10. Restart the Arduino IDE.
11. Done!

# Install
1. Download or Clone the Repository.
2. Open the folder and open `cts&nulldata_attck.ino` in Arduino IDE.
4. Select board from the `Tools` → `Board` → `AmebaD ARM (32-bits) Boards`.
   - It is `Ai-Thinker BW16 (RTL8720DN)`.
5. Select the port of that board.
6. Go to `Tools` → `Board` → `Auto Flash Mode` and select `Enable`.
7. Upload the code.
   - Open Serial Monitor (115200) and write "scan", see networks, and choose one <br>
   "cts [channel 1-13, 36-165]" or "sleep [MAC/BSSID Router] [MAC Client] [channel of u router]" <br>
   exemple: "cts 6" or sleep "FFAABBCCEEDD 112233445566 56"

# Note
Tested only on provider none smart routers<br>
Code only for educational purposes, good luck
