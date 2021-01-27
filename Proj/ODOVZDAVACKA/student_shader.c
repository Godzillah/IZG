/*!
 * @file
 * @brief This file contains implemenation of phong vertex and fragment shader.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <assert.h>
#include <math.h>

#include <student/gpu.h>
#include <student/student_shader.h>
#include <student/uniforms.h>

/// \addtogroup shader_side Úkoly v shaderech
/// @{

void phong_vertexShader(GPUVertexShaderOutput *const      output,
                        GPUVertexShaderInput const *const input,
                        GPU const                         gpu) {
  /// \todo Naimplementujte vertex shader, který transformuje vstupní vrcholy do
  /// clip-space.<br>
  /// <b>Vstupy:</b><br>
  /// Vstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu ve
  /// world-space (vec3) a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).<br>
  /// <b>Výstupy:</b><br>
  /// Výstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu (vec3)
  /// ve world-space a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).
  /// Výstupní vrchol obsahuje pozici a normálu vrcholu proto, že chceme počítat
  /// osvětlení ve world-space ve fragment shaderu.<br>
  /// <b>Uniformy:</b><br>
  /// Vertex shader by měl pro transformaci využít uniformní proměnné obsahující
  /// view a projekční matici.
  /// View matici čtěte z uniformní proměnné "viewMatrix" a projekční matici
  /// čtěte z uniformní proměnné "projectionMatrix".
  /// Zachovejte jména uniformních proměnných a pozice vstupních a výstupních
  /// atributů.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Využijte vektorové a maticové funkce.
  /// Nepředávajte si data do shaderu pomocí globálních proměnných.
  /// Pro získání dat atributů použijte příslušné funkce vs_interpret*
  /// definované v souboru program.h.
  /// Pro získání dat uniformních proměnných použijte příslušné funkce
  /// shader_interpretUniform* definované v souboru program.h.
  /// Vrchol v clip-space by měl být zapsán do proměnné gl_Position ve výstupní
  /// struktuře.<br>
  /// <b>Seznam funkcí, které jistě použijete</b>:
  ///  - gpu_getUniformsHandle()
  ///  - getUniformLocation()
  ///  - shader_interpretUniformAsMat4()
  ///  - vs_interpretInputVertexAttributeAsVec3()
  ///  - vs_interpretOutputVertexAttributeAsVec3()
  (void)output;
  (void)input;
  (void)gpu;

  Vec4 positionOfVector;
  Mat4 eMatrix;

  Uniforms const gpuUniforms = gpu_getUniformsHandle(gpu);


  UniformLocation matrix = getUniformLocation(gpu, "viewMatrix");
  UniformLocation projectMatrix = getUniformLocation(gpu, "projectionMatrix");

  Mat4 const *viewMatrix = shader_interpretUniformAsMat4(gpuUniforms, matrix);
  Mat4 const *projectionMatrix = shader_interpretUniformAsMat4(gpuUniforms, projectMatrix);

  Vec3 const*const inPosition = vs_interpretInputVertexAttributeAsVec3(gpu, input, 0);    // Gpu <-> in(vertex) <--> idx vertex attr
  Vec3 const*const inNormal = vs_interpretInputVertexAttributeAsVec3(gpu, input, 1);        // Gpu <-> in(vertex) <--> idx vertex attr

  copy_Vec3Float_To_Vec4(&positionOfVector, inPosition, 1.f);

  multiply_Mat4_Mat4(&eMatrix, projectionMatrix, viewMatrix);
  multiply_Mat4_Vec4(&output->gl_Position, &eMatrix, &positionOfVector);

  Vec3 *const outPosition = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 0);
  Vec3 *const outNormal = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 1);

  init_Vec3(outPosition, inPosition->data[0], inPosition->data[1], inPosition->data[2]);
  init_Vec3(outNormal, inNormal->data[0], inNormal->data[1], inNormal->data[2]);
}

void phong_fragmentShader(GPUFragmentShaderOutput *const      output,
                          GPUFragmentShaderInput const *const input,
                          GPU const                           gpu) {
  /// \todo Naimplementujte fragment shader, který počítá phongův osvětlovací
  /// model s phongovým stínováním.<br>
  /// <b>Vstup:</b><br>
  /// Vstupní fragment by měl v nultém fragment atributu obsahovat
  /// interpolovanou pozici ve world-space a v prvním
  /// fragment atributu obsahovat interpolovanou normálu ve world-space.<br>
  /// <b>Výstup:</b><br>
  /// Barvu zapište do proměnné color ve výstupní struktuře.<br>
  /// <b>Uniformy:</b><br>
  /// Pozici kamery přečtěte z uniformní proměnné "cameraPosition" a pozici
  /// světla přečtěte z uniformní proměnné "lightPosition".
  /// Zachovejte jména uniformních proměnný.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Dejte si pozor na velikost normálového vektoru, při lineární interpolaci v
  /// rasterizaci může dojít ke zkrácení.
  /// Zapište barvu do proměnné color ve výstupní struktuře.
  /// Shininess faktor nastavte na 40.f
  /// Difuzní barvu materiálu nastavte podle normály povrchu.
  /// V případě, že normála směřuje kolmo vzhůru je difuzní barva čistě bílá.
  /// V případě, že normála směřuje vodorovně nebo dolů je difuzní barva čiště zelená.
  /// Difuzní barvu spočtěte lineární interpolací zelené a bíle barvy pomocí interpolačního parameteru t.
  /// Interpolační parameter t spočtěte z y komponenty normály pomocí t = y*y (samozřejmě s ohledem na negativní čísla).
  /// Spekulární barvu materiálu nastavte na čistou bílou.
  /// Barvu světla nastavte na bílou.
  /// Nepoužívejte ambientní světlo.<br>
  /// <b>Seznam funkcí, které jistě využijete</b>:
  ///  - shader_interpretUniformAsVec3()
  ///  - fs_interpretInputAttributeAsVec3()
  (void)output;
  (void)input;
  (void)gpu;

  Uniforms const uniformsHandle = gpu_getUniformsHandle(gpu);

  Vec3 const* camPosition = shader_interpretUniformAsVec3(uniformsHandle, getUniformLocation(gpu, "cameraPosition"));     // ---> CAMERA <---
  Vec3 const* lightPosition = shader_interpretUniformAsVec3(uniformsHandle, getUniformLocation(gpu, "lightPosition")); // ---> LIGHT <---

  Vec3 const *positionOfFragment = fs_interpretInputAttributeAsVec3(gpu, input, 0);             // interpolated position
  Vec3 const *normalOfFragment = fs_interpretInputAttributeAsVec3(gpu, input, 1);               // interpolated normal

  Vec3 directionOfLightImpact;                                                                  // direction light impact

  sub_Vec3(&directionOfLightImpact, lightPosition, positionOfFragment);                         // calculate direction of light impact
  normalize_Vec3(&directionOfLightImpact, &directionOfLightImpact);                             // normalization of the direction of light impact

  float vectorMulDiffusion = dot_Vec3(normalOfFragment, &directionOfLightImpact);
  float t = dot_Vec3(normalOfFragment,  positionOfFragment );


  if(vectorMulDiffusion > 0 && vectorMulDiffusion <= 1 ) {
	    Vec3 difColor;                                                      // defining all vectors...
        Vec3 reflection;
        Vec3 directionOfCamera;
        Vec3 spectColor;
        Vec3 ultimateColor;

        init_Vec3(&difColor, 0.f, 1.f, 0.f);                                // RGB -> middle is green
        difColor.data[1] *= vectorMulDiffusion;                             // green has to multiplicate.... others does not matter
        reflect(&reflection, &directionOfLightImpact, normalOfFragment);
        normalize_Vec3(&reflection, &reflection);
        sub_Vec3(&directionOfCamera, camPosition, positionOfFragment);      //view direction
        normalize_Vec3(&directionOfCamera, &directionOfCamera);

        float vectorMulSpec = dot_Vec3(&directionOfCamera, &reflection);
        float spectConst = powf(vectorMulSpec, 40.0);

        init_Vec3(&spectColor, 1.f, 1.f, 1.f);                              // camera white color
        multiply_Vec3_Float(&spectColor, &spectColor, spectConst);


        add_Vec3(&ultimateColor, &difColor, &spectColor);
        copy_Vec3Float_To_Vec4(&output->color, &ultimateColor, 1.0);
        output->depth = input->depth;

  }

  if(t > 0.15 && t <= 0.4){
        Vec3 difColor;
        Vec3 spectColor;
        Vec3 ultimateColor;

        init_Vec3(&difColor, 1.f, 1.f, 1.f);                            // white color
        difColor.data[0] *= vectorMulDiffusion;                         // all factors has to muliplicate....
        difColor.data[1] *= vectorMulDiffusion;
        difColor.data[2] *= vectorMulDiffusion;

        add_Vec3(&ultimateColor, &difColor, &spectColor);
        copy_Vec3Float_To_Vec4(&output->color, &ultimateColor, 1.0);
        output->depth = input->depth;
    }

}

/// @}
