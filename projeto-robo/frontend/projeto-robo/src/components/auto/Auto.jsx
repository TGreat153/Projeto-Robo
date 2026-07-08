import { useState } from "react"

function Auto() {

  const [color, setColor] = useState('void');
  const [message, setMessage] = useState("")
  const [loading, setLoading] = useState(false);
  const send_target = async(target) => {
    setLoading(true)
    try {
      const response = await fetch("http://localhost:8080/api/robo/auto/start",{
      method:"POST",
      headers:{
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        'target': target
      })
    });
    if (!response.ok){
      throw new Error(`Erro ao conectar com o servidor ${response.status}`)
    }
    setColor('void')
    setLoading(false)
    } catch (erro) {
      alert("Erro ao conectar com o servidor.")
    }
    finally {
      setColor('void')
    }
  }

   const stop_robot = async(target) => {
    setLoading(true)
    try {
      const response = await fetch("http://localhost:8080/api/robo/auto/stop",{
      method:"POST",
      headers:{
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        'target': target
      })
    });
    setColor('void')
    setLoading(false)
    } catch (erro) {
      alert("Erro ao conectar com o servidor.");
      setLoading(false)
    }
    finally {
      setColor('void')
      setLoading(false)
    }
  }

  return (
    <div className="text_body">
        <h1>Escolha uma cor e aperte iniciar:</h1>
        <div className="buttons">
            <button type="button" onClick={() => setColor('red')} className={`${color !== 'red' ? "btn btn-danger" : "btn btn-light"}`}>Vermelho</button>
            <button type="button" onClick={() => setColor('blue')} className={`${color !== 'blue' ? "btn btn-primary" : "btn btn-light"}`}>Azul</button>
            <button type="button" onClick={() => setColor('green')} className={`${color !== 'green' ? "btn btn-success" : "btn btn-light"}`}>Verde</button>
        </div>
        <div>
          <button type="button" onClick={() => send_target(color)} disabled={loading} className="btn btn-dark btn-lg" >Iniciar</button>
          <button type="button" onClick={() => stop_robot('stop')} disabled={loading} className="btn btn-dark btn-lg">Parar</button>
          </div>
        <div>
          <img src={"http://192.168.4.1:81/stream"} alt="video"></img>
        </div>
    </div>
  )
}

export default Auto