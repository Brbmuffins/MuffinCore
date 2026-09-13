<#
.SYNOPSIS
  Run a bounded task through the local Codex CLI non-interactively.

.DESCRIPTION
  Reads the prompt from a file and pipes it on stdin. That avoids Windows PowerShell 5.1
  mangling quoted native arguments. The final message goes to -OutFile.
  Defaults to a read-only sandbox; pass -Sandbox workspace-write only for approved edit tasks.

.EXAMPLE
  powershell -NoProfile -File tools\codex-task.ps1 -PromptFile task.prompt.md -OutFile task.result.md
#>
param(
    [Parameter(Mandatory = $true)] [string] $PromptFile,
    [Parameter(Mandatory = $true)] [string] $OutFile,
    [ValidateSet('read-only', 'workspace-write')] [string] $Sandbox = 'read-only',
    [string] $WorkDir = (Split-Path -Parent $PSScriptRoot),
    [string] $Model,
    # Optional JSONL event stream (commands run, messages) so a long task's progress can be checked.
    [string] $EventsFile
)

$ErrorActionPreference = 'Stop'

$codex = (Get-Command codex -ErrorAction SilentlyContinue).Source
if (-not $codex) {
    $fallback = Join-Path $env:USERPROFILE '.codex\plugins\.plugin-appserver\codex.exe'
    if (Test-Path $fallback) { $codex = $fallback } else { throw 'Codex CLI not found on PATH or in the default install location.' }
}

if (-not (Test-Path $PromptFile)) { throw "Prompt file not found: $PromptFile" }

$arguments = @('exec', '--sandbox', $Sandbox, '-C', $WorkDir, '--ephemeral', '--color', 'never', '-o', $OutFile)
if ($Model) { $arguments += @('-m', $Model) }
$arguments += '-'

if ($EventsFile) {
    $arguments = @('exec', '--json') + $arguments[1..($arguments.Count - 1)]
    Get-Content -Raw -LiteralPath $PromptFile | & $codex @arguments > $EventsFile
} else {
    Get-Content -Raw -LiteralPath $PromptFile | & $codex @arguments
}
exit $LASTEXITCODE
