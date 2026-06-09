$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$outputPath = Join-Path $scriptDir "hashes.txt"

$files = Get-ChildItem -Path $scriptDir -File | Where-Object {
    $_.Name -notin @("collect_hashes.ps1", "hashes.txt")
}

$lines = @()
$lines += "# TODO-077 hashes"
$lines += ""

foreach ($file in $files | Sort-Object Name) {
    $hash = (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash
    $lines += "$($file.Name)`t$hash"
}

Set-Content -LiteralPath $outputPath -Value $lines -Encoding UTF8
Write-Output "Written: $outputPath"
