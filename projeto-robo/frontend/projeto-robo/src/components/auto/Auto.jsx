function Auto() {
  return (
    <div className="text_body">
        <h1>Escolha uma cor e aperte iniciar:</h1>
        <div className="buttons">
            <button type="button" className="btn btn-danger">Vermelho</button>
            <button type="button" className="btn btn-primary">Azul</button>
            <button type="button" className="btn btn-success">Verde</button>
        </div>
        <button type="button" className="btn btn-dark btn-lg">Iniciar</button>
    </div>
  )
}

export default Auto