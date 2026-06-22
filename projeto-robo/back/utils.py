def manual(dados):
    action = dados['command']
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

def auto(dados):
    action = dados['target']
    if action == "red":
        value = "Até o vermelho"
    elif action == "blue":
        value = "Até o azul"
    elif action == "green":
        value = "Até o verde"
    elif action == "void":
        value = "Por favor, escolha uma cor!"
    elif action == "stop":
        value = "Fluxo interrompido!"
    return value