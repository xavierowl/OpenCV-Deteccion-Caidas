#!/usr/bin/env python3
## Importación del módulo para crear un socket
import socket


## Importación para el bot de telegram
import telegram

## Importación de los módulos necesarios para el uso de la red neuronal
from tensorflow.keras.preprocessing import image
from tensorflow.keras.optimizers import RMSprop
from datetime import *
import time as tiempo
import tensorflow as tf
import numpy as np
import base64
import requests
import subprocess

## Carga del bot
token = '1856302557:AAHqmoPlCbQjGySxlv0C2PXrxiQUvJGUfqM'

## Carga del modelo de la red neuronal
modelo = tf.keras.models.load_model("../Modelo/modelo.h5")
 
# Compilar modelo cargado y listo para usar.
modelo.compile(loss="binary_crossentropy",
             optimizer = RMSprop(lr = 0.001),
             metrics = ['accuracy'])

HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
PORT = 65432        # Port to listen on (non-privileged ports are > 1023)

bot_token = '1856302557:AAHqmoPlCbQjGySxlv0C2PXrxiQUvJGUfqM'
# ID Wilson
bot_chatID = '1754308402'
# ID Byron
#bot_chatID = '1836609434'
bot_message = 'Su familiar pudo sufrir una posible caida'

print("Modelo cargado y listo para usarse: ")

while True:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        conn, addr = s.accept()
        with conn:
            print('Validando imagen...')
            img = image.load_img("../Validacion/deteccion.jpg")
            ruta_video = "/home/pi/Desktop/OpenCV-Deteccion-Caidas/video.mp4"
            img = img.resize((200, 200))
                
            x = image.img_to_array(img)
            x = np.expand_dims(x, axis = 0)
            images = np.vstack([x])
            prediccion = modelo.predict(images)

            if prediccion == 0:
                print("Se ha detectado una posible caída (",prediccion,")")                

                with open("../Validacion/deteccion.jpg", "rb") as image2string:
                    converted_string = base64.b64encode(image2string.read())
                url = 'http://34.125.213.125:80/rest/caidas'
                caida = {
                     "imgbinary" : converted_string,
                     "precision": 4.0}

                x = requests.post(url, json = caida)

                video = open(ruta_video, 'rb')

                bot = telegram.Bot(token)
                bot.send_message(bot_chatID,"Su familiar pudo haber sufrido una caída:")
                bot.send_video(bot_chatID, video)
                
                #print(response.json)
                
                print("Caída almacenada y notificada")
            else:
                print("No se ha detectado una caída (",prediccion,")")

            while True:
                data = conn.recv(1024)
                if not data:
                    break
                if prediccion == 0:
                    conn.sendall(b'Se ha detectado una posible emergencia')
                else:
                    conn.sendall(b'No se ha detectado una posible emergencia')