
$ErrorActionPreference = "Stop"

# Crear build/ si no existe
New-Item -ItemType Directory -Path "build" -Force | Out-Null

# Definir los tests y sus archivos fuente
$tests = @(
    @{
        Name = "test_power"
        Sources = @("tests/test_power.cpp", "src/PowerManager.cpp")
    },
    @{
        Name = "test_sensor"
        Sources = @("tests/test_sensor.cpp", "src/SensorValidator.cpp")
    },
    @{
        Name = "test_camera"
        Sources = @("tests/test_camera.cpp", "src/CameraManager.cpp")
    },
    @{
        Name = "test_mission"
        Sources = @("tests/test_mission.cpp", "src/MissionManager.cpp")
    },
    @{
        Name = "test_controller"
        Sources = @(
            "tests/test_controller.cpp",
            "src/PowerManager.cpp",
            "src/SensorValidator.cpp",
            "src/CameraManager.cpp",
            "src/MissionManager.cpp",
            "src/MissionClock.cpp",
            "src/SatelliteController.cpp"
        )
    },
    @{
        Name = "test_clock"
        Sources = @("tests/test_clock.cpp", "src/MissionClock.cpp")
    },
    @{
        Name = "test_orbit"
        Sources = @("tests/test_orbit.cpp", "src/OrbitSimulator.cpp")
    }
)

foreach ($test in $tests) {
    $exe = "build/$($test.Name).exe"

    Write-Host "`nCompiling $($test.Name)..."

    & g++ -std=c++17 -Iinclude @($test.Sources) -o $exe

    if ($LASTEXITCODE -ne 0) {
        Write-Host "COMPILATION FAILED: $($test.Name)"
        exit 1
    }

    Write-Host "Running $($test.Name)..."

    & "./$exe"

    if ($LASTEXITCODE -ne 0) {
        Write-Host "TEST FAILED: $($test.Name)"
        exit 1
    }
}

Write-Host "`nALL TESTS PASSED"
