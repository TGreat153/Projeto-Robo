from flask import Flask, request, jsonify
from flask_cors import CORS
import utils

app = Flask(__name__)
cors = CORS(app, resources={r'/api/*': {"origins": "*"}})
@app.route("/api/robo/manual", methods=["POST"])
def receber_dados_manual():
    dados = request.get_json()
    dado = utils.manual(dados)

    return jsonify({
            'message': f'O robo andou: {dado}',
            'resultado': dado
        })


@app.route("/api/robo/auto", methods=["POST"])
def receber_dados_auto():
    dados = request.get_json()
    dado = utils.auto(dados)

    return jsonify({
            'message': f'{dado}',
            'resultado': dado
        })


if __name__ == "__main__":
    app.run(debug=True, port=8080)