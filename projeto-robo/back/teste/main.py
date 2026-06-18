import cv2 as cv
import numpy as np

# Carregar video
video = cv.VideoCapture('video.mp4')
while True:
    # Ler frame do video
    ret, frame = video.read()
    if not ret:
        break
    # Sair com a tecla 'q'
    if cv.waitKey(1) & 0xFF == ord('q'):
        break

def ler_video(url, cor, comando):
    video = cv.VideoCapture(url)
    while True:
        # Ver vídeo
        if str(comando).strip().lower() == "exit":
            break
        pos = ver_video(video)
        andar(pos)
        
def ver_video(video, cor):
    hsv = cv.cvtColor(video, cv.COLOR_BGR2RGB)
    # Definir cor
    cor_maior = ""
    cor_menor = ""
    if str(cor).strip().lower() == "azul":
        cor_menor = np.array([110, 50, 50])
        cor_maior = np.array([130, 255, 255])
    elif str(cor).strip().lower() == "vermelho":
        cor_menor = np.array([0, 70, 50])
        cor_maior = np.array([10, 255, 255])
    elif str(cor).strip().lower() == "verde":
        cor_menor = np.array([35, 50, 50])
        cor_maior = np.array([35, 255, 255])
    if cor_maior != "" and cor_menor != "":
        ver = cv.inRange(hsv, cor_menor, cor_maior)
    # Ver se cor está no frame
    # Retornar posição da cor
    pass

def andar(pos):
    # Cor na direita
    # Cor na esquerda
    # Cor na frente
    pass