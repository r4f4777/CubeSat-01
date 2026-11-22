import { useEffect, useRef, useState } from 'react'
import type { Entity } from 'cesium'
import {
  Viewer,
  Cartesian3,
  Cartesian2,
  Color,
  ArcType,
  JulianDate,
  CallbackPositionProperty,
  Ion,
  IonImageryProvider,
  TileMapServiceImageryProvider,
  buildModuleUrl,
} from 'cesium'

import 'cesium/Build/Cesium/Widgets/widgets.css'
import './App.css'



// Parámetros de nuestra órbita circular
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


    // Dejar de seguir al satélite
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

    // Distancia y orientación inicial de la cámara
    // respecto al CubeSat (metros, sistema local).
    satellite.viewFrom = new Cartesian3(
      -1500,
      -3500,
      1800
    )

    // Activar seguimiento automático
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

    // Permitir acercarse más a la superficie y al satélite
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
        // Intentar cargar las imágenes de Bing Maps
        const bing = await IonImageryProvider.fromAssetId(2)

        if (viewer.isDestroyed()) return

        viewer.imageryLayers.addImageryProvider(bing)

        // Retirar la textura antigua cuando Bing esté disponible
        viewer.imageryLayers.remove(fallbackLayer)
      } catch (error) {
        console.error('Error cargando Bing Maps:', error)
      }
    }

    loadEarth().catch(console.error)


    // ========================================
    // 1. TRAYECTORIA ORBITAL
    // ========================================

    const orbitPositions: Cartesian3[] = []

    for (let i = 0; i <= 360; i++) {
      const angle = (i * Math.PI) / 180

      const x = radiusKm * 1000 * Math.cos(angle)
      const y = radiusKm * 1000 * Math.sin(angle)

      orbitPositions.push(new Cartesian3(x, y, 0))
    }

    viewer.entities.add({
      name: 'Órbita de CubeSat-01',
      polyline: {
        positions: orbitPositions,
        width: 2,
        material: Color.CYAN,
        arcType: ArcType.NONE,
      },
    })

    // ========================================
    // 2. TIEMPO DE SIMULACIÓN
    // ========================================

    
    const startTime = JulianDate.clone(
      viewer.clock.currentTime
    )

    viewer.clock.multiplier = 100
    viewer.clock.shouldAnimate = true

    // Actualizar el tiempo de misión en el panel
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

    // La posición del satélite sigue calculándose
    // con el tiempo de Cesium
    const satellitePosition = new CallbackPositionProperty(
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
    )


    // ========================================
    // 3. MODELO 3D DEL CUBESAT
    // ========================================

    const satellite = viewer.entities.add({
      name: 'CubeSat-01',
      position: satellitePosition,

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

    // ========================================
    // 4. CÁMARA MÁS ALEJADA
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

      if (!viewer.isDestroyed()) {
        viewer.destroy()
      }
    }
  }, [])

  return (
    <div className="app">
      <header className="header">
        <h1>🛰 CubeSat-01</h1>
        <span>MISSION SIMULATOR</span>
        <span className="status">
          ● SIMULATION READY
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
              {isPlaying ? '⏸ Pausar' : '▶ Reanudar'}
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
