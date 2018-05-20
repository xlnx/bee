const electron = require('electron')
const app = electron.app
const BrowserWindow = electron.BrowserWindow

const path = require('path')
const url = require('url')

let mainWindow

function createWindow () {
  app.commandLine.appendSwitch('--disable-gpu-vsync')
  app.commandLine.appendSwitch('--use-gl=desktop')

  mainWindow = new BrowserWindow({
    width: 1024,// - 496, 
    height: 1024,// - 453, 
    'web-preferences': {plugins: true}
  })

  mainWindow.loadURL(url.format({
    pathname: path.join(__dirname, 'build/index.html'),
    protocol: 'file:',
    slashes: true
  }))

  mainWindow.on('closed', function () {
    mainWindow = null
  })
}

app.on('ready', createWindow)

app.on('window-all-closed', function () {
  if (process.platform !== 'darwin') {
    app.quit()
  }
})

app.on('activate', function () {
  if (mainWindow === null) {
    createWindow()
  }
})
