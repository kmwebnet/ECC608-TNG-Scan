# ECC608-TNG-Scan

This communicates ATECC608A-TNGTLS secure chip from ESP32 and read out certificate chain.       

# Requirements

Platformio(PIO Core:5.0.1 PLATFORM: Espressif 32 1.12.4) with VS Code environment.  
install "Espressif 32" platform definition on Platformio  

you need to buy ATECC608A-TNGTLS.   

# Environment reference
  
  Espressif ESP32-DevkitC  
  this project initialize I2C 1 port   
  pin assined as below:  

      I2C 1 SDA GPIO_NUM_16  
      I2C 1 SCL GPIO_NUM_17  
       
  ATECC608A-TNGTLS(on I2C port 1)   

# Usage  

"git clone --recursive " on your target directory. and you need to change a serial port number which actually connected to ESP32 in platformio.ini.    

# Run this project

just execute "Upload" on Platformio.   

# License

This software is released under the MIT License, see LICENSE.  
