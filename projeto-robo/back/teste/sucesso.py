import cv2 as cv

def video():
    camera = cv.VideoCapture(0)
    if camera.isOpened():
        valite, frame = camera.read()
        color = input('Escolha uma cor: \n')
        height, width, channels = frame.shape
        height_center = int(height / 2)
        width_center = int(width / 2)
        tolerancia = 50

        print(f"altura central = {height_center}, largura central = {width_center}")
        while valite:
            valite, frame = camera.read()
            #cv.imshow("teste", frame)
            key = cv.waitKey(5)
            if key == 27: #ESC
                break
            hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
            mask = create_colors(hsv, color)
            contours, hierarchy = cv.findContours(mask, cv.RETR_CCOMP, cv.CHAIN_APPROX_NONE)
            if contours != ():
                bigger = max(contours, key=cv.contourArea)
                moment = cv.moments(bigger)
                if moment['m00']!= 0:
                    cx = int(moment["m10"] / moment['m00'])
                    cy = int(moment['m01'] / moment['m00'])
                    img = teste(frame, cx, cy)
                else:
                    cx = 0
                    cy = 0
                    img = teste(frame, 0, 0)
                x, y, w, h = cv.boundingRect(bigger)
                cv.circle(img, (width_center, height_center), 5, (0, 0, 255), -1)
                if abs(x - width_center) <= tolerancia and abs(y - height_center) <= tolerancia:
                    cv.imwrite("centro.png",img)
                    print(f"Coordenada x = {x}, y = {y}")
                    break
                cv.imshow("teste", img)


    camera.release()
    cv.destroyAllWindows()



def create_colors(hsv, color):
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

def teste(frame, x,y):
    size = 10     
    color = (255,0,0)
    cv.line(frame, (x - size,y),(x + size,y),color,5)
    cv.line(frame, (x,y - size),(x, y + size),color,5)

    return frame



video()