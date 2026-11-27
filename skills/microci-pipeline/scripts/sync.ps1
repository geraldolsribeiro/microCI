# sync.ps1 — Refresh the microCI skill docs/templates cache from GitHub.
# Usage: powershell -ExecutionPolicy Bypass -File scripts/sync.ps1 [-CacheDir <path>] [-Branch <name>]
# Downloads the repository archive ONCE and extracts only the needed files (fast, 2 requests total).
# Exit code != 0 means the cache must NOT be trusted.
[CmdletBinding()]
param(
    [string]$CacheDir = (Join-Path $env:LOCALAPPDATA 'microci-skill'),
    [string]$Owner    = 'geraldolsribeiro',
    [string]$Repo     = 'microCI',
    [string]$Branch   = 'master'
)

$ErrorActionPreference = 'Stop'
try {
    $sw        = [Diagnostics.Stopwatch]::StartNew()
    $apiBase   = "https://api.github.com/repos/$Owner/$Repo"
    $docsDir   = Join-Path $CacheDir 'docs'
    $tmplDir   = Join-Path $CacheDir 'templates'

    # Head commit (best effort; failure here is not fatal)
    $commit = 'unknown'
    try {
        $head = Invoke-RestMethod -Uri "$apiBase/commits/$Branch" -Headers @{ 'User-Agent' = 'microci-skill-sync' }
        $commit = $head.sha
    } catch { Write-Warning "Could not resolve head commit: $_" }

    # Single archive download to a temp file.
    # NOTE: use .zip, not .tar.gz — some Windows setups have a unix tar.exe first in PATH
    # that cannot decompress gzip; bsdtar's zip support is native everywhere.
    $archiveUrl= "https://codeload.github.com/$Owner/$Repo/zip/refs/heads/$Branch"
    $tmpZip    = Join-Path ([IO.Path]::GetTempPath()) ("microci-skill-{0}.zip" -f [Guid]::NewGuid())
    try {
        Invoke-WebRequest -Uri $archiveUrl -OutFile $tmpZip -Headers @{ 'User-Agent' = 'microci-skill-sync' } -UseBasicParsing

        New-Item -ItemType Directory -Force -Path $docsDir, $tmplDir | Out-Null

        Add-Type -AssemblyName System.IO.Compression.FileSystem
        $zip  = [IO.Compression.ZipFile]::OpenRead($tmpZip)
        try {
            $prefixDocs = "$Repo-$Branch/docs/"
            $prefixNew  = "$Repo-$Branch/new/"

            $docsCount = 0
            foreach ($entry in $zip.Entries | Where-Object { $_.FullName -like "$prefixDocs*" }) {
                $name = [IO.Path]::GetFileName($entry.FullName)
                if (-not $name -or $name -notmatch '^\d{3}_.*\.md$') { continue }
                $dest = Join-Path $docsDir $name
                if (Test-Path -LiteralPath $dest) { Remove-Item -LiteralPath $dest -Force }
                [IO.Compression.ZipFileExtensions]::ExtractToFile($entry, $dest, $true)
                $docsCount++
            }

            $tmplCount = 0
            foreach ($entry in $zip.Entries | Where-Object { $_.FullName -like "$prefixNew*" }) {
                $name = [IO.Path]::GetFileName($entry.FullName)
                if (-not $name -or $name -notmatch '\.yml$') { continue }
                $dest = Join-Path $tmplDir $name
                if (Test-Path -LiteralPath $dest) { Remove-Item -LiteralPath $dest -Force }
                [IO.Compression.ZipFileExtensions]::ExtractToFile($entry, $dest, $true)
                $tmplCount++
            }
        } finally {
            $zip.Dispose()
        }
    } finally {
        Remove-Item -Force $tmpZip -ErrorAction SilentlyContinue
    }

    # Sanity lower bounds + failure check (upstream grows over time)
    if ($docsCount -lt 30 -or $tmplCount -lt 25) { throw "Sanity check failed: docs=$docsCount templates=$tmplCount" }

    $meta = @(
        "source=https://github.com/$Owner/$Repo"
        "branch=$Branch"
        "commit=$commit"
        "synced_at=$([DateTime]::UtcNow.ToString('yyyy-MM-ddTHH:mm:ssZ'))"
        "docs_count=$docsCount"
        "templates_count=$tmplCount"
    ) -join [Environment]::NewLine
    New-Item -ItemType Directory -Force -Path $CacheDir | Out-Null
    Set-Content -Path (Join-Path $CacheDir '.sync_meta') -Value $meta -Encoding UTF8

    Write-Host ("microci-skill sync OK: {0} docs, {1} templates in {2:N1}s -> {3} (commit {4})" -f `
        $docsCount, $tmplCount, $sw.Elapsed.TotalSeconds, $CacheDir, $commit)
} catch {
    Write-Error "microci-skill sync FAILED: $_"
    exit 1
}

