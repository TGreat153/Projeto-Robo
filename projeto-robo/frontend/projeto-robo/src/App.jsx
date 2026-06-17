import Header from './components/header/Header'
import Home from './components/home/Home';
import Manual from './components/manual/Manual';
import { BrowserRouter, Routes, Route } from 'react-router-dom';
import "./App.css"
function App() {
  return (
    <BrowserRouter>
      <div>
        <Header />
      </div>
      <Routes>
        <Route path='/' element={<Home />}></Route>
        <Route path='/manual' element={<Manual />}></Route>
        <Route path='/' element={<Home />}></Route>
      </Routes>
    </BrowserRouter>
  )
}

export default App
