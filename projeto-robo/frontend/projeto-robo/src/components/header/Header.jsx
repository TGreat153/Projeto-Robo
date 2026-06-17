import logo from "../../assets/logo.jpeg"
function Header() {
  return (
    <div>
        <nav id='logo'>
          <img src={logo} alt="logo_poli" />
        </nav>
        <nav id='navbar'>
          <a href='/'>Inicio</a>
          <a href='/manual'>Manual</a>
          <a href=''>Auto</a>
          <a href=''>Sobre</a>
        </nav>
    </div>
  )
}

export default Header
