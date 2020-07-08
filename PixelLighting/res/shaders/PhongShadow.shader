#shader vertex#version 410 corelayout(location = 0) in vec4 vtPosition;   /* vertex position */layout(location = 1) in vec4 vtNormal;     /* vertex normal   */layout(location = 2) in vec2 vtTexCoord;   /* vertex texture coordinate */layout(location = 3) in vec3 vtTangent;    /* vertex tangent (calculated on obj load) */uniform mat4 u_MVP;                        /* object MVP matrix */uniform vec4 u_globalLightColor;           /* global light color */uniform float u_globalLightStrength;       /* global light strength */uniform mat4 u_DepthMvp;                   /* mvp for the depth map */out VS_OUT{    vec4 position;                         /* vertex position */    vec4 normal;                           /* vertex normal */    vec2 texCoord;                         /* vertex texture coordinate */    vec3 tangent;                          /* vertex tangent */    vec4 globalLight;                      /* global light component */    vec4 shadowMapCoords;                  /* shadow map coords */} vs_out;void main(){    vs_out.position = vtPosition;    vs_out.normal = normalize(vtNormal);    vs_out.texCoord = vtTexCoord;    vs_out.tangent = vtTangent;    vs_out.globalLight = u_globalLightStrength * u_globalLightColor;    vs_out.globalLight.a = 1;    vs_out.shadowMapCoords = u_DepthMvp * vtPosition;    gl_Position = u_MVP * vtPosition;};#shader fragment#version 410 core#define SAMPLES_COUNT 32#define INV_SAMPLES_COUNT (1.0f / SAMPLES_COUNT)  in VS_OUT{    vec4 position;                       /* vertex position */    vec4 normal;                         /* vertex normal */    vec2 texCoord;                       /* vertex texture coordinate */    vec3 tangent;                        /* vertex tangent */    vec4 globalLight;                    /* global light component */    vec4 shadowMapCoords;                  /* shadow map coords */} fs_in;// texture samplersuniform struct Samplers{    sampler2D AmbientTexture;            /* sampler for material map_ka */    sampler2D DiffuseTexture;            /* sampler for material map_kd */    sampler2D BumpTexture;               /* sampler for material map_bump */} samplers;// texture flags// material propertiesuniform struct Material{    float mshi;                         /* material shiness */    float ka;    float ks;    float kd;    bool hasAmbientTexture;           /* true when material has map_ka */    bool hasDiffuseTexture;           /* true when material has map_kd */    bool hasBumpTexture;              /* true when material has map_bump */} material;// light propertiesuniform struct Light{    float kc;                           /* constant term for katt */    float kl;                           /* linear term for katt */    float kq;                           /* quadratic term for katt */    float spotLightCutoff;              /* cutoff angle for the spotlight cone */    int sexp;                           /* spotlight beam angle exponent*/    vec3 cameraSpacePos;                /* light position in camera space */    vec3 cameraSpaceDir;                /* light direction in camera space */    vec3 color;                         /* light source color */} light;// shadow map propertiesuniform struct ShadowMap{    float blur;                            /* shadow blurriness */    float samplesCount;                    /* number of samples for each fragment */    float jitterMapWidth;                  /* jitter map width */    float jitterMapHeight;                 /* jitter map height */    float jitterRadius;                    /* concentration of the jitters */    sampler3D JitOffsets;                  /* sampler for jittered offsets */    sampler2DShadow DepthMap;              /* sampler for shadowmap depth map */} shadowMap;// transformation matricesuniform mat4 u_MV;                       /* object model view matrix */uniform mat4 u_invTransMV;               /* inverse transpose of the object model view matrix */layout(location = 0) out vec4 color;     /* output color */vec3 expand(vec3 v){    return (v - 0.5) * 2;}void main(){    /* TBN matrix */    vec3 T = normalize(mat3(u_MV) * normalize(fs_in.tangent.xyz));    vec3 N = normalize(mat3(u_MV) * normalize(fs_in.normal.xyz));    vec3 B = normalize(cross(N, T));    mat3 TBN = mat3(T, B, N);    /* camera direction in camera space */    vec3 cameraSpaceEye = vec3(0, 0, 0);    vec3 cameraSpaceFragPos = vec3(u_MV * fs_in.position);    vec3 cameraSpaceViewerDir = normalize(cameraSpaceEye - cameraSpaceFragPos);    /* light vector relative to fragment */    vec3 cameraSpaceFragLightDir = normalize(light.cameraSpacePos - cameraSpaceFragPos);    /* calculating lighting properties in camera space */    float d = distance(light.cameraSpacePos, cameraSpaceFragPos);    float katt = 1 / (light.kc + light.kl * d + light.kq * d * d);    float spotLightCos = dot(-cameraSpaceFragLightDir, light.cameraSpaceDir);    float kspot = pow(max(spotLightCos, 0), light.sexp);    /* calculating normal in camera space */    vec3 cameraSpaceFragNormal = normalize(vec3(u_invTransMV * vec4(fs_in.normal.xyz, 0)));    vec3 cameraSpaceNormal;    if (material.hasBumpTexture)    {        vec3 bumpNormal = expand(texture(samplers.BumpTexture, fs_in.texCoord).xyz);        cameraSpaceNormal = normalize(TBN * bumpNormal);    }    else        cameraSpaceNormal = cameraSpaceFragNormal;    /* diffuse lighting in camera space */    float NL = dot(cameraSpaceNormal, cameraSpaceFragLightDir);    float difIntensity = max(NL, 0.0);    vec4 diffuse = vec4(difIntensity);    if (material.hasDiffuseTexture)        diffuse *= texture(samplers.DiffuseTexture, fs_in.texCoord);    diffuse *= vec4(light.color, 1);    /* ambient lighting */    vec4 ambient = vec4(1);    if (material.hasAmbientTexture)        ambient = texture(samplers.AmbientTexture, fs_in.texCoord);    ambient *= vec4(light.color, 1);    /* specular lighting in camera space */    float specular = 0;    if (dot(cameraSpaceNormal, cameraSpaceFragLightDir) > 0)    {        vec3 refl = normalize(reflect(-cameraSpaceFragLightDir, cameraSpaceNormal));        specular = pow(max(dot(refl, cameraSpaceFragLightDir), 0), material.mshi);    }    /* shadow mapping - using disk filter with jittered offsets*/    float visibility=0.0;    vec4 shadowCoords = fs_in.shadowMapCoords;    // Slope-Scaled Depth Bias    shadowCoords.z -= (1 - dot(cameraSpaceFragNormal, cameraSpaceFragLightDir)) * 0.05 + 0.001;       vec2 jitSampleCoord = gl_FragCoord.xy;   jitSampleCoord.x /= shadowMap.jitterMapWidth;   jitSampleCoord.y /= shadowMap.jitterMapHeight;      // first, sampling only outer radius of the disk    float smallSample = shadowMap.samplesCount / shadowMap.jitterRadius;    float propOuterCircle = 1 / shadowMap.jitterRadius;   for (float i = 0; i < propOuterCircle; i+= 1.0 / shadowMap.samplesCount)   {       vec4 offsets = vec4(texture(shadowMap.JitOffsets, vec3(jitSampleCoord, 1 - i)));       offsets = offsets * 2 - 1;       vec4 shadowSampleCoords = shadowCoords;       shadowSampleCoords.xy += offsets.rg * shadowMap.blur;       visibility += textureProj(shadowMap.DepthMap, shadowSampleCoords.xyzw) / smallSample;   }      // if first sampling isn't totaly visible or totaly blocked, sample the rest   if ((visibility - 1) * visibility != 0)   {       visibility *= smallSample / shadowMap.samplesCount;       for (float i = 0; i < 1-propOuterCircle; i += 1.0 / shadowMap.samplesCount)       {           vec4 offsets = vec4(texture(shadowMap.JitOffsets, vec3(jitSampleCoord, i)));           offsets = offsets * 2 - 1;           vec4 shadowSampleCoords = shadowCoords;           shadowSampleCoords.xy += offsets.rg * shadowMap.blur;           visibility += textureProj(shadowMap.DepthMap, shadowSampleCoords.xyzw) / shadowMap.samplesCount;       }   }    /* Set final pixel color */    color = fs_in.globalLight;    if (dot(cameraSpaceFragNormal, cameraSpaceViewerDir) > 0)        color += (material.ka * ambient) + (visibility * material.kd * diffuse + visibility * material.ks * specular) * katt * kspot;    color.w = 1;};/*COMMENTS    1.  Light.z check (cameraSpaceFragLightDir.z > 0?) was not done for the diffuse lighting and neither for the specular lighting,        because it was giving strange results on the sphere. Depending on the lighting of the sphere, there are        regions with LightDir.z < 0 that should receive light.*/