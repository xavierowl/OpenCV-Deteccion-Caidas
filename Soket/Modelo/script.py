## Importación de los métodos necesarios
from tensorflow.keras.preprocessing import image
from tensorflow.keras.optimizers import RMSprop
## import matplotlib.pyplot as plt
import tensorflow as tf
import numpy as np

#modelo = tf.keras.models.load_model("modelo/modelo.h5")
modelo = tf.keras.models.load_model("modelo.h5")
 
# Compilar modelo cargado y listo para usar.
modelo.compile(loss="binary_crossentropy",
             optimizer = RMSprop(lr = 0.001),
             metrics = ['accuracy'])

# img = image.load_img("Validacion/deteccion.jpg")
img = image.load_img("../Validacion/deteccion.jpg")
img = img.resize((200, 200))
    
x = image.img_to_array(img)
x = np.expand_dims(x, axis = 0)
images = np.vstack([x])
prediccion = modelo.predict(images)

if prediccion == 0:
    print("Se ha detectado una posible caída (",prediccion,")")
else:
    print("No se ha detectado una caída (",prediccion,")")