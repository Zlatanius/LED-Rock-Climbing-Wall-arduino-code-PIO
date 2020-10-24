- # LED Rock Climbing Wall Arduino Code

  This is the Arduino code for the hardware part of the [LED Rock Climbing Project](https://github.com/Zlatanius/LED-Rock-Climbing-Wall-RN-project#warnings).

  ### Motivation

  Outdoor rock climbing is a sport where the objective is to get to the top of the mountain using only your hands and feet. Indoor rock climbing is quite similar apart from the fact that you are climbing on artificial rock. This is usually fine, however sometimes there are too many holds placed very close together and it becomes hard to tell which ones you are supposed to use. That is the problem this project aims to solve. By placing LEDs under each hold and being able to choose which LEDs turn on using a mobile app it would become much easier to tell which holds you are supposed to grab onto. Apart from showing you the holds you are using, you would also be able to create new boulders (rock climbing jargon for climbing paths) and save them for other users to try out and climb themselves. While a product like this already exists, called [The Moon Board](https://www.moonboard.com/moonboard-app), it only works for a standardized climbing wall with a grid layout. My aim is to make a system that would work on any climbing wall, even if the holds are not arranged in a regular pattern.

  

  ## Warnings

  The MAX7219 approach is far more challenging and brings with it many difficulties. However it is the approach I have used since at the time of making the first Moonboard we ware not able to get any WS28xx LEDs. [This](https://www.instructables.com/Large-8x8-LED-Matrix-Display/) Instructable is a great resource for making your own MAX7219 multiplexer drivers and matrices. Another limitation of using the MAX7219 is that it requires the LEDs be arranged in a regular grid.

  

  ## Hardware

  - Arduino Mega
  - A generic SD card module with a logic level shifter
  - HC-06 bluetooth module (this module requires a 3.3 volt input signal)
  - Either any of the WS2811/WS2812(b)/WS2813 RGB LEDs or generic LEDs with the MAX7219 multiplexer chip.

  

  ## Installation

  The code was written using the PlatformIO IDE extension but in essence is a classic Arduino sketch and should work either in the Arduino IDE or by using the VS code Arduino extension.

  However it will not work in the Arduino online editor. The reason for this is that to achieve adequate transfer speeds between the bluetooth module and the Arduino board the input serial buffer size has to be increased to 256 bits which is not possible in the online editor.

  To increase the input serial buffer size follow this guide: https://internetofhomethings.com/homethings/?p=927

  #### Used Libraries:

  - FastLED - for using WS28xx LEDs
  - LedControl - for using the MAX7219 chip (if using the PlatformIO library installer select the second the second most popular library with this name)
  - SPI
  - SD

  ## Functionality

  This system works by sending the commands from [the mobile app](https://github.com/Zlatanius/LED-Rock-Climbing-Wall-RN-project#warnings) to the bluetooth module. The HC-06 module passes any data it receives to the Arduino over UART. The Arduino then interprets the commands and adjusts the LEDs accordingly. So far there are only four commands that can be sent.

  #### LED control (WS28xx):

  This is the main command for controlling the LEDs. It says exactly which LEDs need to be turned on and in what color. The structure of the command goes like this:

  "**LN C N C N C**...". 

  **L** is the id of the command, it tells the board that a string of LED instructions is coming.

  **N** is the serial number of the LED, since all the LEDs are connected in series (the signal lines are in connected in series, the VCC and ground are in connected in parallel) each LED has a serial number starting from 0.

  **C** is represents the color of the LED. The colors are defined in the configuration array at the top of the sketch.

  An example of an LED control command would be: "**L0 1 11 1 27 2**"

  

  #### LED control (MAX7219):

  This command is almost exactly the same as the previous one but is used when controlling the LEDs with the MAX7219 chip.

  

  The command structure is:

  "**MX Y X Y X Y**..."

  **M** is the command id.

  **X** is the x coordinate of the LED you want to have turned on.

  **Y** is the y coordinate of the LED you want to have turned on.

  The 0,0 coordinate is located in the bottom left corner.

  

  #### Reset:

  The reset command simply turns off all the LEDs. It is called by sending the letter "**R**".

  #### Read from SD card:

  This command is used to read LED control commands from the SD card. Each command is stored in a separate text file. Each file represents one climbing path (boulder). The structure goes like this:

  "**SN**"

  **S** is the id of the command.

  **N** is the serial number of the file from which to load the boulder.

  Example: "**S1**"

  

  ## Future Plans

  - Add the ability to save boulders to the SD card so that you or other users can revisit them.
  - Make it possible to preview the saved boulders in the app
