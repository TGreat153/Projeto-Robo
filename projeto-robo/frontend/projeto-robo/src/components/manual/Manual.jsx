import { useState, useEffect, useRef } from "react"

function Manual() {

  const [action, setAction] = useState("void");
  const [message, setmessage]= useState("");
  const botoesRef = useRef({});
  const handleEnviar = async (command) => {

    const send_command = command || action;

    try {
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
    } catch (erro) {
      console.error("Erro ao conectar com o backend:", erro);
      setmessage("Erro ao conectar com o servidor.");
      alert("Erro ao conectar com o servidor.");
    }
  }

  const press_button = (direction) => {
    setAction(direction)
    handleEnviar(direction)
  }

  useEffect(() => {
    const press_keyboard = (event) => {
      const mapping_keys = {
        ArrowUp: 'up',
        ArrowDown: 'down',
        ArrowLeft: 'left',
        ArrowRight: 'right',
      };

      const button = mapping_keys[event.key];
      if (button && botoesRef.current[button]) {
        event.preventDefault();
        botoesRef.current[button].click();
      }
    };

    window.addEventListener('keydown', press_keyboard);
    return () => window.removeEventListener('keydown', press_keyboard);
  }, []);
  
  return (
    <div className="text_body">
        <h1>Controle o robô pelo teclado ou clicando:</h1>
        <div className="buttons">
            <button type="button" onClick={() => press_button("straight")} ref={(el) => (botoesRef.current['up'] = el)} className="btn btn-dark">↑</button>
            <div id="double_line">
              <button ref={(el) => (botoesRef.current['left'] = el)} className="btn btn-dark" type="button" onClick={() => press_button("left")} className="btn btn-dark">←</button>
              <button type="button" className="btn btn-dark">•</button>
              <button ref={(el) => (botoesRef.current['right'] = el)} type="button" onClick={() => press_button("right")} className="btn btn-dark">→</button>
            </div>
            <button type="button" className="btn btn-dark" onClick={() => press_button("backward")} ref={(el) => (botoesRef.current['down']) = el}>↓</button>
        </div>
        <div className="buttons">
          <button type="button" onClick={() => press_button("stop")} className="btn btn-dark">Parar</button>
        </div>
    </div>
  )
}

export default Manual