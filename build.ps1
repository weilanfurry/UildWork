<#
    UildWork build script (no CMake required).

    Usage:
        .\build.ps1                        # build all executables into .\build\bin
        .\build.ps1 -Clean                 # remove intermediate files first
        .\build.ps1 -DeployDir "D:\Uild\UildWork\ZERO\app"
                                           # build, then copy binaries to the packaging folder

    Requirements: MinGW-w64 / TDM-GCC (g++ + windres).
    The script looks for Code::Blocks' bundled MinGW first, then PATH.
#>
[CmdletBinding()]
param(
    [string]$OutDir = '',
    [string]$DeployDir = '',
    [switch]$Clean
)

$ErrorActionPreference = 'Stop'

if (-not $OutDir) {
    $OutDir = Join-Path $PSScriptRoot 'build\bin'
}

function Write-Step($message) {
    Write-Host "==> $message" -ForegroundColor Cyan
}

function Find-Tool($name, $extraPaths) {
    foreach ($candidate in $extraPaths) {
        if ($candidate -and (Test-Path -LiteralPath $candidate)) {
            return (Resolve-Path -LiteralPath $candidate).Path
        }
    }
    $fromPath = Get-Command $name -ErrorAction SilentlyContinue
    if ($fromPath) { return $fromPath.Source }
    return $null
}

$toolCandidates = @()
if (${env:ProgramFiles(x86)}) {
    $toolCandidates += (Join-Path ${env:ProgramFiles(x86)} 'CodeBlocks\MinGW\bin')
}
if ($env:ProgramFiles) {
    $toolCandidates += (Join-Path $env:ProgramFiles 'CodeBlocks\MinGW\bin')
}
$toolCandidates += @('C:\mingw64\bin', 'C:\msys64\mingw64\bin', 'C:\MinGW\bin')

$gpp = Find-Tool 'g++' ($toolCandidates | ForEach-Object { Join-Path $_ 'g++.exe' })
$windres = Find-Tool 'windres' ($toolCandidates | ForEach-Object { Join-Path $_ 'windres.exe' })

if (-not $gpp) {
    throw 'g++ not found. Install MinGW-w64/TDM-GCC, or add it to PATH.'
}

Write-Step "Compiler: $gpp"
if ($windres) { Write-Step "Resource compiler: $windres" } else { Write-Host 'warn: windres not found, executable icon will be skipped' -ForegroundColor Yellow }

# The TDM-GCC layout puts as.exe/ld.exe in <mingw>\mingw32\bin, while libiconv-2.dll /
# libintl-8.dll live in <mingw>\bin. Windows resolves those DLLs through PATH, so the
# toolchain's bin directory must be on PATH (this is what Code::Blocks does automatically).
# Without it the assembler aborts with "libintl-8.dll not found" and g++ exits with code 1
# while printing nothing.
$toolBin = Split-Path -Parent $gpp
if ($env:PATH -notlike "*$toolBin*") {
    $env:PATH = "$toolBin;$env:PATH"
    Write-Step "Added toolchain to PATH: $toolBin"
}

$root = $PSScriptRoot
$objDir = Join-Path $root 'build\obj'

if ($Clean -and (Test-Path -LiteralPath $objDir)) {
    Write-Step "Cleaning $objDir"
    Remove-Item -LiteralPath $objDir -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $objDir | Out-Null
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

$compileFlags = @(
    '-std=gnu++11',
    '-O2',
    '-Wall',
    '-Wextra',
    '-Wno-unused-parameter',
    '-finput-charset=UTF-8',
    '-fexec-charset=UTF-8',
    "-I$root\src"
)
$linkFlags = @(
    '-static',
    '-static-libgcc',
    '-static-libstdc++',
    '-lwinmm',
    '-lshell32',
    '-lole32'
)

$commonSources = Get-ChildItem -LiteralPath (Join-Path $root 'src\common') -Filter '*.cpp' | Sort-Object Name
$appSources = Get-ChildItem -LiteralPath (Join-Path $root 'src\apps') -Filter '*.cpp' | Sort-Object Name
$sharedSources = @($commonSources) + @($appSources)

# ---- compile shared objects -------------------------------------------------
$sharedObjects = @()
foreach ($source in $sharedSources) {
    $object = Join-Path $objDir ($source.BaseName + '.o')
    Write-Step "Compiling $($source.Name)"
    & $gpp @compileFlags '-c' $source.FullName '-o' $object
    if ($LASTEXITCODE -ne 0) { throw "Failed to compile $($source.Name)" }
    $sharedObjects += $object
}

# ---- icon resource ----------------------------------------------------------
$resourceObject = $null
if ($windres) {
    $resourceSource = Join-Path $root 'resources\app.rc'
    if (Test-Path -LiteralPath $resourceSource) {
        $resourceObject = Join-Path $objDir 'app.res.o'
        Write-Step 'Compiling icon resource'
        # windres builds the preprocessor command line itself and does not quote the
        # compiler path, so a toolchain under "Program Files (x86)" breaks it. Feeding it
        # the 8.3 short path keeps that command line space-free.
        $shortGcc = $gpp
        try {
            $fso = New-Object -ComObject Scripting.FileSystemObject
            $gccCandidate = Join-Path (Split-Path -Parent $gpp) 'gcc.exe'
            if (Test-Path -LiteralPath $gccCandidate) {
                $shortGcc = $fso.GetFile($gccCandidate).ShortPath
            }
            $shortResourceSource = $fso.GetFile($resourceSource).ShortPath
            $shortResourceObject = Join-Path $objDir 'app.res.o'
        } catch {
            Write-Host "warn: could not resolve short paths ($($_.Exception.Message))" -ForegroundColor Yellow
            $shortResourceSource = $resourceSource
        }
        Push-Location $root
        & $windres "--preprocessor=$shortGcc -E -xc -DRC_INVOKED" "$shortResourceSource" '-o' "$shortResourceObject"
        $resourceExit = $LASTEXITCODE
        Pop-Location
        if ($resourceExit -ne 0) {
            Write-Host 'warn: windres failed, continuing without icon' -ForegroundColor Yellow
            $resourceObject = $null
        }
    }
}

# ---- link targets -----------------------------------------------------------
function Build-Target($sourceFile, $targetName, [switch]$WithIcon) {
    $object = Join-Path $objDir ($targetName + '.main.o')
    Write-Step "Building $targetName"
    & $gpp @compileFlags '-c' $sourceFile '-o' $object
    if ($LASTEXITCODE -ne 0) { throw "Failed to compile $sourceFile" }
    $output = Join-Path $OutDir $targetName
    $inputs = @($object) + $sharedObjects
    if ($WithIcon -and $resourceObject) { $inputs += $resourceObject }
    & $gpp @inputs @linkFlags '-o' $output
    if ($LASTEXITCODE -ne 0) { throw "Failed to link $targetName" }
}

Build-Target (Join-Path $root 'src\main.cpp') 'UildWork.exe' -WithIcon

$legacyTargets = @(
    @{ Source = 'Statistics.cpp';     Name = 'Statistics.exe' },
    @{ Source = 'Manifest.cpp';       Name = 'Manifest.exe' },
    @{ Source = 'Stopwatch.cpp';      Name = 'Stopwatch.exe' },
    @{ Source = 'shishishizhong.cpp'; Name = 'shishishizhong.exe' },
    @{ Source = 'zhengjishi.cpp';     Name = 'zhengjishi.exe' },
    @{ Source = 'daojishi.cpp';       Name = 'daojishi.exe' },
    @{ Source = 'naozhong.cpp';       Name = 'naozhong.exe' },
    @{ Source = 'fanqiezhong.cpp';    Name = 'fanqiezhong.exe' }
)
foreach ($target in $legacyTargets) {
    Build-Target (Join-Path $root ('src\legacy\' + $target.Source)) $target.Name
}

if ($DeployDir) {
    Write-Step "Deploying to $DeployDir"
    New-Item -ItemType Directory -Force -Path $DeployDir | Out-Null
    Copy-Item -Path (Join-Path $OutDir '*') -Destination $DeployDir -Force
    $assetDir = Join-Path $DeployDir 'assets'
    New-Item -ItemType Directory -Force -Path $assetDir | Out-Null
    Copy-Item -Path (Join-Path $root 'assets\*') -Destination $assetDir -Force
}

Write-Step 'Done'
Get-ChildItem -LiteralPath $OutDir -Filter '*.exe' | Sort-Object Name |
    Select-Object Name, @{ Name = 'SizeKB'; Expression = { [math]::Round($_.Length / 1KB) } } |
    Format-Table -AutoSize | Out-String -Width 120 | Write-Host
