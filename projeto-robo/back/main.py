from flask import Flask, request, jsonify
from flask_cors import CORS
import utils
import threading
from concurrent.futures import ThreadPoolExecutor

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
                'message': f'Erro!',
                'result': "Aparte Parar"
            })
    thread_stop.clear()
    with ThreadPoolExecutor() as executor:
        futuro = executor.submit(utils.auto, data, thread_stop)
        result = futuro.result()
        
    return jsonify({
            'message': 'Sucesso!',
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