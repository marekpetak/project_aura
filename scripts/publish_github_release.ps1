param(
  [Parameter(Mandatory = $true)]
  [string]$Version,
  [string]$BuildId,
  [string]$Repo = "21cncstudio/project_aura",
  [string]$Tag,
  [string]$Title,
  [string]$AssetsDir,
  [string]$NotesPath,
  [string]$TargetCommitish = "main",
  [int]$ApiTimeoutSec = 60,
  [int]$UploadTimeoutSec = 300,
  [int]$ConnectTimeoutSec = 15,
  [switch]$Draft,
  [switch]$SkipNotes,
  [switch]$ForceLegacyApi,
  [switch]$SkipReleaseUpdate,
  [switch]$PruneAssetsToList,
  [string[]]$AssetNames
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$script:LastGhExitCode = 0

function Write-Step {
  param([string]$Message)
  Write-Host ("[{0}] {1}" -f (Get-Date -Format "HH:mm:ss"), $Message)
}

function Resolve-BuildId {
  param([string]$Root)

  if ($BuildId) {
    return $BuildId
  }

  $git = Get-Command git -ErrorAction SilentlyContinue
  if (-not $git) {
    return $null
  }

  Push-Location $Root
  try {
    $sha = (& git rev-parse --short=7 HEAD 2>$null).Trim()
    if ([string]::IsNullOrWhiteSpace($sha)) {
      return $null
    }
    return $sha
  } finally {
    Pop-Location
  }
}

function Test-IsStableVersion {
  param([string]$Value)

  return -not [string]::IsNullOrWhiteSpace($Value) -and
         $Value -match '^\d+(?:\.\d+)+$'
}

if (-not $Tag) {
  $Tag = "v$Version"
}
if (-not $Title) {
  $Title = "Project Aura v$Version"
}

$root = Resolve-Path (Join-Path $PSScriptRoot "..")
$resolvedBuildId = Resolve-BuildId -Root $root
$displayVersion = $Version
if ($resolvedBuildId -and -not (Test-IsStableVersion -Value $Version)) {
  $displayVersion = "{0}-{1}" -f $Version, $resolvedBuildId
}
if (-not $AssetsDir) {
  $AssetsDir = Join-Path $root ("release-assets\{0}" -f $Tag)
}
if (-not $NotesPath) {
  $NotesPath = Join-Path $root ("docs\releases\v{0}.md" -f $Version)
}

if (-not (Test-Path $AssetsDir)) {
  throw "Assets directory not found: $AssetsDir"
}

if (-not $AssetNames -or $AssetNames.Count -eq 0) {
  $AssetNames = @(
    ("project_aura_{0}_ota_firmware.bin" -f $displayVersion)
  )
}

foreach ($name in $AssetNames) {
  $path = Join-Path $AssetsDir $name
  if (-not (Test-Path $path)) {
    throw "Missing asset file: $path"
  }
}

if (-not (Get-Command curl.exe -ErrorAction SilentlyContinue)) {
  throw "curl.exe is required but not found."
}
if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
  throw "git is required but not found."
}

function Get-GitHubCredentials {
  $request = "protocol=https`nhost=github.com`nusername=21cncstudio`n`n"
  $oldGcmInteractive = $env:GCM_INTERACTIVE
  try {
    $env:GCM_INTERACTIVE = "never"
    $raw = $request | git credential fill 2>$null
  } finally {
    if ($null -eq $oldGcmInteractive) {
      Remove-Item Env:\GCM_INTERACTIVE -ErrorAction SilentlyContinue
    } else {
      $env:GCM_INTERACTIVE = $oldGcmInteractive
    }
  }
  if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace($raw)) {
    throw "Unable to read GitHub credentials from git credential helper."
  }

  $map = @{}
  foreach ($line in ($raw -split "`n")) {
    if ($line -match "=") {
      $k, $v = $line.Split("=", 2)
      $map[$k.Trim()] = $v.Trim()
    }
  }

  if (-not $map.ContainsKey("username") -or -not $map.ContainsKey("password")) {
    throw "Credential helper returned incomplete GitHub credentials."
  }

  return [pscustomobject]@{
    Username = $map["username"]
    Password = $map["password"]
    BasicAuth = [Convert]::ToBase64String([Text.Encoding]::ASCII.GetBytes(("{0}:{1}" -f $map["username"], $map["password"])))
  }
}

function Resolve-GhCommand {
  $gh = Get-Command gh -ErrorAction SilentlyContinue
  if ($gh) {
    return $gh.Source
  }

  $candidates = @(
    (Join-Path ${env:ProgramFiles} "GitHub CLI\gh.exe"),
    (Join-Path ${env:LOCALAPPDATA} "Programs\GitHub CLI\gh.exe")
  ) | Where-Object { -not [string]::IsNullOrWhiteSpace($_) }

  foreach ($candidate in $candidates) {
    if (Test-Path $candidate) {
      return $candidate
    }
  }

  return $null
}

function Invoke-Gh {
  param(
    [string]$GhExe,
    [string]$Token,
    [string[]]$CommandArgs,
    [switch]$AllowFailure
  )

  $oldGhToken = $env:GH_TOKEN
  $oldGhHost = $env:GH_HOST
  try {
    $env:GH_TOKEN = $Token
    $env:GH_HOST = "github.com"
    $output = & $GhExe @CommandArgs 2>&1
    $exitCode = $LASTEXITCODE
  } finally {
    if ($null -eq $oldGhToken) {
      Remove-Item Env:\GH_TOKEN -ErrorAction SilentlyContinue
    } else {
      $env:GH_TOKEN = $oldGhToken
    }

    if ($null -eq $oldGhHost) {
      Remove-Item Env:\GH_HOST -ErrorAction SilentlyContinue
    } else {
      $env:GH_HOST = $oldGhHost
    }
  }

  if ($exitCode -ne 0 -and -not $AllowFailure) {
    $details = (($output | ForEach-Object { $_.ToString() }) -join [Environment]::NewLine).Trim()
    if ([string]::IsNullOrWhiteSpace($details)) {
      $details = "gh exited with code $exitCode"
    }
    throw $details
  }

  $script:LastGhExitCode = $exitCode
  return ,$output
}

function Invoke-GhJson {
  param(
    [string]$GhExe,
    [string]$Token,
    [string[]]$CommandArgs,
    [switch]$AllowFailure
  )

  $output = Invoke-Gh -GhExe $GhExe -Token $Token -CommandArgs $CommandArgs -AllowFailure:$AllowFailure
  $text = (($output | ForEach-Object { $_.ToString() }) -join [Environment]::NewLine).Trim()
  if ([string]::IsNullOrWhiteSpace($text)) {
    return $null
  }
  $trimmed = $text.TrimStart()
  if (-not ($trimmed.StartsWith("{") -or $trimmed.StartsWith("["))) {
    if ($AllowFailure) {
      return $null
    }
    throw $text
  }
  return ($text | ConvertFrom-Json)
}

function Invoke-GhReleaseApiJson {
  param(
    [string]$GhExe,
    [string]$Token,
    [string]$Method,
    [string]$Path,
    [string]$InputPath,
    [switch]$AllowFailure
  )

  $args = @(
    "api",
    "--method", $Method,
    $Path,
    "-H", "Accept: application/vnd.github+json",
    "-H", "X-GitHub-Api-Version: 2022-11-28"
  )

  if ($InputPath) {
    $args += @("--input", $InputPath)
  }

  return Invoke-GhJson -GhExe $GhExe -Token $Token -CommandArgs $args -AllowFailure:$AllowFailure
}

function Get-ReleaseByTagViaGh {
  param(
    [string]$GhExe,
    [string]$Token,
    [string]$RepoName,
    [string]$ReleaseTag
  )

  $output = Invoke-Gh -GhExe $GhExe -Token $Token -CommandArgs @(
    "release", "view", $ReleaseTag,
    "--repo", $RepoName,
    "--json", "databaseId,url,name,tagName,isDraft"
  ) -AllowFailure

  if ($script:LastGhExitCode -ne 0) {
    return $null
  }

  $text = (($output | ForEach-Object { $_.ToString() }) -join [Environment]::NewLine).Trim()
  if ([string]::IsNullOrWhiteSpace($text)) {
    return $null
  }
  if (-not $text.TrimStart().StartsWith("{")) {
    return $null
  }

  return ($text | ConvertFrom-Json)
}

function Find-UntaggedDraftByNameViaGh {
  param(
    [string]$GhExe,
    [string]$Token,
    [string]$RepoName,
    [string[]]$CandidateNames
  )

  $releases = Invoke-GhReleaseApiJson -GhExe $GhExe -Token $Token -Method "GET" -Path ("repos/{0}/releases?per_page=100" -f $RepoName)
  if ($null -eq $releases) {
    return $null
  }

  foreach ($release in @($releases)) {
    if (-not $release.draft) {
      continue
    }
    if (-not [string]::IsNullOrWhiteSpace($release.tag_name)) {
      continue
    }
    if ($CandidateNames -contains $release.name) {
      return $release
    }
  }

  return $null
}

function Get-ReleaseAssetsViaGh {
  param(
    [string]$GhExe,
    [string]$Token,
    [string]$RepoName,
    [int]$ReleaseId
  )

  $assets = Invoke-GhReleaseApiJson -GhExe $GhExe -Token $Token -Method "GET" -Path ("repos/{0}/releases/{1}/assets" -f $RepoName, $ReleaseId)
  if ($null -eq $assets) {
    return @()
  }
  return @($assets)
}

function Invoke-GhReleaseUpload {
  param(
    [string]$GhExe,
    [string]$Token,
    [string]$RepoName,
    [string]$ReleaseTag,
    [string]$FilePath
  )

  [void](Invoke-Gh -GhExe $GhExe -Token $Token -CommandArgs @(
    "release", "upload", $ReleaseTag, $FilePath,
    "--repo", $RepoName,
    "--clobber"
  ))
}

function Get-ObjectPropertyValue {
  param(
    [object]$InputObject,
    [string]$PropertyName
  )

  if ($null -eq $InputObject) {
    return $null
  }

  $prop = $InputObject.PSObject.Properties[$PropertyName]
  if ($null -eq $prop) {
    return $null
  }

  return $prop.Value
}

function Invoke-CurlJson {
  param(
    [string]$Method,
    [string]$Url,
    [string]$BasicAuth,
    [object]$BodyObj,
    [int]$TimeoutSec = 60
  )

  $curlArgs = @(
    "-sS",
    "--fail-with-body",
    "--http1.1",
    "--connect-timeout", "$ConnectTimeoutSec",
    "--max-time", "$TimeoutSec",
    "-X", $Method,
    $Url,
    "-H", ("Authorization: Basic {0}" -f $BasicAuth),
    "-H", "Accept: application/vnd.github+json",
    "-H", "X-GitHub-Api-Version: 2022-11-28",
    "-H", "User-Agent: project-aura-release-script",
    "-H", "Expect:"
  )

  $tmp = $null
  try {
    if ($null -ne $BodyObj) {
      $tmp = Join-Path $env:TEMP "aura_release_payload.json"
      $json = $BodyObj | ConvertTo-Json -Depth 20
      Set-Content -Path $tmp -Value $json -Encoding UTF8
      $curlArgs += @("-H", "Content-Type: application/json", "--data-binary", ("@{0}" -f $tmp))
    }

    $response = & curl.exe @curlArgs
    if ($LASTEXITCODE -ne 0) {
      throw "curl failed with exit code $LASTEXITCODE"
    }
  } finally {
    if ($tmp -and (Test-Path $tmp)) {
      Remove-Item -Force $tmp -ErrorAction SilentlyContinue
    }
  }

  if ([string]::IsNullOrWhiteSpace($response)) {
    return $null
  }
  return ($response | ConvertFrom-Json)
}

function Invoke-CurlUpload {
  param(
    [string]$Url,
    [string]$FilePath,
    [string]$BasicAuth,
    [int]$TimeoutSec = 300
  )

  $curlArgs = @(
    "-sS",
    "--fail-with-body",
    "--http1.1",
    "--connect-timeout", "$ConnectTimeoutSec",
    "--max-time", "$TimeoutSec",
    "-X", "POST",
    $Url,
    "-H", ("Authorization: Basic {0}" -f $BasicAuth),
    "-H", "Accept: application/vnd.github+json",
    "-H", "X-GitHub-Api-Version: 2022-11-28",
    "-H", "User-Agent: project-aura-release-script",
    "-H", "Content-Type: application/octet-stream",
    "-H", "Expect:",
    "--data-binary", ("@{0}" -f $FilePath)
  )

  $response = & curl.exe @curlArgs
  if ($LASTEXITCODE -ne 0) {
    throw "Upload failed for $FilePath (exit code $LASTEXITCODE)"
  }
  return ($response | ConvertFrom-Json)
}

Write-Step "Resolving GitHub credentials"
$credentials = Get-GitHubCredentials
$basicAuth = $credentials.BasicAuth
$headers = @{
  Authorization = "Basic $basicAuth"
  Accept = "application/vnd.github+json"
  "X-GitHub-Api-Version" = "2022-11-28"
  "User-Agent" = "project-aura-release-script"
}
$ghCommand = $null
if (-not $ForceLegacyApi) {
  $ghCommand = Resolve-GhCommand
}

$release = $null
$releaseByTagUrl = "https://api.github.com/repos/$Repo/releases/tags/$Tag"
Write-Step "Checking existing release by tag: $Tag"
if ($ghCommand) {
  Write-Step "Using GitHub CLI path: $ghCommand"
  $release = Get-ReleaseByTagViaGh -GhExe $ghCommand -Token $credentials.Password -RepoName $Repo -ReleaseTag $Tag
} else {
  try {
    $release = Invoke-RestMethod -Method Get -Uri $releaseByTagUrl -Headers $headers -TimeoutSec $ApiTimeoutSec
  } catch {
    $status = $null
    if ($_.Exception.Response) {
      try { $status = [int]$_.Exception.Response.StatusCode } catch {}
    }
    if ($status -ne 404) {
      throw
    }
  }
}

$notesText = $null
if (-not $SkipReleaseUpdate -and -not $SkipNotes -and (Test-Path $NotesPath)) {
  Write-Step "Loading release notes: $NotesPath"
  $notesText = Get-Content -Raw -Path $NotesPath
}

if ($ghCommand) {
  $draftNames = @($Title, "v$Version", $Version) | Where-Object { -not [string]::IsNullOrWhiteSpace($_) } | Select-Object -Unique
  $notesTmp = $null
  $payloadTmp = $null
  try {
    if ($notesText) {
      $notesTmp = Join-Path $env:TEMP ("aura_release_notes_{0}.md" -f $Version)
      Set-Content -Path $notesTmp -Value $notesText -Encoding UTF8
    }

    if (-not $release) {
      $draftRelease = Find-UntaggedDraftByNameViaGh -GhExe $ghCommand -Token $credentials.Password -RepoName $Repo -CandidateNames $draftNames
      $payload = @{
        tag_name = $Tag
        target_commitish = $TargetCommitish
        name = $Title
        draft = [bool]$Draft
        prerelease = $false
      }
      if ($notesText) {
        $payload["body"] = $notesText
      }

      $payloadTmp = Join-Path $env:TEMP ("aura_release_payload_{0}.json" -f $Version)
      $payload | ConvertTo-Json -Depth 20 | Set-Content -Path $payloadTmp -Encoding UTF8

      if ($draftRelease) {
        Write-Step "Updating untagged draft release id=$($draftRelease.id)"
        $release = Invoke-GhReleaseApiJson -GhExe $ghCommand -Token $credentials.Password -Method "PATCH" -Path ("repos/{0}/releases/{1}" -f $Repo, $draftRelease.id) -InputPath $payloadTmp
        Write-Host "Updated draft release: $($release.html_url)"
      } else {
        Write-Step "Creating release $Tag (target: $TargetCommitish)"
        $release = Invoke-GhReleaseApiJson -GhExe $ghCommand -Token $credentials.Password -Method "POST" -Path ("repos/{0}/releases" -f $Repo) -InputPath $payloadTmp
        Write-Host "Created release: $($release.html_url)"
      }
    } else {
      if ($SkipReleaseUpdate) {
        Write-Step "Skipping release metadata update for existing release id=$($release.databaseId)"
        Write-Host "Using release: $($release.url)"
      } else {
        Write-Step "Updating existing release id=$($release.databaseId)"
        $payload = @{
          name = $Title
          draft = [bool]$Draft
          prerelease = $false
        }
        if ($notesText) {
          $payload["body"] = $notesText
        }
        $payloadTmp = Join-Path $env:TEMP ("aura_release_payload_{0}.json" -f $Version)
        $payload | ConvertTo-Json -Depth 20 | Set-Content -Path $payloadTmp -Encoding UTF8
        $release = Invoke-GhReleaseApiJson -GhExe $ghCommand -Token $credentials.Password -Method "PATCH" -Path ("repos/{0}/releases/{1}" -f $Repo, $release.databaseId) -InputPath $payloadTmp
        Write-Host "Updated release: $($release.html_url)"
      }
    }

    $releaseIdValue = Get-ObjectPropertyValue -InputObject $release -PropertyName "id"
    if ($null -eq $releaseIdValue) {
      $releaseIdValue = Get-ObjectPropertyValue -InputObject $release -PropertyName "databaseId"
    }
    if ($null -eq $releaseIdValue) {
      throw "Unable to determine release id for GitHub release operation."
    }
    $releaseId = [int]$releaseIdValue
    Write-Step "Fetching existing assets"
    $existing = Get-ReleaseAssetsViaGh -GhExe $ghCommand -Token $credentials.Password -RepoName $Repo -ReleaseId $releaseId

    if ($PruneAssetsToList) {
      Write-Step "Pruning release assets to requested list"
      foreach ($asset in $existing) {
        if ($AssetNames -notcontains $asset.name) {
          [void](Invoke-GhReleaseApiJson -GhExe $ghCommand -Token $credentials.Password -Method "DELETE" -Path ("repos/{0}/releases/assets/{1}" -f $Repo, $asset.id))
          Write-Host "Deleted extra asset: $($asset.name)"
        }
      }

      $existing = Get-ReleaseAssetsViaGh -GhExe $ghCommand -Token $credentials.Password -RepoName $Repo -ReleaseId $releaseId
    }

    foreach ($name in $AssetNames) {
      $path = Join-Path $AssetsDir $name
      Write-Step "Uploading asset: $name"
      Invoke-GhReleaseUpload -GhExe $ghCommand -Token $credentials.Password -RepoName $Repo -ReleaseTag $Tag -FilePath $path
      $fileInfo = Get-Item -LiteralPath $path
      Write-Host ("Uploaded: {0} ({1} bytes)" -f $name, $fileInfo.Length)
    }

    $release = Get-ReleaseByTagViaGh -GhExe $ghCommand -Token $credentials.Password -RepoName $Repo -ReleaseTag $Tag
    Write-Host ""
    Write-Host "Release ready: $($release.url)"
    return
  } finally {
    if ($notesTmp -and (Test-Path $notesTmp)) {
      Remove-Item -Force $notesTmp -ErrorAction SilentlyContinue
    }
    if ($payloadTmp -and (Test-Path $payloadTmp)) {
      Remove-Item -Force $payloadTmp -ErrorAction SilentlyContinue
    }
  }
}

if (-not $release) {
  Write-Step "Creating release $Tag (target: $TargetCommitish)"
  $createPayload = @{
    tag_name = $Tag
    target_commitish = $TargetCommitish
    name = $Title
    draft = [bool]$Draft
    prerelease = $false
    generate_release_notes = $false
  }
  if ($notesText) {
    $createPayload["body"] = $notesText
  }
  $release = Invoke-CurlJson -Method "POST" -Url ("https://api.github.com/repos/$Repo/releases") -BasicAuth $basicAuth -BodyObj $createPayload -TimeoutSec $ApiTimeoutSec
  Write-Host "Created release: $($release.html_url)"
} else {
  if ($SkipReleaseUpdate) {
    Write-Step "Skipping release metadata update for existing release id=$($release.id)"
    Write-Host "Using release: $($release.html_url)"
  } else {
    Write-Step "Updating existing release id=$($release.id)"
    $patchPayload = @{
      name = $Title
      draft = [bool]$Draft
      prerelease = $false
    }
    if ($notesText) {
      $patchPayload["body"] = $notesText
    }
    $release = Invoke-CurlJson -Method "PATCH" -Url ("https://api.github.com/repos/$Repo/releases/$($release.id)") -BasicAuth $basicAuth -BodyObj $patchPayload -TimeoutSec $ApiTimeoutSec
    Write-Host "Updated release: $($release.html_url)"
  }
}

$assetsApiUrl = "https://api.github.com/repos/$Repo/releases/$($release.id)/assets"
Write-Step "Fetching existing assets"
$existing = Invoke-RestMethod -Method Get -Uri $assetsApiUrl -Headers $headers -TimeoutSec $ApiTimeoutSec
if ($null -eq $existing) {
  $existing = @()
} elseif ($existing -isnot [System.Array]) {
  $existing = @($existing)
}

if ($PruneAssetsToList) {
  Write-Step "Pruning release assets to requested list"
  foreach ($asset in $existing) {
    if ($AssetNames -notcontains $asset.name) {
      $deleteUrl = "https://api.github.com/repos/$Repo/releases/assets/$($asset.id)"
      [void](Invoke-CurlJson -Method "DELETE" -Url $deleteUrl -BasicAuth $basicAuth -BodyObj $null -TimeoutSec $ApiTimeoutSec)
      Write-Host "Deleted extra asset: $($asset.name)"
    }
  }

  $existing = Invoke-RestMethod -Method Get -Uri $assetsApiUrl -Headers $headers -TimeoutSec $ApiTimeoutSec
  if ($null -eq $existing) {
    $existing = @()
  } elseif ($existing -isnot [System.Array]) {
    $existing = @($existing)
  }
}

foreach ($name in $AssetNames) {
  $path = Join-Path $AssetsDir $name
  Write-Step "Uploading asset: $name"
  $dupes = $existing | Where-Object { $_.name -eq $name }
  foreach ($asset in $dupes) {
    $deleteUrl = "https://api.github.com/repos/$Repo/releases/assets/$($asset.id)"
    [void](Invoke-CurlJson -Method "DELETE" -Url $deleteUrl -BasicAuth $basicAuth -BodyObj $null -TimeoutSec $ApiTimeoutSec)
    Write-Host "Deleted old asset: $name"
  }

  $uploadUrl = "https://uploads.github.com/repos/$Repo/releases/$($release.id)/assets?name=$([uri]::EscapeDataString($name))"
  $uploaded = $null
  $maxRetries = 3
  for ($attempt = 1; $attempt -le $maxRetries; $attempt++) {
    try {
      Write-Step ("Attempt {0}/{1}: {2}" -f $attempt, $maxRetries, $name)
      $uploaded = Invoke-CurlUpload -Url $uploadUrl -FilePath $path -BasicAuth $basicAuth -TimeoutSec $UploadTimeoutSec
      break
    } catch {
      if ($attempt -eq $maxRetries) {
        throw
      }
      Start-Sleep -Seconds (2 * $attempt)
    }
  }

  Write-Host ("Uploaded: {0} ({1} bytes)" -f $uploaded.name, $uploaded.size)
}

Write-Host ""
Write-Host "Release ready: $($release.html_url)"
