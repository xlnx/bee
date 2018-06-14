const electron = require('electron')
const app = electron.app
const BrowserWindow = electron.BrowserWindow
const Menu = electron.Menu

const path = require('path')
const url = require('url')

let mainWindow

function createWindow () {
  // app.commandLine.appendSwitch('--disable-gpu-vsync')
  app.commandLine.appendSwitch('--use-gl=desktop')
  app.commandLine.appendSwitch('--enable-webgl-draft-extensions')

  Menu.setApplicationMenu(null)
  mainWindow = new BrowserWindow({
    width: 1200, 
    height: 900, 
    frame: false, 
    resizable: false,
    'web-preferences': {
      plugins: true
    }
  })
  mainWindow.webContents.openDevTools()
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
