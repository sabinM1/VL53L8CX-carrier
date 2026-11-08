# AN6271 — Liquid-level measurement

**1. Observations on Water**
Measuring liquid levels is complex due to reflection, absorption, and penetration of photons. 
*[Image: Figure 1. Different return paths of photons on water - Diagram showing photons reflecting off the surface, penetrating and returning from the bottom, or scattering.]*
- **Still Water:** Acts specularly (mirror-like). Only center zones return data.
*[Image: Figure 2. Still water at range = 140 mm - 8x8 matrix where only the central 4 zones show valid distance and signal strength.]*
- **Agitated/Turbid Water:** Acts as a Lambertian surface, scattering light and providing data across many zones.
*[Image: Figure 3. Agitated water at 140 mm - Matrix showing valid, slightly varying distance readings across a wide spread of zones.]*
*[Image: Figure 4. Turbid (dirty) water - Matrix showing valid distance readings heavily populated across the entire sensor grid.]*
- **Container properties:** Reflective containers interfere with measurements.
*[Image: Figure 5. Still water in a glass dish - Matrix showing heavy interference where outer zones measure the table beneath the glass rather than the water surface.]*

**2. Measurement Principle**
Uses the 12 center zones of the 8x8 array.
*[Image: Figure 6. Measurement zone - An 8x8 grid highlighting zones 19, 20, 26-29, 34-37, 43, 44 as the core area of interest.]*
A C-code algorithm is provided that searches the center zones: if signal strength is highly localized, it determines the water is still (specular) and returns the max signal distance.