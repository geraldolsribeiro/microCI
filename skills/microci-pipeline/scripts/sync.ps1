# sync.ps1 — Refresh the microCI skill docs/templates cache from GitHub.
# Usage: powershell -ExecutionPolicy Bypass -File sync.ps1 [-CacheDir <path>] [-Branch <name>]
# Exit code != 0 means the cache must not be trusted.
[CmdletBinding()]
param(
    [string]$CacheDir = (Join-Path $env:LOCALAPPDATA 'microci-skill'),
    [string]$Owner    = 'geraldolsribeiro',
    [string]$Repo     = 'microCI',
    [string]$Branch   = 'master'
)

$ErrorActionPreference = 'Stop'
try {
    $apiBase  = "https://api.github.com/repos/$Owner/$Repo"
    $rawBase  = "https://raw.githubusercontent.com/$Owner/$Repo/$Branch"
    $docsDir  = Join-Path $CacheDir 'docs'
    $tmplDir  = Join-Path $CacheDir 'templates'
    New-Item -ItemType Directory -Force -Path $docsDir, $tmplDir | Out-Null

    # Head commit (best effort; failure here is not fatal)
    $commit = 'unknown'
    try {
        $head = Invoke-RestMethod -Uri "$apiBase/commits/$Branch" -Headers @{ 'User-Agent' = 'microci-skill-sync' }
        $commit = $head.sha
    } catch { Write-Warning "Could not resolve head commit: $_" }

    # Listings
    $docsItems = (Invoke-RestMethod -Uri "$apiBase/contents/docs?ref=$Branch" -Headers @{ 'User-Agent' = 'microci-skill-sync' }) |
        Where-Object { $_.type -eq 'file' -and $_.name -match '^\d{3}_.*\.md$' }
    $tmplItems = (Invoke-RestMethod -Uri "$apiBase/contents/new?ref=$Branch" -Headers @{ 'User-Agent' = 'microci-skill-sync' }) |
        Where-Object { $_.type -eq 'file' -and $_.name -match '\.yml$' }

    if (-not $docsItems -or -not $tmplItems) { throw 'Empty listing from GitHub contents API.' }

    $docsCount = 0
    foreach ($item in $docsItems) {
        Invoke-WebRequest -Uri "$rawBase/docs/$($item.name)" -OutFile (Join-Path $docsDir $item.name) `
            -Headers @{ 'User-Agent' = 'microci-skill-sync' } -UseBasicParsing
        $docsCount++
    }
    $tmplCount = 0
    foreach ($item in $tmplItems) {
        Invoke-WebRequest -Uri "$rawBase/new/$($item.name)" -OutFile (Join-Path $tmplDir $item.name) `
            -Headers @{ 'User-Agent' = 'microci-skill-sync' } -UseBasicParsing
        $tmplCount++
    }

    # Sanity lower bounds (upstream grows over time)
    if ($docsCount -lt 30 -or $tmplCount -lt 25) { throw "Sanity check failed: docs=$docsCount templates=$tmplCount" }

    $meta = @(
        "source=https://github.com/$Owner/$Repo"
        "branch=$Branch"
        "commit=$commit"
        "synced_at=$([DateTime]::UtcNow.ToString('yyyy-MM-ddTHH:mm:ssZ'))"
        "docs_count=$docsCount"
        "templates_count=$tmplCount"
    ) -join [Environment]::NewLine
    Set-Content -Path (Join-Path $CacheDir '.sync_meta') -Value $meta -Encoding UTF8

    Write-Host "microci-skill sync OK: $docsCount docs, $tmplCount templates -> $CacheDir (commit $commit)"
} catch {
    Write-Error "microci-skill sync FAILED: $_"
    exit 1
}
