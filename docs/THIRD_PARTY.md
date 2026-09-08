# Third-party resources

## FreeGLUT 3.8.0

- [Source](https://github.com/freeglut/freeglut/tree/v3.8.0), by Pawel W. Olszta
  and the FreeGLUT contributors.
- [License and copyright notice](https://github.com/freeglut/freeglut/blob/v3.8.0/COPYING)
  and [authors](https://github.com/freeglut/freeglut/blob/v3.8.0/AUTHORS).
- Used for window creation, input, primitive geometry and fonts.
- Downloaded automatically into ignored build directories; dependency source is
  not stored in this repository. Both build paths use
  [FreeGLUT.cmake](../cmake/FreeGLUT.cmake).
- The build copies the license to `FREEGLUT-LICENSE.txt` beside the executable.
- Original
  [source archive](https://codeload.github.com/freeglut/freeglut/zip/refs/tags/v3.8.0)
  SHA-256: `66c12fbf41ef5da34a386d59dbd389e30b601124937f8edb81a30a5006247f93`.

## Patrick Hand

- Handwriting by Patrick Wagesreiter, from
  [Google Fonts](https://github.com/google/fonts/tree/aeb9574d3cb4ab2ac5de70a8c730b11662281803/ofl/patrickhand).
- The generated atlas and glyph metrics use the
  [SIL Open Font License 1.1](../assets/fonts/OFL.txt).
- The atlas ships in `assets/fonts/`; no font installation or download is needed
  to run the farm. Regenerate it with
  [generate-font.py](../scripts/generate-font.py).

## Farmer head texture

- **Author:** TheNess.
- **Source:**
  [Human male body and head texture (painted with Adobe Animate)](https://opengameart.org/content/human-male-body-and-head-texture-painted-with-adobe-animate).
- **Original file:**
  [head_texture.png](https://opengameart.org/sites/default/files/head_texture.png).
- **License:**
  [CC0 1.0 Universal](https://creativecommons.org/publicdomain/zero/1.0/).
- **Bundled file:** `assets/textures/characters/farmer_head.bmp`.
- **Changes:** Converted from PNG to 24-bit BMP, retaining the complete original
  150 x 150 image. The renderer maps its face region onto a procedural head.
- [Source and checksum](../assets/textures/characters/SOURCE.txt) and
  [full license](../assets/textures/characters/CC0-1.0.txt) are bundled beside
  the BMP.
- Recreate the BMP with
  [import-farmer-texture.ps1](../scripts/import-farmer-texture.ps1).

The character geometry, farmer clothing, lantern and animation are implemented
locally. No personal photograph, external human model or animation pack is used.

## References

Conceptual references; no code or images were copied from these pages:

- [FreeGLUT API](https://freeglut.sourceforge.net/docs/api.php).
- [Khronos lighting](https://wikis.khronos.org/opengl/How_lighting_works).
- Tiago Sousa,
  [GPU Gems 3: Vegetation Procedural Animation](https://developer.nvidia.com/gpugems/gpugems3/part-iii-rendering/chapter-16-vegetation-procedural-animation-and-shading-crysis).

Farm geometry and the four material textures were created for this project. The
material textures can be regenerated with
[generate-textures.py](../scripts/generate-textures.py).
