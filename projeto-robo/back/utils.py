import cv2 as cv

def manual(dados):
    action = dados.get['command']
    if action == "straight":
        value = "Para frente"
    elif action == "left":
        value = "Para esquerda"
    elif action == "right":
        value = "Para direita"
    elif action == "backward":
        value = "Para trás"
    elif action == "stop":
        value = "Parou de andar"
    elif action == "void":
        value = "Começou a andar"
    return value

def auto(dados, url, thread_stop):
    url = 0
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
                identif_ballon(mask)
    else:
        print('finalizado')

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

def identif_ballon(mask, contours, width_center, height_center, tolerancia, frame):
        contours, hierarchy = cv.findContours(mask, cv.RETR_CCOMP, cv.CHAIN_APPROX_NONE)
        if contours != ():
            bigger = max(contours, key=cv.contourArea)
            x, y, w, h = cv.boundingRect(bigger)
            cv.circle(frame, (width_center, height_center), 5, (0, 0, 255), -1)
            if abs(x - width_center) <= tolerancia and abs(y - height_center) <= tolerancia:
                print("andar para frente")
            if abs(x - width_center) < 0:
                print("andar para direita")
            if abs(x - width_center) > 0:
                print("andar para esquerda")
            if abs(y - height_center) < 0:
                print("andar para frente")
            if abs(y - height_center) > 0:
                print("andar para trás")

                
def teste(frame, x,y):
    size = 10     
    color = (255,0,0)
    cv.line(frame, (x - size,y),(x + size,y),color,5)
    cv.line(frame, (x,y - size),(x, y + size),color,5)

    return frame