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

## References

Conceptual references; no code or images were copied from these pages:

- [FreeGLUT API](https://freeglut.sourceforge.net/docs/api.php).
- [Khronos lighting](https://wikis.khronos.org/opengl/How_lighting_works).
- Tiago Sousa,
  [GPU Gems 3: Vegetation Procedural Animation](https://developer.nvidia.com/gpugems/gpugems3/part-iii-rendering/chapter-16-vegetation-procedural-animation-and-shading-crysis).

Farm geometry and textures were created for this project. Textures can be
regenerated with [generate-textures.py](../scripts/generate-textures.py).
