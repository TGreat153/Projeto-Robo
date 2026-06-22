import { use, useState } from "react"

function Auto() {

  const [color, setColor] = useState('void');
  const [message, setmessage] = useState("")
  const send_target = async(target) => {
    try {
      const response = await fetch("http://localhost:8080/api/robo/auto",{
      method:"POST",
      headers:{
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        'target': target
      })
    });
    const dados = await response.json()
    setmessage(dados.message)
    alert(dados.message)
    setColor('void')
    } catch (erro) {
      console.error("Erro ao conectar com o backend:", erro);
      setmessage("Erro ao conectar com o servidor.");
      alert(dados.message)
    }
    finally {
      setColor('void')
    }
  }
  const choice_target = (target) => {
    setColor(target)
    send_target(target)
  }

  return (
    <div className="text_body">
        <h1>Escolha uma cor e aperte iniciar:</h1>
        <div className="buttons">
            <button type="button" onClick={() => setColor('red')} className="btn btn-danger">Vermelho</button>
            <button type="button" onClick={() => setColor('blue')} className="btn btn-primary">Azul</button>
            <button type="button" onClick={() => setColor('green')} className="btn btn-success">Verde</button>
        </div>
        <button type="button" onClick={() => choice_target(color)} className="btn btn-dark btn-lg" >Iniciar</button>
        <button type="button" onClick={() => choice_target('stop')} className="btn btn-dark btn-lg">Parar</button>
    </div>
  )
}

export default Auto