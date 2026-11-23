
import { createServer } from 'node:http'
import { execFile } from 'node:child_process'
import { fileURLToPath } from 'node:url'
import { dirname, resolve } from 'node:path'

const root = resolve(dirname(fileURLToPath(import.meta.url)), '..')

const orbitCli = resolve(root, 'build/orbit_cli.exe')
const samplesCli = resolve(root, 'build/orbit_samples_cli.exe')

function sendJSON(res, status, data) {
  res.writeHead(status, { 'Content-Type': 'application/json' })
  res.end(JSON.stringify(data))
}

function runCpp(executable, args, res) {
  execFile(
    executable,
    args,
    {
      timeout: 3000,
      maxBuffer: 1024 * 1024,
      windowsHide: true,
    },
    (error, stdout) => {
      if (error) {
        console.error(error)
        sendJSON(res, 500, { error: 'C++ execution failed' })
        return
      }

      try {
        sendJSON(res, 200, JSON.parse(stdout))
      } catch {
        sendJSON(res, 500, { error: 'Invalid C++ output' })
      }
    }
  )
}

const server = createServer((req, res) => {
  const url = new URL(req.url, 'http://localhost')

  // Una posición orbital
  if (url.pathname === '/api/orbit') {
    const time = Number(url.searchParams.get('time'))

    if (
      !url.searchParams.has('time') ||
      !Number.isFinite(time) ||
      time < 0 ||
      time > 1e9
    ) {
      sendJSON(res, 400, { error: 'Invalid time' })
      return
    }

    runCpp(orbitCli, [String(time)], res)
    return
  }

  // Varias posiciones orbitales
  if (url.pathname === '/api/orbit/samples') {
    const start = Number(url.searchParams.get('start'))
    const step = Number(url.searchParams.get('step'))
    const count = Number(url.searchParams.get('count'))

    if (
      !['start', 'step', 'count'].every(p => url.searchParams.has(p)) ||
      !Number.isFinite(start) ||
      !Number.isFinite(step) ||
      !Number.isInteger(count) ||
      start < 0 ||
      step <= 0 ||
      count < 2 ||
      count > 1000 ||
      start + step * (count - 1) > 1e9
    ) {
      sendJSON(res, 400, { error: 'Invalid sampling parameters' })
      return
    }

    runCpp(
      samplesCli,
      [String(start), String(step), String(count)],
      res
    )
    return
  }

  sendJSON(res, 404, { error: 'Not found' })
})

server.listen(3001, '127.0.0.1', () => {
  console.log('CubeSat API running at http://127.0.0.1:3001')
})
