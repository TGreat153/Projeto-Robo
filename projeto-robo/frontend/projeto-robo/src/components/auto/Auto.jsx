import { useState } from "react"

function Auto() {

  const [color, setColor] = useState('void');

  const message = `A cor usada foi ${color}`

    const showAlert = () => {
    alert(message);
  };

  return (
    <div className="text_body">
        <h1>Escolha uma cor e aperte iniciar:</h1>
        <div className="buttons">
            <button type="button" onClick={() => setColor('red')} className="btn btn-danger">Vermelho</button>
            <button type="button" onClick={() => setColor('blue')} className="btn btn-primary">Azul</button>
            <button type="button" onClick={() => setColor('green')} className="btn btn-success">Verde</button>
        </div>
        <button type="button" onClick={showAlert} className="btn btn-dark btn-lg">Iniciar</button>
        <button type="button" className="btn btn-dark btn-lg">Parar</button>
    </div>
  )
}

export default Auto