from flask import Flask, request, jsonify
from flask_cors import CORS
import utils
import threading

app = Flask(__name__)
cors = CORS(app, resources={r'/api/*': {"origins": "http://localhost:5173"}})

thread = None
thread_stop = threading.Event()

@app.route("/api/robo/manual", methods=["POST"])
def receber_data_manual():
    data = request.get_json()
    value = utils.manual(data)

    return jsonify({
        'message': f'O robo andou: {value}',
        'result': value
    })


@app.route("/api/robo/auto/start", methods=["POST"])
def receber_data_auto():
    global thread
    data = request.get_json()
    
    if thread and thread.is_alive():
        return jsonify({
            'message': 'Erro!',
            'result': 'O robô já está em modo automático. Aperte Parar primeiro.'
        }), 400

    thread_stop.clear()
    
    thread = threading.Thread(target=utils.auto, args=(data, thread_stop))
    thread.start()
        
    return jsonify({
        'message': 'Modo automático iniciado com sucesso!',
    })


@app.route("/api/robo/auto/stop", methods=["POST"])
def stop_loop():
    global thread
    
    thread_stop.set()

    return jsonify({
        'message': 'Comando de parada enviado!',
    })


if __name__ == "__main__":
    app.run(debug=True, port=8080)