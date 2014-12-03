Flash Thrasher
=======

How many times can we write to an EEPROM location inside an ATmega328 on an Arduino? This code repeatedly writes to a location then reads from it to test if the write was successful. Atmel guarantees 100,000 cycles but what happens at 100,001? Nothing actually. Testing on a [RedBoard](https://www.sparkfun.com/products/12757) with ATmega328P (date code 1224 = 24th week of 2012) it takes an average of 13,000,000 writes before the location stops functioning. Nice work Atmel.

![Serial output from program](https://raw.githubusercontent.com/nseidle/Flash_Thrasher/master/Failure-2.png)

While it can take 12 hours to destroy a location this is a pretty mean thing to sneak onto someone's board. Be nice.

Load this code onto an Arduino then open a serial terminal at 115200bps. The output will show how many writes have been successful and will stop when the EEPROM location stops responding.

In case of power or system reset within the 12 hours of thrashing the running counter is recorded to EEPROM. Yea, silly idea, so we spread the counter across 100 EEPROM locations and update the counter every tenth write/read cycle. This should stress the 8th counter spot with:

13,000,000 / 10 / 100 = 13,000 cycles


License Information
-------------------

This code is public domain but you buy me a beer if you use this and we meet someday ([Beerware license](http://en.wikipedia.org/wiki/Beerware)).

