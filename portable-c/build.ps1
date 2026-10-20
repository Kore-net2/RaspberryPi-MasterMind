$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot
if (Get-Command gcc -ErrorAction SilentlyContinue) {
    & gcc -std=c11 -Wall -Wextra -Werror -pedantic -O2 matcher.c test_matcher.c -o test_matcher.exe
    if ($LASTEXITCODE -ne 0) { throw 'GCC test build failed' }
    & ./test_matcher.exe
    if ($LASTEXITCODE -ne 0) { throw 'Tests failed' }
    & gcc -std=c11 -Wall -Wextra -Werror -pedantic -O2 matcher.c main.c -o mastermind.exe
    if ($LASTEXITCODE -ne 0) { throw 'GCC game build failed' }
} elseif (Get-Command cl -ErrorAction SilentlyContinue) {
    & cl /nologo /W4 /TC matcher.c test_matcher.c /Fe:test_matcher.exe
    if ($LASTEXITCODE -ne 0) { throw 'MSVC test build failed' }
    & ./test_matcher.exe
    if ($LASTEXITCODE -ne 0) { throw 'Tests failed' }
    & cl /nologo /W4 /TC matcher.c main.c /Fe:mastermind.exe
    if ($LASTEXITCODE -ne 0) { throw 'MSVC game build failed' }
} else {
    throw 'No C compiler found. Use the browser demo in ../desktop/index.html, or install GCC/MSVC.'
}
Write-Host 'Build successful. Play: .\mastermind.exe --secret 123'
