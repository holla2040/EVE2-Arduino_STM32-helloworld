# EVE2-helloworld
Here's my simplest FTDI EVE2 TFT display project for STM32F103 (bluepill).

I looked over all the 'sample' arduino code and documentation from the following FTDI, Bridgetek, Matrix Orbital, New Haven, Gameduino 2, FT81xmania, riverdi, and rudolphriedel and concluded that Matrix Orbital's [EVE2-School](https://github.com/MatrixOrbital/EVE-School) project was the simplest starting point. 

Starting there, I wrote this simple helloworld with both RAM_DL direct and FIFO Send_CMD operations demonstrating topics from the FT81x datasheet and programmer's guide. It's far from complete but it's simplest F103 stm32duino project to get the display to do something. I'm using both New Haven's 3.5 and 4.3 TFT display. This project demos screen and touch operations.
