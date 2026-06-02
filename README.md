# Model Viewer — Real-time PBR Rendering Engine

This project is a high-performance, **physically based rendering (PBR) engine** developed as part of my **Project Laboratory (Önálló laboratórium)** at the Budapest University of Technology and Economics. The primary goal was to implement a modern rendering pipeline that accurately simulates the interaction between light and various material surfaces using industry-standard mathematical models.

The engine is built from the ground up using **C++** and **modern OpenGL**, focusing on architectural clarity, resource efficiency and visual fidelity.

> ***Note:***
This project takes heavy inspiration from [**LearnOpenGL**](https://learnopengl.com/), courtesy of [***Joey De Vries***](https://github.com/JoeyDeVries). All credit for such inspirations goes to him.

https://github.com/user-attachments/assets/927b0fd4-53ee-42ca-b07d-b68f36e43162

## The PBR Approach

Traditional lighting models — like ***Phong*** or ***Blinn-Phong*** which, too is explored in this project — often rely on empirical approximations that require manual tweaking to look "decent" under different lighting conditions.

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

### Radiance

Before diving into the equations and complex mathematical concepts, it is essential to define the physical quantity we are simulating. The engine operates in a **radiometrically linear space**, focusing primarily on ***Radiance*** ($L$).

Radiance is the amount of radiant flux ($\Phi$) emitted, reflected or received by a surface, per unit solid angle ($\omega$), per unit projected area ($A$), scaled by the incident angle ($\theta$) of the light to the surface normal:

$$L = \frac{d^2\Phi}{dA \cdot \cos\theta \cdot d\omega}$$

It is the most important quantity in rendering because it corresponds directly to what a pixel in a camera or an eye perceives.

In our PBR pipeline:

* **Light sources** are defined by their radiance.

* **Materials** attenuate this radiance based on the BRDF.

* **The final image** is ***tone-mapped*** to a displayable color range and subsequently ***gamma-corrected*** to account for non-linear brightness output of monitors, mapping the linear (*physically correct*) values to the **sRGB** color space. 

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

## Core Rendering: The Cook-Torrance Reflectance Model

The engine implements the **Cook-Torrance BRDF** ([***Bidirectional Reflectance Distribution Function***](https://en.wikipedia.org/wiki/Bidirectional_reflectance_distribution_function)) to resolve the specular component of the reflected light. This model is based on the microfacet theory, where the surface's appearance is determined by the statistical distribution of microscopic mirrors.

The specular BRDF is calculated using the following formula:

$$f_{specular} = \frac{D \cdot F \cdot G}{4(\omega_o \cdot n)(\omega_i \cdot n)}$$

To achieve realistic results, I implemented the following approximations for each term:

>**Note**:
You can dinamically switch between which approximation of the *Distribution* and *Geometry* term the engine uses during runtime, as described in the Documentation section at the end.

### 1. Normal Distribution Function (*D*)

This function estimates the ratio of microfacets exactly aligned with the halfway vector ($h$), which defines the shape and "glow" of the specular highlight.

#### Trowbridge-Reitz GGX

$$D_{\text{GGX}}(n, h, \alpha) = \frac{\alpha^2}{\pi((n \cdot h)^2(\alpha^2 - 1) + 1)^2}$$

#### Beckmann

$$D_{\text{Beckmann}}(n, h, \alpha) = \frac{1}{\pi \alpha^2 (n \cdot h)^4} \exp\left( \frac{(n \cdot h)^2 - 1}{\alpha^2 (n \cdot h)^2} \right)$$

### 2. Geometry Function (*G*)

This accounts for microfacet self-occlusion (*such as shadowing and masking*), which ensures that the material does not appear unrealistically bright at grazing angles (*and thus adhering to energy conservation*).

#### Schlick-GGX

$$G(n, v, l, k) = G_{sub}(n, v, k) \cdot G_{sub}(n, l, k)$$
$$G_{sub}(n, v, k) = \frac{n \cdot v}{(n \cdot v)(1 - k) + k}$$

#### Kelemen-Szirmay

$$G_{\text{Kelemen}}(n, l, v, h) = \frac{(n \cdot l)(n \cdot v)}{(v \cdot h)^2}$$

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

$$L_o(p,\omega_o) \approx \int_{\Omega} L_i(p,\omega_i) d\omega_i \cdot \int_{\Omega} f_r(p,\omega_i,\omega_o) (n \cdot \omega_i) d\omega_i$$

This splits the **reflectance part** of the rendering equation into two individual parts, as described below:

* **Pre-filtered Environment Map:** The engine generates a cubemap with different mipmap levels, of which each level stores the environment convolved with a **GGX** specular lobe for a specific roughness value.

* **BRDF Integration Look-Up Texture (LUT):** A 2D texture that stores the scale and bias values of the Fresnel respone ($F_0$). This allows the engine to combine the pre-filtered map with the material properties in a single texture fetch.

## Post-Processing Pipeline

To increase visual fidelity and realism, the rendering pipeline concludes with a dedicated post-processing pass:

* **Screen-Space Ambient Occlusion (SSAO):** Based on a modified version of Crytek's technique, the engine utilizes a hemisphere sampling kernel to simulate realistic, soft contact shadows in corners, tight crevices, and overlapping geometries.

* **Physcially based Bloom:** Implements a threshold-free downsampling and upsampling pyramid chain based on the technique developed by **Sledgehammer Games** (for *Call of Duty: Advanced Warfare*). This simulates natural lens light-scattering and soft glow across the entire scene without temporal flickering.

## Technical Details

With the mathematical foundations established, the following sections focus on the practical realization of the engine — highlighting the technologies used, the external libraries integrated and **aiming to give a baseline understanding of the underlying software architecture** that brings the PBR pipeline to life.

### Technology Stack

* **Language:** C++20

* **Graphics API:** OpenGL 4.3 (*Core Profile*)

* **Build System:** CMake

### Dependencies

Besides the core technologies described above, the engine integrates the following third-party libraries to handle window management, asset loading and mathematics:

* [**GLFW**](https://www.glfw.org/) and [**GLAD:**](https://github.com/Dav1dde/glad) Window management, OpenGL context initialization and retrieving driver-specific OpenGL functions.

* [**GLM:**](https://github.com/g-truc/glm) A popular mathematics library for C++.

* [**Assimp:**](https://github.com/assimp/assimp) A powerful library for complex 3D model and scene loading.

* [**stb_image:**](https://github.com/nothings/stb/blob/master/stb_image.h) Lightweight single-header image loading library for HDR and LDR textures.

>***Note:***
All of the external libraries are included in the repository as is — as such, ***all credit goes to their respective authors***.

## Architectural Overview

The engine is built on a modular architecture that prioritizes clean abstractions and performance optimization during runtime. The system can be divided into three primary pillars:

### 1. Resource & Data Management

The engine utilizes a specialized hierarchy to manage GPU resources and their associated data:

* **Uniform Management:** The `UniformSource` interface serves as the foundation for data-providing entities such as `Model`, `Material`, `Mesh`, `Light` and `Camera`. Each specialization handles its own uniform data independently.

* **Texture Abstraction:** A simple and thin `Texture` master layer governs all image-based resources. This includes:

    * **File-based textures:** `Texture2D` and `TextureCube` is available for standard assets, with a special `HDRTexture` available for loading HDR equirectangular maps.

    * **Render targets:** A `RenderTexture` interface (*with 2D and Cube specializations*) designed for `Framebuffer` and `FramebufferCube` objects.

* **PBR Pre-computation:** The engine leverages a flexible Framebuffer system to handle essential pre-calculation passes required for the PBR pipeline — e.g. ***Irradiance*** and ***Pre-filter maps***.

### 2. The Rendering Pipeline

The pipeline is designed to maximize efficiency through high-level abstractions:

* **Shader & Uniform Logic:** Shader management is handled via `ShaderLoader` and `ShaderProgram` classes, while the `UniformRegistry` ensures efficient variable tracking and storage for individual programs.

* **Observer-based Updates:** The ensure a clean decoupling between individual uniform data and shader logic, the engine employs an ***Observer-like registration pattern***. `UniformSource` implementations have the option to subscribe to their respective `ShaderProgram` objects — *which can be any number of programs* — during initialization; the shader then automatically triggers uniform data updates and uploads to the GPU during rendering.

* **Batch Rendering:** Performance is further optimized via the `ShaderBatch` system. By grouping draw calls by shader state, the engine minimizes expensive GPU state changes, significantly improving framerate throughput.

### 3. Scene & Camera System

* **Navigation:** A standard fly-by FPS `Camera` provides intuitive maneuvering within the environment.

* **Scene Configuration:** For enhanced modularity, the engine utilizes a ***Builder-like pattern*** (`SceneBuilder`), which allows for rapid and flexible swapping between different `Scene` configurations and lighting setups — even at runtime.

## Compilation and Execution

### Prerequisites

To build and run this engine, you need:

* **Compiler:** A **C++20** compliant compiler (e.g. MSVC 19.29+, GCC 10+ or Clang 10+).

* **Graphics API:** A GPU driver with **OpenGL 4.3+** support.

* **Build System:** **CMake** (version 3.22 or higher).

* **System Dependencies (Linux only):** Development headers for X11 and OpenGL. 

### How to Build and Run

First and foremost, clone the repository (if you haven't already):

```bash
git clone https://github.com/horvathbalazsgyorgy/BMEIIT-Project-Laboratory.git
cd BMEIIT-Project-Laboratory
```

#### Windows

The easiest way to build on Windows is using Visual Studio or CMake via CLI:

```bash
cmake -B build -S .
cmake --build build --config Release
```

The executable will be located in the `build/bin/Release` folder.

#### Linux

First, ensure you have the necessary development headers installed:

```bash
sudo apt-get update
sudo apt-get install -y libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev
```

Then, generate and build:

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

You can run the application with the following command:

```bash
cd build/bin
./ProjectLaboratory
```

## Documentation (Work in Progress)

At the time of this writing, formal documentation is not yet available — *neither English nor Hungarian*. However, in order to give this engine a try, you can find a quick user manual detailing the control mechanics below:

* **Movement:** You can move around using the ***WASD*** keys. You can also look around with your mouse while holding down either the ***Left*** or ***Right*** mouse button.

* **Scene Manipulation:** You can directly control and customize the look of your scene with the following keys:

    * ***M*** - Change the shading mode. Shading mode is either **Blinn-Phong** or **PBR**.
    * ***I*** - Controls whether the scene uses Irradiance Map or Spherical Harmonics for IBL.
    * ***P*** - When in PBR mode, switch between the **Beckmann distribution** coupled with the **Kelemen-Szirmay geometry function** and the standard **GGX approximations**.
    * ***O*** - Enable/disable SSAO.
    * ***B*** - Enable/disable Bloom.

* **Cinematic Camera:** By pressing ***C***, the program will switch to a cinematic camera that repeatedly spans throughout the scene, as seen in the demo video above. Pressing ***C*** again will switch back to the regular camera.
 
## License

As of $2026.05.10$, this project is licensed under the **MIT License** — see the [LICENSE](./LICENSE) file for details.
