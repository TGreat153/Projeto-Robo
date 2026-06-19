True:
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

#função manual(comando, url):
# if comando == "iniciar":
#   while comando != "parar":
#       função andar(comando, url):
# Else:
#   Mostrar aviso("Inicie o robo")


#função andar(comando, url):
# if frente
#   return "frente"
# if esquerda
#   return "esquerda"
# if direita
#   return "direita"
# if trás
#   return "trás"
# else:
#   return "Nada"

#Mapeamento de teclas:
# ↑ Para frente e acelerar
# ← Para esquerda
# → Para direita
# ↓ Para trás e desacelerar


#função auto(cor, url):
# if cor != "vazio":
#   dado_cor = função ver_cor(cor);
#   while comando != "parar":
#       video = centralizar_cor(dado_cor, url);
#       mover(video, url)
# Else:
#   Mostrar aviso("Inicie o robo")
