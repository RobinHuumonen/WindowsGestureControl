import pyautogui
import serial 
import time
import wmi
import math 

global hw_infos
global hwmon 
global cpu 
cpu = [] 

hwmon = wmi.WMI(namespace="root\OpenHardwareMonitor") 
arduino = serial.Serial('com11',9600, timeout=.7)  


def updateHWData():
    hw_infos = hwmon.Sensor()
    for sensor in hw_infos:
        if sensor.SensorType==u'Temperature':
            if sensor.Name == u'CPU Package':
                cpu.insert(0,  "CPU temp:%dC   " % sensor.Value) # %D = desimal(10-base)
            
while True: 
    updateHWData() 
    arduino.write(cpu[0].encode()) 
    incoming_data = str (arduino.readline())        
    # uncomment for debugging
    #print (incoming_data) 
    
    if 'next T     ' in incoming_data: 
        pyautogui.hotkey('ctrl', 'pgdn')

    if 'previous T' in incoming_data: 
        pyautogui.hotkey('ctrl', 'pgup')

    if 'mute      ' in incoming_data: 
        pyautogui.press('volumemute')

    if 'play/pause' in incoming_data: 
        pyautogui.press('playpause')
        pyautogui.press('k')     

    if 'volume D   ' in incoming_data:  
        pyautogui.press('volumedown') 
        pyautogui.press('volumedown')
        pyautogui.press('volumedown')
        pyautogui.press('volumedown')
      
    if 'volume U   ' in incoming_data: 
        pyautogui.press('volumeup')
        pyautogui.press('volumeup') 
        pyautogui.press('volumeup') 
        pyautogui.press('volumeup') 

    incoming_data = ""