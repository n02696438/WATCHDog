#!/usr/bin/python
import time
import subprocess
import Adafruit_CharLCD as LCD

lcd_rs = 'P8_8'
lcd_en = 'P8_10'
lcd_d4 = 'P8_18'
lcd_d5 = 'P8_16'
lcd_d6 = 'P8_14'
lcd_d7 = 'P8_12'
lcd_backlight = 'P8_7'

lcd_columns = 16
lcd_rows = 2

lcd = LCD.Adafruit_CharLCD(lcd_rs,lcd_en,lcd_d4,lcd_d5,lcd_d6,lcd_d7,lcd_columns,lcd_rows, lcd_backlight)

#lcd.set_backlight(1)
#lcd.clear()
message = 'Hold ID up\nto camera'
lcd.message(message)
time.sleep(10)
