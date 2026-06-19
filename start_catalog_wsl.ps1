$projectPath = Split-Path -Parent $MyInvocation.MyCommand.Path
$wslProjectPath = (wsl.exe -d Ubuntu -- wslpath -a ($projectPath -replace "\\", "/")).Trim()

if (-not $wslProjectPath) {
    Write-Error "Не удалось определить путь WSL."
    exit 1
}

wsl.exe -d Ubuntu -- bash -lc "pkill -f 'CatalogService' || true"
wsl.exe -d Ubuntu -- bash -lc "cd '$wslProjectPath' && setsid -f ./build/CatalogService >/tmp/unitedlibra-catalog.log 2>&1"

Start-Sleep -Seconds 5

try {
    $null = Invoke-RestMethod "http://127.0.0.1:8001/books/get-authors"
    Write-Host "Каталожный backend запущен: http://127.0.0.1:8001"
}
catch {
    Write-Error "Каталожный backend не ответил на http://127.0.0.1:8001/books/get-authors"
    Write-Host "Лог WSL: /tmp/unitedlibra-catalog.log"
    exit 1
}
