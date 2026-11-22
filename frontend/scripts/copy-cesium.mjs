
import { cpSync, mkdirSync } from 'node:fs'
import { join } from 'node:path'

const source = 'node_modules/cesium/Build/Cesium'
const destination = 'public/cesium'

const folders = ['Assets', 'Workers', 'Widgets', 'ThirdParty']

mkdirSync(destination, { recursive: true })

for (const folder of folders) {
  cpSync(
    join(source, folder),
    join(destination, folder),
    { recursive: true, force: true }
  )
}

console.log('Cesium assets copied successfully!')
