function Home() {
  return (
    <div className="text_body">
        <h1>Olá, escolha a opção desejada:</h1>
        <div className="buttons">
            <a href="/manual" type="button" className="btn btn-dark">Manual</a>
            <a href="" className="/" type="button" className="btn btn-dark">Auto</a>
        </div>
    </div>
  )
}

export default Home
