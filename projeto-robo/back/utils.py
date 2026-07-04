import cv2 as cv
import requests

def manual(dados):
    # Pegar url
    ESP32_IP = "192.168.4.1"
    url = f"http://{ESP32_IP}"

    # Pegar comando
    action = dados['command']

    #Dar comandos
    if action == "straight":
        command = "/frente"
        mover(url, command)
    elif action == "left":
        command = "/esquerda"
        mover(url, command)
    elif action == "right":
        command = "/direita"
        mover(url, command)

    elif action == "backward":
        command = "/tras"
        mover(url, command)
    elif action == "stop":
        command = "/parar"
        mover(url, command)

def auto(dados, pre_url, thread_stop):
    # Pegar url
    ESP32_IP = "192.168.4.1"
    pre_url = f"http://{ESP32_IP}"
    url = f"{pre_url}/stream"

    #Pegar cor e limite
    action = dados['target']
    tolerancia = 20
    if action != 'void':
        while not thread_stop.is_set():
            camera = cv.VideoCapture(url)
            if camera.isOpened():
                valite, frame = camera.read()
                height, width, channels = frame.shape
                height_center = int(height / 2)
                width_center = int(width / 2)
                mask = search_color(dados, frame)
                identif_ballon(mask, width_center, height_center, tolerancia, frame)
    else:
        print('finalizado')

def mover(url, command):
        url_final = f"{url}{command}"
        try:
            requests.get(url_final, timeout=2)
        except:
            value = "Erro ao conectar a camera!"
            return value

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

def identif_ballon(mask, width_center, height_center, tolerancia, frame):
        contours, hierarchy = cv.findContours(mask, cv.RETR_CCOMP, cv.CHAIN_APPROX_NONE)
        if contours != ():
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
                elif position_x < -tolerancia:
                    pre_url = f"http://{ip}"
                    command = command = "/esquerda"
                    mover(pre_url, command)
                    command = command = "/frente"
                    mover(pre_url, command)
                    command = command = "/direita"