$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing
$taskRoot = Split-Path $PSScriptRoot -Parent
$taskCache = Join-Path $taskRoot 'build\farmer-assets'
$taskDestination = Join-Path $taskRoot 'assets\textures\characters'
New-Item -ItemType Directory -Force -Path $taskCache, $taskDestination | Out-Null
$taskSource = Join-Path $taskCache 'head_texture.png'
Invoke-WebRequest -Uri 'https://opengameart.org/sites/default/files/head_texture.png' -OutFile $taskSource
if ((Get-FileHash -LiteralPath $taskSource -Algorithm SHA256).Hash -ne 'AD3B2C7EECF959D6675E3F1EBF3474A00D40D91619EA4D9A3DCCB1B68B8927D7') {
    throw 'Farmer texture checksum mismatch.'
}
$taskImage = [Drawing.Image]::FromFile($taskSource)
try {
    # Convert the online CC0 texture to the BMP format used by this renderer.
    $taskBitmap = [Drawing.Bitmap]::new($taskImage.Width, $taskImage.Height, [Drawing.Imaging.PixelFormat]::Format24bppRgb)
    $taskGraphics = [Drawing.Graphics]::FromImage($taskBitmap)
    try {
        # Explicit pixel units avoid scaling images whose source DPI differs from
        # the destination bitmap DPI (DrawImageUnscaled uses physical dimensions).
        $taskRectangle = [Drawing.Rectangle]::new(0, 0, $taskImage.Width, $taskImage.Height)
        $taskGraphics.DrawImage($taskImage, $taskRectangle, 0, 0, $taskImage.Width, $taskImage.Height, [Drawing.GraphicsUnit]::Pixel)
        $taskBitmap.Save((Join-Path $taskDestination 'farmer_head.bmp'), [Drawing.Imaging.ImageFormat]::Bmp)
    }
    finally {
        $taskGraphics.Dispose()
        $taskBitmap.Dispose()
    }
}
finally { $taskImage.Dispose() }
Write-Output 'Imported TheNess head texture (CC0 1.0) as farmer_head.bmp. See assets/textures/characters/SOURCE.txt.'
