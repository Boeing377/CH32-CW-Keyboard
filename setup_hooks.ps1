# setup_hooks.ps1 - 配置 Git hooks 路径
# 运行一次即可: powershell -ExecutionPolicy Bypass -File setup_hooks.ps1

$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $repoRoot

Write-Host "Configuring Git hooks path..." -ForegroundColor Cyan
git config core.hooksPath .githooks

if ($LASTEXITCODE -eq 0) {
    Write-Host "Done! Git hooks configured successfully." -ForegroundColor Green
    Write-Host "Hooks directory: $repoRoot\.githooks"
    Write-Host ""
    Write-Host "Available hooks:"
    Get-ChildItem "$repoRoot\.githooks\*" | ForEach-Object {
        Write-Host "  - $($_.Name)"
    }
} else {
    Write-Error "Failed to configure Git hooks. Is this a Git repository?"
}
