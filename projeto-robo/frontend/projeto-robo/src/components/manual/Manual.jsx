import { useState } from "react"

function Manual() {

  const [action, setAction] = useState("void");

  return (
    <div className="text_body">
        <h1>Controle o robô pelo teclado ou clicando:</h1>
        <div className="buttons">
            <button type="button" onClick={() => setAction("straight")} className="btn btn-dark">↑</button>
            <div id="double_line">
              <button type="button" onClick={() => setAction("left")} className="btn btn-dark">←</button>
              <a type="button" className="btn btn-dark">•</a>
              <a type="button" onClick={() => setAction("right")} className="btn btn-dark">→</a>
            </div>
            <a type="button" onClick={() => setAction("left")} className="btn btn-dark">↓</a>
            <div>
              <a type="button" onClick={() => setAction("stop")} className="btn btn-dark">Parar</a>
              <a type="button" onClick={() => setAction("go")} className="btn btn-dark">Iniciar</a>
            </div>
        </div>

    </div>
  )
}

export default Manual