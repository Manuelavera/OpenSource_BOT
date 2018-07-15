# SE ENVIA UNA FOTO A TWITTER AL RECIBIR LA ORDEN DESDE ARDUINO



# -*- coding: utf-8 -*-
#Importamos los paquetes necesarios
import utilidades
import RPi.GPIO as GPIO
import os
import time
from datetime import datetime, date


#Inicializamos y leemos el pin de arduino
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BOARD)
GPIO.setup(15, GPIO.IN)         
GPIO.setup(11, GPIO.OUT)


print"Ready!"

while True:
        
        try:
            i=GPIO.input(15)
            if i==0:                 
                time.sleep(0.1)
                print "Standby"
                GPIO.output(11,False)
            elif i==1:               
                print "Hago foto y la envío"
                GPIO.output(11,True)
                utilidades.enviar_foto_twitter()
                time.sleep(6)
        except KeyboardInterrupt:
                #Pulsar ctrl + C
                pass

