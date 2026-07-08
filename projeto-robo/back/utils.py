import cv2 as cv
import requests
import time


#Controlar manualmente
def manual(dados):
    # Pegar url
    esp_ip = "192.168.4.1"
    url = f"http://{esp_ip}"
    # Pegar comando
    action = dados['command']
    #Dar comandos
    if action == "straight":
        command = "/frente"
        mover(url, command)
        command = "/parar"
        mover(url, command)
        value = "está em frente!"
        return value
    elif action == "left":
        command = "/esquerda"
        mover(url, command)
        command = "/parar"
        mover(url, command)
        value = "está girando!"
        return value
    elif action == "right":
        command = "/direita"
        mover(url, command)
        command = "/parar"
        mover(url, command)
        value = "está girando!"
        return value
    elif action == "backward":
        command = "/tras"
        mover(url, command)
        command = "/parar"
        mover(url, command)
        value = "está de Ré"
        return value
    elif action == "stop":
        command = "/parar"
        mover(url, command)
        value = "está parando"
        return value

#Buscar automaticamente
def auto(dados, thread_stop):
    # Pegar url
    esp_ip = "192.168.4.1:81"
    pre_url = f"http://{esp_ip}"
    url = f"{pre_url}/stream"
    #Pegar cor e limite
    action = dados['target']
    tolerancia = 20
    if action == 'void':
         print("Escolha um valor!")
         return
    camera = cv.VideoCapture(url)

    try:
         while not thread_stop.is_set():
            if not camera.isOpened():
                print("Erro camera não recohecida!")
                time.sleep(1)
                camera.open(url)
                continue
            valide, frame = camera.read()
            if not valide:
                 time.sleep(0.5)
                 continue
            
            height, width, channels = frame.shape
            height_center = int(height / 2)
            width_center = int(width / 2)

            mask = search_color(action, frame)
            if mask is not None:
                identif_ballon(mask, width_center, height_center, tolerancia)
    finally:
         if camera.isOpened():
              camera.release()

#Buscar a cor correta
def mover(url, command):
        url_final = f"{url}{command}"
        try:
            requests.get(url_final, timeout=2)
        except:
            value = "Erro ao conectar a camera!"
            return value

#Criar máscara de cor
def search_color(color, frame):
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
    if color =='red':
        red1 = cv.inRange(hsv, (0, 100, 100), (10, 255, 255))
        red2 = cv.inRange(hsv, (170, 100, 100), (180, 255, 255))
        mask_red = cv.bitwise_or(red1, red2)
        return mask_red
    if color =='blue':
        mask_blue = cv.inRange(hsv, (100, 50, 50), (130, 255, 255))
        return mask_blue
    if color =='green':
        mask_green = cv.inRange(hsv, (40, 50, 50), (80, 255, 255))
        return mask_green

#Buscar balão
def identif_ballon(mask, width_center, height_center, tolerancia, frame):
        contours, hierarchy = cv.findContours(mask, cv.RETR_CCOMP, cv.CHAIN_APPROX_NONE)
        if len(contours) > 0:
            bigger = max(contours, key=cv.contourArea)
            x, y, w, h = cv.boundingRect(bigger)
            b_x = x + int(w/2)
            position_x = b_x - width_center
            ip = "192.168.4.1"
            if position_x <= tolerancia:      
                    pre_url = f"http://{ip}"
                    command = command = "/frente"
                    mover(pre_url, command)
            else:
                if position_x > tolerancia:
                    pre_url = f"http://{ip}"
                    command = command = "/direita"
                    mover(pre_url, command)
                    command = command = "/frente"
                    mover(pre_url, command)
                    command = command = "/esquerda"
                    mover(pre_url, command)
                elif position_x < tolerancia:
                    pre_url = f"http://{ip}"
                    command = command = "/esquerda"
                    mover(pre_url, command)
                    command = command = "/frente"
                    mover(pre_url, command)
                    command = command = "/direita"
                    mover(pre_url, command)