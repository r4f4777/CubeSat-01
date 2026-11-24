import { useEffect, useRef, useState } from 'react'
import type { Entity } from 'cesium'
import {
  Viewer,
  Cartesian3,
  Cartesian2,
  Color,
  JulianDate,
  Ion,
  ArcType,
  IonImageryProvider,
  TileMapServiceImageryProvider,
  buildModuleUrl,
  SampledPositionProperty,
  LinearApproximation,
} from 'cesium'

import 'cesium/Build/Cesium/Widgets/widgets.css'
import './App.css'



// ParÃ¡metros de nuestra Ã³rbita circular
const EARTH_RADIUS_KM = 6371
const ALTITUDE_KM = 500
const MU = 398600.4418

const radiusKm = EARTH_RADIUS_KM + ALTITUDE_KM
const speedKmS = Math.sqrt(MU / radiusKm)
const periodSeconds = (2 * Math.PI * radiusKm) / speedKmS

Ion.defaultAccessToken = import.meta.env.VITE_CESIUM_ION_TOKEN

function App() {
  const containerRef = useRef<HTMLDivElement>(null)
  const viewerRef = useRef<Viewer | null>(null)
  const satelliteRef = useRef<Entity | null>(null)
  const [isPlaying, setIsPlaying] = useState(true)
  const [simulationSpeed, setSimulationSpeed] = useState(100)
  const [missionSeconds, setMissionSeconds] = useState(0)


  function showGlobalView() {
    const viewer = viewerRef.current

    if (!viewer) return


    // Dejar de seguir al satÃ©lite
    viewer.trackedEntity = undefined

    const cameraPosition = new Cartesian3(
      0,
      -25000000,
      18000000
    )

    viewer.camera.flyTo({
      destination: cameraPosition,
      orientation: {
        direction: Cartesian3.normalize(
          Cartesian3.negate(cameraPosition, new Cartesian3()),
          new Cartesian3()
        ),
        up: Cartesian3.UNIT_Z,
      },
      duration: 2,
    })
  }

  function followSatellite() {
    const viewer = viewerRef.current
    const satellite = satelliteRef.current

    if (!viewer || !satellite) return

    // Distancia y orientaciÃ³n inicial de la cÃ¡mara
    // respecto al CubeSat (metros, sistema local).
    satellite.viewFrom = new Cartesian3(
      -1500,
      -3500,
      1800
    )

    // Activar seguimiento automÃ¡tico
    viewer.trackedEntity = satellite
  }


  function toggleSimulation() {
    const viewer = viewerRef.current
    if (!viewer) return

    const nextState = !viewer.clock.shouldAnimate

    viewer.clock.shouldAnimate = nextState
    setIsPlaying(nextState)
  }

  function changeSimulationSpeed(speed: number) {
    const viewer = viewerRef.current
    if (!viewer) return

    viewer.clock.multiplier = speed
    setSimulationSpeed(speed)
  }


  useEffect(() => {
    if (!containerRef.current) return

    const viewer = new Viewer(containerRef.current, {
      baseLayer: false,
      baseLayerPicker: false,
      animation: false,
      timeline: false,
      geocoder: false,
      homeButton: false,
      sceneModePicker: false,
      navigationHelpButton: false,
      fullscreenButton: false,
      infoBox: false,
      selectionIndicator: false,
    })

    viewerRef.current = viewer


    // ========================================
    // OBJETIVO TERRESTRE: MADRID
    // ========================================

    const madridPosition = Cartesian3.fromDegrees(
      -3.7038,  // Longitud oeste
      40.4168,   // Latitud norte
      10000      // Altura visual del marcador: 10 km
    )

    viewer.entities.add({
      name: 'Madrid - Photography Target',
      position: madridPosition,

      point: {
        pixelSize: 12,
        color: Color.RED,
        outlineColor: Color.WHITE,
        outlineWidth: 2,
        disableDepthTestDistance: Number.POSITIVE_INFINITY,
      },

      label: {
        text: 'MADRID - TARGET',
        font: '15px Arial',
        fillColor: Color.WHITE,
        outlineColor: Color.BLACK,
        outlineWidth: 2,
        pixelOffset: new Cartesian2(0, -25),
        disableDepthTestDistance: Number.POSITIVE_INFINITY,
      },
    })

    // Comprobar conexiÃ³n con nuestro motor C++


    /*async function testOrbitSamples() {
      try {
        const response = await fetch(
          '/api/orbit/samples?start=0&step=60&count=100'
        )

        if (!response.ok) {
          throw new Error(`HTTP ${response.status}`)
        }

        const samples = await response.json()

        console.log(
          'ðŸ›° Muestras orbitales recibidas desde C++:',
          samples.length,
          samples
        )
      } catch (error) {
        console.error('Error recibiendo muestras:', error)
      }
    }

    void testOrbitSamples()*/


    /*async function testCppConnection() {
      try {
        const response = await fetch('/api/orbit?time=1417')

        if (!response.ok) {
          throw new Error(`HTTP ${response.status}`)
        }

        const position = await response.json()

        console.log('Coordenadas recibidas desde C++:', position)

        if (viewer.isDestroyed()) return

        // OrbitSimulator devuelve kilÃ³metros.
        // Cesium utiliza metros.
        const positionMeters = new Cartesian3(
          position.x * 1000,
          position.y * 1000,
          position.z * 1000
        )

        // Marcador de prueba calculado por C++
        viewer.entities.add({
          name: 'PosiciÃ³n calculada por C++',
          position: positionMeters,

          point: {
            pixelSize: 14,
            color: Color.MAGENTA,
            outlineColor: Color.WHITE,
            outlineWidth: 2,
          },

          label: {
            text: 'C++ Â· t=1417s',
            font: '14px Arial',
            fillColor: Color.WHITE,
            pixelOffset: new Cartesian2(0, -25),
          },
        })

      } catch (error) {
        console.error('Error conectando con C++:', error)
      }
    }

    void testCppConnection()*/

    // Permitir acercarse mÃ¡s a la superficie y al satÃ©lite
    viewer.scene.screenSpaceCameraController.minimumZoomDistance = 10
    viewer.scene.screenSpaceCameraController.maximumZoomDistance = 100000000

    // Cargar textura terrestre
    async function loadEarth() {
      // Textura de respaldo: sabemos que funciona
      const fallback =
        await TileMapServiceImageryProvider.fromUrl(
          buildModuleUrl('Assets/Textures/NaturalEarthII')
        )

      if (viewer.isDestroyed()) return

      const fallbackLayer =
        viewer.imageryLayers.addImageryProvider(fallback)

      try {
        // Intentar cargar las imÃ¡genes de Bing Maps
        const bing = await IonImageryProvider.fromAssetId(2)

        if (viewer.isDestroyed()) return

        viewer.imageryLayers.addImageryProvider(bing)

        // Retirar la textura antigua cuando Bing estÃ© disponible
        viewer.imageryLayers.remove(fallbackLayer)
      } catch (error) {
        console.error('Error cargando Bing Maps:', error)
      }
    }

    loadEarth().catch(console.error)


    // ========================================
    // 2. TIEMPO DE SIMULACIÃ“N
    // ========================================
    const startTime = JulianDate.clone(
      viewer.clock.currentTime
    )

    viewer.clock.multiplier = 100
    viewer.clock.shouldAnimate = true

    // Actualizar el tiempo de misiÃ³n en el panel
    let lastUpdate = 0

    const updateMissionTime = () => {
      const now = Date.now()

      // Actualizamos la interfaz cada 250 ms
      if (now - lastUpdate < 250) return

      lastUpdate = now

      const elapsed = JulianDate.secondsDifference(
        viewer.clock.currentTime,
        startTime
      )

      setMissionSeconds(Math.max(0, Math.floor(elapsed)))
    }

    viewer.clock.onTick.addEventListener(updateMissionTime)

    // La posiciÃ³n del satÃ©lite sigue calculÃ¡ndose
    // con el tiempo de Cesium
    /*const satellitePosition = new CallbackPositionProperty(
      (time, result) => {
        const elapsedSeconds = JulianDate.secondsDifference(
          time ?? startTime,
          startTime
        )

        const angle =
          (2 * Math.PI * elapsedSeconds) / periodSeconds

        const x = radiusKm * 1000 * Math.cos(angle)
        const y = radiusKm * 1000 * Math.sin(angle)

        return Cartesian3.fromElements(x, y, 0, result)
      },
      false
    )*/


    // ========================================
    // 3. MODELO 3D DEL CUBESAT
    // ========================================

    const satellite = viewer.entities.add({
      name: 'CubeSat-01',

      model: {
        uri: '/models/cubesat-01.glb',
        scale: 1,
        minimumPixelSize: 100,
        maximumScale: 1000000000,
      },

      label: {
        text: 'CubeSat-01',
        font: '15px Arial',
        fillColor: Color.WHITE,
        outlineColor: Color.BLACK,
        outlineWidth: 2,
        pixelOffset: new Cartesian2(0, -45),
      },
    })

    satelliteRef.current = satellite

    let disposed = false
    let removeOrbitUpdater = () => {}


    async function animateFromCpp() {
      const sampledPosition = new SampledPositionProperty()

      sampledPosition.setInterpolationOptions({
        interpolationDegree: 1,
        interpolationAlgorithm: LinearApproximation,
      })

      const STEP = 60
      const COUNT = 100

      let lastSampleTime = -STEP
      let loading = false

      async function loadMoreSamples(start: number) {
        if (loading || disposed) return

        loading = true

        try {
          const response = await fetch(
            `/api/orbit/samples?start=${start}&step=${STEP}&count=${COUNT}`
          )

          if (!response.ok) {
            throw new Error(`HTTP ${response.status}`)
          }

          const samples: {
            time: number
            x: number
            y: number
            z: number
          }[] = await response.json()

          if (disposed || viewer.isDestroyed()) return

          for (const sample of samples) {
            const time = JulianDate.addSeconds(
              startTime,
              sample.time,
              new JulianDate()
            )

            sampledPosition.addSample(
              time,
              new Cartesian3(
                sample.x * 1000,
                sample.y * 1000,
                sample.z * 1000
              )
            )
          }


          if (start === 0) {
            const orbitPositions = samples.map(
              (sample) =>
                new Cartesian3(
                  sample.x * 1000,
                  sample.y * 1000,
                  sample.z * 1000
                )
            )

            // Cerrar el dibujo de la Ã³rbita circular
            orbitPositions.push(orbitPositions[0].clone())

            viewer.entities.add({
              name: 'Órbita de CubeSat-01',
              polyline: {
                positions: orbitPositions,
                width: 2,
                material: Color.CYAN,
                arcType: ArcType.NONE,
              },
            })
          }


          lastSampleTime = samples[samples.length - 1].time

          console.log(
            `C++: posiciones disponibles hasta ${lastSampleTime} s`
          )
        } catch (error) {
          console.error('Error cargando muestras C++:', error)
        } finally {
          loading = false
        }
      }

      // Primera carga
      await loadMoreSamples(0)

      if (disposed || viewer.isDestroyed()) return
      if (lastSampleTime < 0) return

      // El CubeSat utiliza las posiciones calculadas por C++
      satellite.position = sampledPosition

      const checkOrbitSamples = () => {
        if (disposed || loading) return

        const elapsed = JulianDate.secondsDifference(
          viewer.clock.currentTime,
          startTime
        )

        // Recargar cuando falten 1200 segundos simulados
        if (lastSampleTime - elapsed < 1200) {
          void loadMoreSamples(lastSampleTime + STEP)
        }
      }

      viewer.clock.onTick.addEventListener(checkOrbitSamples)

      removeOrbitUpdater = () => {
        viewer.clock.onTick.removeEventListener(checkOrbitSamples)
      }

      console.log('CubeSat: Ã³rbita continua desde C++ activada')
    }

    void animateFromCpp()



    // ========================================
    // 4. CÃMARA MÃS ALEJADA
    // ========================================

    const cameraPosition = new Cartesian3(
      0,
      -25000000,
      18000000
    )

    viewer.camera.setView({
      destination: cameraPosition,
      orientation: {
        direction: Cartesian3.normalize(
          Cartesian3.negate(
            cameraPosition,
            new Cartesian3()
          ),
          new Cartesian3()
        ),
        up: Cartesian3.UNIT_Z,
      },
    })

    return () => {
      viewer.clock.onTick.removeEventListener(updateMissionTime)

      viewerRef.current = null
      satelliteRef.current = null

      disposed = true
      removeOrbitUpdater()

      if (!viewer.isDestroyed()) {
        viewer.destroy()
      }
    }
  }, [])

  return (
    <div className="app">
      <header className="header">
        <h1>CubeSat-01</h1>
        <span>MISSION SIMULATOR</span>
        <span className="status">
          SIMULATION READY
        </span>
      </header>

      <main className="viewport">
        <div ref={containerRef} className="earth" />

        <div className="overlay">
          <h2>EARTH ORBITAL VIEW</h2>
          <p>LEO · 500 km · Circular orbit</p>
          <p>Velocidad: {speedKmS.toFixed(2)} km/s</p>
          <p>Periodo: {(periodSeconds / 60).toFixed(1)} min</p>
          <p>Tiempo de misión: {missionSeconds} s</p>
          <p>Velocidad: x{simulationSpeed}</p>

          <div className="simulation-controls">
            <button onClick={toggleSimulation}>
              {isPlaying ? 'Pausar' : 'Reanudar'}
            </button>

            <div className="speed-controls">
              {[1, 10, 100].map((speed) => (
                <button
                  key={speed}
                  className={simulationSpeed === speed ? 'active' : ''}
                  onClick={() => changeSimulationSpeed(speed)}
                >
                  x{speed}
                </button>
              ))}
            </div>
          </div>
          <div className="camera-controls">
            <button onClick={showGlobalView}>
              Vista global
            </button>

            <button onClick={followSatellite}>
              Seguir CubeSat
            </button>
          </div>
        </div>
      </main>
    </div>
  )
}

export default App
