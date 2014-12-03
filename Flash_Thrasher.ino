/*
 Test how many times we can write to an EEPROM location.
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 24th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 This is a "bad idea" program. It writes over and over again to an EEPROM location to see how many writes
 it takes before the location fails to record a value.
 
 From datasheet: 
 Write/Erase Cycles: 10,000 Flash/100,000 EEPROM
 3.4ms per erase/write
 
 Space 1600 destroyed in 10,217,936 writes. 
 Space 1599 destroyed in 16,554,910 writes.
 
 */

#include <EEPROM.h>
#include <avr/pgmspace.h>

const byte statLED = 13;  //Flashes with incoming serial traffic

unsigned long largestCount;
byte largestSpot;

//Each counter location needs to store a number up to 100,000 or more
//We will have 10 counters that will be written to every 10 writes
#define COUNTER_LOCATION_START  1
#define COUNTER_WIDTH  4 //We use a 4-byte (4.29B) unsigned long.
#define COUNTER_SPREAD 100 //Number of locations we spread this counter across

//EEPROM on the ATmega328 goes from 0 to 2048
//We recommend you destroy a location you won't every use in upper memory like 1492 or something
//Avoid 0
//Destroyed: 1600, 1599
#define LOCATION_TO_DESTROY  1598

void setup()
{
  Serial.begin(115200);
  Serial.println("Flash Thrasher");

  pinMode(statLED, OUTPUT);

  //Once we've nuked a location we use this to reset all the counter locations
  //for(int x = COUNTER_LOCATION_START ; x < COUNTER_LOCATION_START + (COUNTER_WIDTH * COUNTER_SPREAD) ; x++)
  //  EEPROM.write(x, 0xFF);
    
  //Find the largest count in EEPROM
  findLargestCount();

  Serial.print("Largestspot: ");
  Serial.println(largestSpot);
  Serial.print("LargestCount: ");
  Serial.println(largestCount);
}

void loop()
{
  if(digitalRead(statLED) == HIGH)
    digitalWrite(statLED, LOW);
  else
    digitalWrite(statLED, HIGH);

  //Write to an EEPROM location 10 times then increment counter
  for(byte x = 0 ; x < 10 ; x++)
  {
    EEPROM.write(LOCATION_TO_DESTROY, largestCount & 0xFF);

    byte check = EEPROM.read(LOCATION_TO_DESTROY);

    if(check == (largestCount & 0xFF))
    {
      //Write was successful
      largestCount++;
    }
    else
    {
      //Write failed!
      Serial.println();
      Serial.println("Failure!");

      Serial.print("Spot should be: ");
      Serial.print(largestCount & 0xFF);
      Serial.print(" We read: ");
      Serial.println(check);

      Serial.print("Count: ");
      Serial.println(largestCount);

      Serial.print("Elapsed time: ");
      long seconds = (largestCount/10 * (long)45) / (long)1000; //Each cycle takes 45ms for 10 writes/reads
      Serial.print(seconds);
      Serial.println(" seconds");

      Serial.println("Stopped");
      while(1);
    }

  }

  Serial.print("Count: ");
  Serial.println(largestCount);

  largestSpot += COUNTER_WIDTH;
  //Look for wrap-around condition
  if(largestSpot == (COUNTER_LOCATION_START + (COUNTER_WIDTH * COUNTER_SPREAD))) largestSpot = COUNTER_LOCATION_START;

  //Record the latest counter
  writeBytes(largestSpot, largestCount, COUNTER_WIDTH);

  delay(1); //For serial printing
}

//Steps through the counter area and finds the largest available counter
void findLargestCount()
{
  largestCount = 0;
  largestSpot = COUNTER_LOCATION_START;

  //Start looking at the beginning, step 3 bytes each loop, stop after 10 slots
  for(int spot = COUNTER_LOCATION_START ; spot < (COUNTER_LOCATION_START + (COUNTER_WIDTH * COUNTER_SPREAD)) ; spot += COUNTER_WIDTH)
  {
    unsigned long count = readBytes(spot, COUNTER_WIDTH);

    Serial.print("[");
    Serial.print(spot);
    Serial.print("] ");

    if(count == 0xFFFFFFFF)
    {
      Serial.println(" Untouched");
    }
    else
    {
      Serial.print(count);

      if(count > largestCount)
      {
        Serial.print(" Largest");
        largestCount = count;
        largestSpot = spot;
      }

      Serial.println();
    }
  }
}

//Record a series of bytes to EEPROM starting at address
void writeBytes(byte address, long setting, byte sizeOfSetting)
{
  for(byte x = 0 ; x < sizeOfSetting ; x++)
  {
    byte toWrite = setting >> 8 * (sizeOfSetting - 1 - x);
    EEPROM.write(address + x, toWrite);   
  }
}

//Give a location read out a number of bytes
unsigned long readBytes(byte address, byte sizeOfSetting)
{
  unsigned long setting = 0;

  for(byte x = 0 ; x < sizeOfSetting ; x++)
  {
    setting <<= 8;
    setting |= EEPROM.read(address + x);
  }

  return(setting);
}

