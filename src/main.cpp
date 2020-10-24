#include <Arduino.h>

#include <SPI.h>
#include <SD.h>
#include <LedControl.h>

const int confArr[][3] = {{49}, {3}, {200, 0, 0}, {0, 0, 200}, {0, 200, 0}}; // Configuration array: number of LEDs, number of colors, colors
const int numOfLeds = 198;                                                   // Sets the number of LEDs from the configuration array
const int ledData = 47;                                                      // Data pin for LED matrix
const int ledClock = 48;                                                     //MAX2719 clock pin
const int ledLoad = 49;                                                      //MAX2719 load pin
const int numOfLedSegments = 6;                                              //Number of MAX2719 chips used
const int chipSelect = 53;                                                   // Pin number for the SD card chip select pin
int ledArr[numOfLeds][2];                                                    // Array for storing the LEDs being turned on, and the color they use, first ellement is the LED posstion, second ellement is LED deafult color
int ledCounter = 0;                                                          // Counts the number of LEDs turned on in one command
String boulders[20];                                                         // Array for storing boulders from the SD card

File myFile;

LedControl lc1 = LedControl(ledData, ledClock, ledLoad, numOfLedSegments); //Sets's up the LED cotroller

void controlLeds(int infArr[][2]) //Turns on LEDs specified in the ledArr array
{
  if (ledCounter > 14)
    return;
  //Serial.println("Entered controlLeds function");
  //Serial.println(ledCounter);

  for (int i = 0; i < ledCounter; i++)
  {
    int x = infArr[i][0];
    int y = infArr[i][1];

    int mat = 0, xa = 0, ya = 0;

    if (x < 6)
    {
      //lijeva polovina
      if (y < 6)
      {
        //dolje lijevo, 0
        mat = 0;
        xa = 5 - x;
        ya = 5 - y;
      }
      else if (y > 11)
      {
        //gore lijevo, 2
        mat = 2;
        xa = 5 - x;
        ya = y - 12;
      }
      else
      {
        //srednje lijevo, 4
        mat = 4;
        xa = y - 6;
        ya = 5 - x;
      }
    }
    else
    {
      //desna polovina
      if (y < 6)
      {
        //dolje desno, 5
        mat = 5;
        xa = 5 - y;
        ya = x - 6;
      }
      else if (y > 11)
      {
        //gore desno, 1
        mat = 1;
        xa = 17 - y;
        ya = x - 6;
      }
      else
      {
        //srednje desno, 3
        mat = 3;
        xa = 11 - y;
        ya = x - 6;
      }
    }

    lc1.setLed(mat, ya, xa, true);
  }
}

void serialReadToArr(int fillArr[][2]) //Reads data from bluethooth module and stores them in a two dimensional array of an x by 2 size (only accepts numbers as high as the number of LEDs)
{

  Serial.println("Entered serialReadToArr function");

  byte currValue = 0;         // Holds value of the current recieved byte
  byte tmpStr[3] = {0, 0, 0}; // Temporerly holds recieved values until mergied in one number
  int digitCount = 0;         // Counts the number of digits in a recieved number
  bool typeIndicator = false; // False when recieveing LED possitions, true when recieveing LED color

  do
  {
    while (!Serial1.available()) // Make sure there is available data
    {
    };
    currValue = Serial1.read();     // Reads next Value
    tmpStr[digitCount] = currValue; // Puts the values on hold
    //Serial.println(currValue);

    if (tmpStr[digitCount] == ' ' || tmpStr[digitCount] == '\n') // Check if whole number was recieved
    {
      for (int i = 0; i < digitCount; i++) // Convert tmpStr to a number
      {
        fillArr[ledCounter][typeIndicator] = fillArr[ledCounter][typeIndicator] + (tmpStr[i] - 48) * (pow(10, digitCount - (i + 1)));
        //Serial.println("fillArr[ledCounter][typeIndicator]");
        //Serial.println(fillArr[ledCounter][typeIndicator]);
      }
      typeIndicator = !typeIndicator;

      if (!typeIndicator)
      {
        ledCounter++;
      };
      digitCount = 0;
    }
    else
    {
      digitCount++;
    }
  } while (currValue != '\n');
  //printArr(fillArr, 49);
}

void printArr(int inArr[][2], int arrLen) //Prints the values stored in a two dimensional array of size x by 2
{

  Serial.println("Entered printArr function");

  for (int i = 0; i <= arrLen - 1; i++)
  {

    Serial.print(inArr[i][0]);
    inArr[i][0] < 10 ? Serial.print("    ") : Serial.print("   ");
    Serial.println(inArr[i][1]);
  }
}

void initializeSDCard()
{
  Serial.println("Initializing SD card...");
  if (!SD.begin(chipSelect))
  {
    Serial.println("initialization failed!");
  }
  else
  {
    Serial.println("initialization done.");
  }
}

void readFileNames(File dir, String filesArr[])
{
  int fileCounter = 0;

  while (true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files
      break;
    }
    if (fileCounter)
    {
      filesArr[fileCounter - 1] = entry.name();
      Serial.println(filesArr[fileCounter - 1]);
    }
    fileCounter++;
    entry.close();
  }

  dir.close();
}

void readSDToArr(int fillArr[][2], String filesArr[], int arrLen, int fileNum) //Reads data from bluethooth module and stores them in a two dimensional array of an x by 2 size (only accepts numbers as high as the number of LEDs)
{

  Serial.println("Entered readSDToArr function");
  Serial.println("");

  char tmpStr[3] = {0, 0, 0}; // Temporerly holds recieved values until mergied in on
  int digitCount = 0;         // Counts the number of digits in a recieved number
  bool typeIndicator = false; //false when recieveing LED possitions, true when recieveing LED color

  File currentFile = SD.open(filesArr[fileNum - 48]); // Open the specified file

  if (currentFile) // Check if the file was succefully opened
  {
    Serial.println("Current file good");
  }
  else
  {
    Serial.println("Problem with current file");
  }

  do
  {
    tmpStr[digitCount] = currentFile.read();                                                                             // Get the next byte from the file
    if (tmpStr[digitCount] == ' ' || tmpStr[digitCount] == '\r' || tmpStr[digitCount] == '\n' || tmpStr[digitCount] < 0) // Check if end of number
    {
      for (int i = digitCount - 1; i >= 0; i--) // Convert the digits in tmpStr and save them
      {
        fillArr[ledCounter][typeIndicator] = fillArr[ledCounter][typeIndicator] + ((int)tmpStr[i] - 48) * (pow(10, (digitCount - (i + 1))));
      }
      typeIndicator = !typeIndicator;
      if (!typeIndicator)
      {
        ledCounter++;
      };
      digitCount = 0;
    }
    else
    {
      digitCount++;
    }
  } while (tmpStr[digitCount] >= 0); // Check if end of file
  currentFile.close();               // Close the file
}

void resetLeds(bool resetAll)
{ //Resets all LEDs
  //Serial.println("Entered resetLeds function");
  //Serial.println(ledCounter);

  for (int i = 0; i < 6; i++)
  {
    lc1.clearDisplay(i);
  }
}

void testLeds()
{
  Serial.println("Testing Leds");
  for (int i = 0; i < 6; i++)
  {
    for (int j = 0; j < 6; j++)
    {
      lc1.setLed(4, i, j, true);
      delay(100);
    }
  }
  for (int i = 0; i < 6; i++)
  {
    for (int j = 0; j < 6; j++)
    {
      lc1.setLed(4, i, j, false);
      delay(100);
    }
  }
}

void setup()
{

  Serial.begin(74880); //Initates serial cmunication with the PC
  Serial1.begin(9600); //Initates serial cmunication with the bluethooth module
  while (!Serial)
  {
    // wait for serial port to connect.
  }
  lc1.shutdown(0, false);
  lc1.shutdown(1, false);
  lc1.shutdown(2, false);
  lc1.shutdown(3, false);
  lc1.shutdown(4, false);
  lc1.shutdown(5, false);
  lc1.setIntensity(0, 8);
  lc1.setIntensity(1, 8);
  lc1.setIntensity(2, 8);
  lc1.setIntensity(3, 8);
  lc1.setIntensity(4, 8);
  lc1.setIntensity(5, 8);
  //  testLeds();
  resetLeds(true);
  initializeSDCard();

  myFile = SD.open("/");
  if (!myFile)
  {
    Serial.println("No myFile");
  }
  else
  {
    Serial.println("All good");
  }

  readFileNames(myFile, boulders);
  Serial.println("Setup finished");
}

void loop()
{

  if (Serial1.available())
  {
    char commandId = Serial1.read();
    Serial.println(int(commandId));
    switch (commandId)
    {
    case 76: //Poslati L i redne brojeve ledioda koje treba upaliti, LL C L C L C ... (L = LED possition, C = LED default color)
      Serial.println();
      Serial.println("Entered case 1");
      resetLeds(false);
      ledCounter = 0;
      memset(ledArr, 0, sizeof(ledArr));
      serialReadToArr(ledArr);
      controlLeds(ledArr);
      break;

    case 82: //Slanje R gasi sve lediode
      Serial.println();
      Serial.println("Entered case 2");
      resetLeds(false);
      memset(ledArr, 0, sizeof(ledArr));
      break;

    case 83:
      ledCounter = 0;
      memset(ledArr, 0, sizeof(ledArr));
      Serial.println();
      Serial.println("Entered case 3");
      while (!Serial1.available())
        ;
      int fileIndex = Serial1.read();
      readSDToArr(ledArr, boulders, 5, fileIndex);
      printArr(ledArr, ledCounter);
      controlLeds(ledArr);
      break;

    case 'M':
      Serial.println();
      Serial.println("Entered case 1");
      resetLeds(false);
      ledCounter = 0;
      memset(ledArr, 0, sizeof(ledArr));
      serialReadToArr(ledArr);
      controlLeds(ledArr);
    };
  };
}
