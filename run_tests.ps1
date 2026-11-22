
$ErrorActionPreference = "Stop"

$tests = @(
    "build/test_power.exe",
    "build/test_sensor_v2.exe",
    "build/test_camera.exe",
    "build/test_mission.exe",
    "build/test_controller.exe"
)

foreach ($test in $tests) {
    Write-Host "`nRunning $test..."

    & ".\$test"

    if ($LASTEXITCODE -ne 0) {
        Write-Host "FAILED: $test"
        exit 1
    }
}

Write-Host "`nALL TESTS PASSED"
