$projectPath = Split-Path -Parent $MyInvocation.MyCommand.Path
$wslProjectPath = wsl.exe -d Ubuntu -- wslpath -a ($projectPath -replace "\\", "/")

if (-not $wslProjectPath) {
    Write-Error "Не удалось определить путь WSL."
    exit 1
}

wsl.exe -d Ubuntu -- bash -lc "pkill -f '$wslProjectPath/build/CatalogService' || true"
Write-Host "Каталожный backend остановлен."

