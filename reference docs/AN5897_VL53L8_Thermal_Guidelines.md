# AN5894_ToF_Fields_of_View.md

**1. Circular and Square Fields of View**
*[Image: Figure 1. STMicroelectronics' ToF sensor module - Photograph of the VL53L7CX optical module.]*
*[Image: Figure 2. ToF module overview - Block diagram showing a VCSEL emitter and SPAD receiver projecting cones of light onto a target.]*
*[Image: Figure 3. Single-zone and multi-zone sensor measurements - Diagram contrasting a single depth output vs a 3x3 multi-zone depth matrix.]*
*[Image: Figure 4. Points used for computing diagonal FoVs - Geometric diagram proving that FoV angles cannot simply be combined using the Pythagorean theorem.]*

**2. Field of View Definitions**
- **Field of Illumination (FoI):** Emitter intensity profile.
*[Image: Figure 5. Example of the VL53L4CD FoI - Heatmap showing concentrated red energy in the center fading to blue at the edges.]*
- **Nominal FoV:** Area where signal can be detected for nominal performances on Lambertian targets.
*[Image: Figure 6. VL53L7CX nominal FoV example - Side view illustrating a 60-degree detection cone for both Rx and Tx channels.]*
- **Exclusion Cone/Zone:** Clearance required to avoid structural cross-talk.
*[Image: Figure 7. Example of the VL53L3CX exclusion cones/zones - Schematic defining a 25-degree collector exclusion cone and 35-degree emitter exclusion cone.]*
- **Detection Volume:** Area where valid distances can actually be extracted, heavily dependent on target reflectance.
*[Image: Figure 8. Impact of the reflectance on the detection volume - Diagram showing a white 88% target being detected easily at the edge of the FoV, whereas a grey 17% target requires a deeper overlap.]*
*[Image: Figure 9. Impact of the distance on the detection volume - Graph showing a spade-like curve where lateral detection volume is widest at short ranges and tapers off at long distances.]*
*[Image: Figure 10. Schematic of all STMicroelectronics ToF field of view definitions - Overlay of Detection volume, Nominal FoV, Exclusion zone, and Keep-out cone profiles.]*