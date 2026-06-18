function Manual() {
  return (
    <div className="text_body">
        <h1>Controle o robô pelo teclado ou clicando:</h1>
        <div className="buttons">
            <a type="button" className="btn btn-dark">↑</a>
            <div id="double_line">
              <a type="button" className="btn btn-dark">←</a>
              <a type="button" className="btn btn-dark">•</a>
              <a type="button" className="btn btn-dark">→</a>
            </div>
            <a type="button" className="btn btn-dark">↓</a>
            <div>
              <a type="button" className="btn btn-dark">Parar</a>
              <a type="button" className="btn btn-dark">Iniciar</a>
            </div>
        </div>

    </div>
  )
}

export default Manual