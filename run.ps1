param(
    [string]$mode = "debug"
)

$BUILD_DIR = "build"
$EXE_NAME = "code_machine_grader.exe"

switch ($mode.ToLower()) {
    "clean" {
        if (Test-Path $BUILD_DIR) {
            Write-Host "Cleaning build directory..."
            Remove-Item -Recurse -Force $BUILD_DIR
        } else {
            Write-Host "Build directory does not exist."
        }
        break
    }

    "debug" {
        if (!(Test-Path $BUILD_DIR)) {
            New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
        }
        Write-Host "Building in Debug mode..."
        cmake -B $BUILD_DIR -S . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        cmake --build $BUILD_DIR
        break
    }

    "release" {
        if (!(Test-Path $BUILD_DIR)) {
            New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
        }
        Write-Host "Building in Release mode..."
        cmake -B $BUILD_DIR -S . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        cmake --build $BUILD_DIR
        break
    }

    "run" {
        if (!(Test-Path $BUILD_DIR)) {
            New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
        }
        Write-Host "Building in Debug mode..."
        cmake -B $BUILD_DIR -S . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

        $buildResult = cmake --build $BUILD_DIR
        if ($LASTEXITCODE -ne 0) {
            Write-Host "Build failed. Aborting execution."
            exit $LASTEXITCODE
        }

        $exePath = Join-Path $BUILD_DIR $EXE_NAME
        if (Test-Path $exePath) {
            Write-Host "Running $EXE_NAME..."
            & $exePath
        } else {
            Write-Host "Error: Executable not found!"
        }
        break
    }

    default {
        Write-Host "Unknown mode '$mode'. Use 'debug', 'release', 'clean', or 'run'."
    }
}
