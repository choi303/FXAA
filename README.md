# FXAA
FXAA technique with DirectX 11/HLSL. Based on [Nvidia's FXAA whitepaper](https://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf).
## What is FXAA?
Fast approximate anti-aliasing (FXAA) is a screen-space anti-aliasing algorithm created by Timothy Lottes at Nvidia.

![fxaa_comp_1](https://user-images.githubusercontent.com/60492235/134978222-9fe5e074-a531-4d17-b622-5051c43d6a41.jpg)
## Algorithm Description

1. The input data is the rendered image and optionally the luminance data.

2. Acquire the luminance data. This data could be passed into the FXAA algorithm from the rendering step as an alpha channel embedded into the image to be antialiased, calculated from the rendered image, or approximated by using the green channel as the luminance data.

3. Find high contrast pixels by using a high pass filter that uses the luminance data. Low contrast pixels that are found are excluded from being further altered by FXAA. The high pass filter that excludes low contrast pixels can be tuned to balance speed and sensitivity.

4. Use contrast between adjacent pixels to heuristically find edges, and determine whether the edges are in the horizontal or vertical directions. The blend direction of a pixel will be perpendicular to the detected edge direction on that pixel.

5. Calculate one blend factor for a high-contrast pixel by analyzing the luminance data in the 3x3 grid of pixels with the pixel in question being the center pixel.

6. Search along the detected edge to determine how long that edge goes for and what direction the actual edge goes when the detected horizontal or vertical edge ends in order to take into account the actual edge's direction in order to calculate a second blend factor. This step can be tuned for more quality by increasing the search resolution and how far the search goes before the search for the edge's end gives up, or for more speed by reducing both.

7. Blend the pixel using the chosen blend direction and the maximum of both of the blend factors that were calculated.

## Comparison
The main advantage of this technique over conventional spatial anti-aliasing is that it does not require large amounts of computing power. It achieves this by smoothing undesirable jagged edges ("jaggies") as pixels, according to how they appear on-screen, rather than analyzing the 3D model itself, as in conventional spatial anti-aliasing. Since it is not based on the actual geometry, it will smooth not only edges between triangles, but also edges inside alpha-blended textures, or those resulting from pixel shader effects, which are immune to the effects of multisample anti-aliasing (MSAA).

The downsides are that high contrast texture maps are blurred, that FXAA must be applied before rendering the HUD elements of a game lest it affect them too, and that polygonal details smaller than one pixel that would have been captured and rendered by MSAA and SSAA will not be captured and rendered by FXAA alone.

![18j12dpqpngjyjpg](https://user-images.githubusercontent.com/60492235/134978568-0febd082-86d0-444f-adf4-423f68dad083.jpg)

## References
- Lottes, Timothy (February 2009). ["FXAA"](https://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf) (PDF). NVIDIA. Retrieved 29 September 2012.
- Jeff Atwood | Coding Horror (December 2011). ["What is FXAA?"](https://www.kotaku.com.au/2011/12/what-is-fxaa/)
- John Mautari, Radeon Pro (October 2013). ["FXAA"](https://www.radeonpro.info/features/post-processing/fxaa/)
