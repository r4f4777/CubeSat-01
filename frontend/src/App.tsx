
import { useEffect, useRef } from 'react'
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

    const satellitePosition = new CallbackPositionProperty(
      (time, result) => {
        const elapsedSeconds =
            JulianDate.secondsDifference(time ?? startTime, startTime)

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

    viewer.entities.add({
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
      viewer.destroy()
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
          <p>Simulación: x100</p>
        </div>
      </main>
    </div>
  )
}

export default App
