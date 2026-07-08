import { useState, useEffect, useRef } from "react"

function Manual() {

  const [action, setAction] = useState("void");
  const [message, setmessage]= useState("");
  const [warning, setWarning] = useState("Parado")
  const [loading, setLoading] = useState(false)
  const handleEnviar = async (command) => {

    const send_command = command || action;

    try {
      setLoading(true)
      const response = await fetch("http://localhost:8080/api/robo/manual",{
      method:"POST",
      headers:{
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        'command': send_command
      })
    });
    const dados = await response.json()
    if (!response.ok){
      throw new Error(`Erro ao conectar com o servidor ${response.status}`)
    }
    if (!dados.result){
      throw new Error(`Dados inválidos`)
    }
    setWarning(dados['result'])
    setLoading(false)
    } catch (erro) {
      console.error("Erro ao conectar com o backend:", erro);
      setmessage("Erro ao conectar com o servidor.");
    } finally{
      setLoading(false)
    }
  }

  const pressButton = (direction) => {
    setAction(direction)
    handleEnviar(direction)
  }

useEffect(() => {
  const press_keyboard = (event) => {
    const mapping_keys = {
      ArrowUp: 'straight',
      ArrowDown: 'backward',
      ArrowLeft: 'right',
      ArrowRight: 'left',
      Enter: 'stop',
    };
    const direction = mapping_keys[event.key];
    if (direction && !loading) {
      event.preventDefault();
      pressButton(direction)
    }
  };

  window.addEventListener('keydown', press_keyboard);
  return () => window.removeEventListener('keydown', press_keyboard);
}, [loading]);
  
  return (
    <div className="text_body">
        <h1>Controle o robô pelo teclado ou clicando:</h1>
        <div className="buttons">
            <button type="button" onMouseDown={() => pressButton("straight")} disabled={loading} className="btn btn-dark">↑</button>
            <div id="double_line">
              <button onMouseDown={() => pressButton("right")} className="btn btn-dark" type="button"disabled={loading} >←</button>
              <button  type="button"  className="btn btn-dark">•</button>
              <button  onMouseDown={() => pressButton("left")} className="btn btn-dark" disabled={loading}>→</button>
            </div>
            <button type="button" className="btn btn-dark" onMouseDown={() => pressButton("backward")} disabled={loading}>↓</button>
        </div>
        <div className="buttons">
          <button type="button" onClick={() => pressButton("stop")} disabled={loading} className="btn btn-dark">Parar</button>
        </div>
        <div>
          <h1>O robo {warning}</h1>
        </div>
    </div>
  )
}

export default Manual