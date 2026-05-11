# Model Viewer — Real-time PBR Rendering Engine

This project is a high-performance, **physically based rendering (PBR) engine** developed as part of my **Project Laboratory (Önálló laboratórium)** at the Budapest University of Technology and Economics. The primary goal was to implement a modern rendering pipeline that accurately simulates the interaction between light and various material surfaces using industry-standard mathematical models.

The engine is built from the ground up using **C++** and **modern OpenGL**, focusing on architectural clarity, resource efficiency and visual fidelity.

> ***Note:***
This project takes heavy inspiration from [**LearnOpenGL**](https://learnopengl.com/), courtesy of [***Joey De Vries***](https://github.com/JoeyDeVries). All credit for such inspirations goes to him.

## The PBR Approach

Traditional rendering models — like ***Phong*** or ***Blinn-Phong*** which, too is explored in this project — often rely on empirical approximations that require manual tweaking to look "decent" under different lighting conditions.

In contrast, the heart of this engine follows the [**Physically Based Rendering (PBR)**](https://en.wikipedia.org/wiki/Physically_based_rendering) paradigm. The core idea is to simulate the physical behaviour of light and materials as they exist in the real world. By adhering to the laws of physics, the engine achieves a consistent and realistic look regardless of the lighting setup.

### Material Properties

The engine uses the **Metallic-Roughness workflow**, where every surface is defined by a few intuitive parameters:

* **Albedo:** The base color of the surface, free from any pre-baked lighting.

* **Roughness:** Controls how smooth or "bumpy" a surface is at the microscopic level, directly affecting the spread of reflections.

* **Metallic:** Defines whether a material is a conductor (*metal*) or a dielectric (*insulator*), which changes how it reflects light at its base.

### Key Principles

To maintain physical accuracy, the engine strictly follows the following rules:

* **Energy Conservation:** A surface cannot reflect more light that it receives. As a material gets rougher, its reflections increasingly become wider but dimmer.

* **Microfacet Theory:** We make the assumption that surfaces are composed of thousands of tiny, microscopic mirrors (*microfacets*). The mathematical models described below — ***Normal Distribution***, ***Geometry*** and ***Fresnel functions*** — are used to calculate how many of these mirrors are aligned to reflect light toward the camera.

## The Rendering Equation

### Radiometry

Before diving into the equations and complex mathematical concepts, it is essential to define the physical quantity we are simulating. The engine operates in a **radiometrically linear space**, focusing primarily on ***Radiance ($L$)***.

Radiance is the amount of radiant flux ($\Phi$) emitted, reflected or received by a surface, per unit solid angle ($\omega$), per unit projected area ($A$), scaled by the incident angle ($\theta$) of the light to the surface normal:

$$L = \frac{d^2\Phi}{dA \cdot \cos\theta \cdot d\omega}$$

It is the most important quantity in rendering because it corresponds directly to what a pixel in a camera or an eye perceives.

In our PBR pipeline:

* **Light sources** are defined by their radiance.

* **Materials** attenuate this radiance based on the BRDF.

* **The final image** is tone-mapped to a displayable color range.

### The Equation

At its core, the engine aims to solve the [**Rendering Equation**](https://en.wikipedia.org/wiki/Rendering_equation) (*originally proposed by **David Immel et al. and James Kajiya** in 1986*). This integral equation describes the total amount of light $L_o$ emitted from a point $p$ in a given direction $\omega_o$:

$$L_o(p, \omega_o) = L_e(p, \omega_o) + \int_{\Omega} f_r(p, \omega_i, \omega_o) L_i(p, \omega_i) (n \cdot \omega_i) d\omega_i$$

#### Breakdown of the Equation:
* **$L_o(p, \omega_o)$:** The total outgoing radiance from point $p$ in direction $\omega_o$.
* **$L_e(p, \omega_o)$:** Emitted radiance (used for light sources or emissive materials). Regarding the engine, this is only used for emission.
* **$\int_{\Omega} \dots d\omega_i$:** The integral over the unit hemisphere $\Omega$, summing all incoming light from all possible directions. It is the sum of all radiance, known as **irradiance**.
* **$f_r(p, \omega_i, \omega_o)$:** The **BRDF**, which defines how much light is reflected toward the camera based on material properties.
* **$L_i(p, \omega_i)$:** Incoming radiance from direction $\omega_i$.
* **$n \cdot \omega_i$:** The cosine term (Lambert's Law), which accounts for the attenuation of light based on its incident angle.

In real-time rendering, solving this integral for every pixel at every frame is computationally impossible. This project demonstrates how we can approximate this equation using **Direct Lighting** (*via the Cook-Torrance BRDF*) and **Indirect Global Illumination** (*via Image-Based Lighting*).

>***Side-note:***
The integration of emissive materials — along with Physically Based Bloom — is still in development.

## Core Rendering: The Cook-Torrance Reflectance Model

The engine implements the **Cook-Torrance BRDF** ([***Bidirectional Reflectance Distribution Function***](https://en.wikipedia.org/wiki/Bidirectional_reflectance_distribution_function)) to resolve the specular component of the reflected light. This model is based on the microfacet theory, where the surface's appearance is determined by the statistical distribution of microscopic mirrors.

The specular BRDF is calculated using the following formula:
$$f_{specular} = \frac{D \cdot F \cdot G}{4(\omega_o \cdot n)(\omega_i \cdot n)}$$

To achieve realistic results, I implemented the following standard approximations for each term:

>***Note:***
This section is ***still in active development.*** The engine will be able to support multiple combinations of different approximations. At the time of this writing, **every single term has its own dedicated implementation**, as described below.

### 1. Normal Distribution Function (*D*)

I used the **Trowbridge-Reitz GGX distribution.** This function estimates the ratio of microfacets exactly aligned with the halfway vector ($h$), which defines the shape and "glow" of the specular highlight:

$$D(n, h, \alpha) = \frac{\alpha^2}{\pi((n \cdot h)^2(\alpha^2 - 1) + 1)^2}$$

### 2. Geometry Function (*G*)

To account for microfacet self-occlusion (*such as shadowing and masking*), I implemented **Smith's method with Schlick-GGX.** This ensures that the material does not appear unrealistically bright at grazing angles (*and thus adhering to energy conservation*):

$$G(n, v, l, k) = G_{sub}(n, v, k) \cdot G_{sub}(n, l, k)$$
$$G_{sub}(n, v, k) = \frac{n \cdot v}{(n \cdot v)(1 - k) + k}$$

### 3. Fresnel Function (*F*)

The **Schlick approximation** is used to determine the ratio of reflected light vs. refracted light — in simpler terms, the *specular* and *diffuse light*. This ensures that non-metals exhibit increased reflectivity at grazing angles, while metals retain their characteristic tinted reflections:

$$F(h, v, F_0) = F_0 + (1 - F_0)(1 - (h \cdot v))^5$$

## Image Based Lighting (IBL)

While the BRDF handles punctual light sources — such as ***directional, point and spotlights*** — [**Image Based Lighting (IBL)**](https://en.wikipedia.org/wiki/Image-based_lighting) allows the engine to treat the surrounding environment as a light source. By using [**High-Dynamic Range (HDR)**](https://en.wikipedia.org/wiki/High_dynamic_range) cubemaps, we can simulate realistic ***indirect global illumination***.

>***Note:***
Keep in mind that ***IBL is still an approximation.*** The engine is not capable of performing *real* global illumination, much less utilize algorithms affiliated with it — like Monte-Carlo ray-tracing.

To make this process doable in real-time, I implemented the **Split-Sum Approximation**, which breaks down the complex radiance integral into two simpler, pre-calculated parts:

### 1. Diffuse Irradiance

The diffuse component represents the light bouncing off the surface in all directions. Instead of sampling the environt ment thousands of times per-pixel, the engine pre-computes an **Irradiance Map**. Each texel in this map stores the integral of the environment's radiance over a hemisphere oriented towards the surface normal.

* **Technique:** Convolution of the HDR environment map using a spherical integral.

* **Optimization:** For the diffuse part, I am also exploring ***Spherical Harmonics (SH)*** to represent this low-frequency lighting data with just 9 coefficients per color channel — generated utilizing *Compute Shaders* — thus significantly reducing memory bandwidth.

### 2. Specular Radiance

The specular part is more complex because it depends on both the surface roughness and the view angle. As mentioned before, I utilize the ***Split Sum Approximation*** (*courtesy of Epic Games*) to solve this:

* **Pre-filtered Environment Map:** I generate a cubemap with different mipmap levels. Each level stores the environment convolved with a **GGX** specular lobe for a specific roughness value.

* **BRDF Integration Look-Up Texture (LUT):** A 2D texture that stores the scale and bias values of the Fresnel respone ($F_0$). This allows the engine to combine the pre-filtered map with the material properties in a single texture fetch.

## Technical Specifications ##

* **Language:** C++20

* **Graphics API:** OpenGL 4.6

* **Libraries:**

    * 

## Features at a glance

## Compilation and Execution

### Prerequisites

In order to run this program you need to have **C++20** installed and an **OpenGL 4.6 compliant** GPU driver.

### How to Build

## Documentation

Work in Progress $\dots$

## License

As of $2026.05.10$, this project is licensed under the **MIT License** — see the [LICENSE](./LICENSE) file for details.
