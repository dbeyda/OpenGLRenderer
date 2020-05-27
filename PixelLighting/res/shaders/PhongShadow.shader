#shader vertex#version 410 corelayout(location = 0) in vec4 vtPosition;   /* vertex position */layout(location = 1) in vec4 vtNormal;     /* vertex normal   */layout(location = 2) in vec2 vtTexCoord;   /* vertex texture coordinate */layout(location = 3) in vec3 vtTangent;    /* vertex tangent (calculated on obj load) */uniform mat4 u_MVP;                        /* object MVP matrix */uniform vec4 u_globalLightColor;           /* global light color */uniform float u_globalLightStrength;       /* global light strength */uniform mat4 u_DepthMvp;                   /* mvp for the depth map */out VS_OUT{    vec4 position;                         /* vertex position */    vec4 normal;                           /* vertex normal */    vec2 texCoord;                         /* vertex texture coordinate */    vec3 tangent;                          /* vertex tangent */    vec4 globalLight;                      /* global light component */    vec4 shadowMapCoords;                  /* shadow map coords */} vs_out;void main(){    vs_out.position = vtPosition;    vs_out.normal = normalize(vtNormal);    vs_out.texCoord = vtTexCoord;    vs_out.tangent = vtTangent;    vs_out.globalLight = u_globalLightStrength * u_globalLightColor;    vs_out.globalLight.a = 1;    vs_out.shadowMapCoords = u_DepthMvp * vtPosition;    gl_Position = u_MVP * vtPosition;};#shader fragment#version 410 core#define SAMPLES_COUNT 64#define INV_SAMPLES_COUNT (1.0f / SAMPLES_COUNT)  in VS_OUT{    vec4 position;                       /* vertex position */    vec4 normal;                         /* vertex normal */    vec2 texCoord;                       /* vertex texture coordinate */    vec3 tangent;                        /* vertex tangent */    vec4 globalLight;                    /* global light component */    vec4 shadowMapCoords;                  /* shadow map coords */} fs_in;// texture samplersuniform struct Samplers{    sampler2D AmbientTexture;            /* sampler for material map_ka */    sampler2D DiffuseTexture;            /* sampler for material map_kd */    sampler2D BumpTexture;               /* sampler for material map_bump */    sampler2DShadow DepthMap;                  /* sampler for material map_bump */} samplers;// texture flags// material propertiesuniform struct Material{    float mshi;                         /* material shiness */    float ka;    float ks;    float kd;    bool hasAmbientTexture;           /* true when material has map_ka */    bool hasDiffuseTexture;           /* true when material has map_kd */    bool hasBumpTexture;              /* true when material has map_bump */} material;// light propertiesuniform struct Light{    float kc;                           /* constant term for katt */    float kl;                           /* linear term for katt */    float kq;                           /* quadratic term for katt */    int sexp;                           /* spotlight beam angle exponent*/    vec3 cameraSpacePos;                /* light position in camera space */    vec3 cameraSpaceDir;                /* light direction in camera space */} light;// transformation matricesuniform mat4 u_MV;                       /* object model view matrix */uniform mat4 u_invTransMV;               /* inverse transpose of the object model view matrix */uniform mat4 u_lightVp;// other uniformsuniform vec2 offsets[SAMPLES_COUNT];layout(location = 0) out vec4 color;     /* output color */vec3 expand(vec3 v){    return (v - 0.5) * 2;}void main(){    /* TBN matrix */    vec3 T = normalize(mat3(u_MV) * normalize(fs_in.tangent.xyz));    vec3 N = normalize(mat3(u_MV) * normalize(fs_in.normal.xyz));    vec3 B = normalize(cross(N, T));    mat3 TBN = mat3(T, B, N);    /* camera direction in camera space */    vec3 cameraSpaceEye = vec3(0, 0, 0);    vec3 cameraSpaceFragPos = vec3(u_MV * fs_in.position);    vec3 cameraSpaceViewerDir = normalize(cameraSpaceEye - cameraSpaceFragPos);    /* light vector relative to fragment */    vec3 cameraSpaceFragLightDir = normalize(light.cameraSpacePos - cameraSpaceFragPos);    /* calculating lighting properties in camera space */    float d = distance(light.cameraSpacePos, cameraSpaceFragPos);    float katt = 1 / (light.kc + light.kl * d + light.kq * d * d);    float kspot = pow(max(dot(-cameraSpaceFragLightDir, light.cameraSpaceDir), 0), light.sexp);    /* calculating normal in camera space */    vec3 cameraSpaceFragNormal = normalize(vec3(u_invTransMV * vec4(fs_in.normal.xyz, 0)));    vec3 cameraSpaceNormal;    if (material.hasBumpTexture)    {        vec3 bumpNormal = expand(texture(samplers.BumpTexture, fs_in.texCoord).xyz);        cameraSpaceNormal = normalize(TBN * bumpNormal);    }    else        cameraSpaceNormal = cameraSpaceFragNormal;    /* diffuse lighting in camera space */    float difIntensity = dot(cameraSpaceNormal, cameraSpaceFragLightDir) > 0 ? max(dot(cameraSpaceNormal, cameraSpaceFragLightDir), 0.0) : 0;    vec4 diffuse = vec4(difIntensity);    if (material.hasDiffuseTexture)        diffuse *= texture(samplers.DiffuseTexture, fs_in.texCoord);    /* ambient lighting */    vec4 ambient = vec4(1);    if (material.hasAmbientTexture)        ambient = texture(samplers.AmbientTexture, fs_in.texCoord);    /* specular lighting in camera space */    float specular = 0;    if (dot(cameraSpaceNormal, cameraSpaceFragLightDir) > 0)    {        vec3 refl = normalize(reflect(-cameraSpaceFragLightDir, cameraSpaceNormal));        specular = pow(max(dot(refl, cameraSpaceFragLightDir), 0), material.mshi);    }    /* shadow mapping */    // Shadow Mapping using regular grid as filter    float fsize = 0.05;    float visibility;    vec4 shadowCoords = fs_in.shadowMapCoords;    //fsize *= 1/(10*dFdx(shadowCoords.x));    // Slope-Scaled Depth Bias    shadowCoords.z -= (1 - dot(cameraSpaceFragNormal, cameraSpaceFragLightDir)) * 0.05 + 0.001;//    shadowCoords.z -= 0.001;    vec4 sm;    int smallSample = SAMPLES_COUNT / 4;       visibility = 0.0;    for (int i=0; i < smallSample; i++)    {        sm = shadowCoords;        sm.xy = shadowCoords.xy + offsets[SAMPLES_COUNT - 1 - i] * fsize;        visibility += textureProj(samplers.DepthMap, sm.xyzw) / float(smallSample);    }    if ((visibility - 1) * visibility != 0)    {        visibility *= smallSample / float(SAMPLES_COUNT);        for(int i=0; i<SAMPLES_COUNT - smallSample; i++)        {            sm = shadowCoords;            sm.xy = shadowCoords.xy + offsets[i] * fsize;            visibility += textureProj(samplers.DepthMap, sm.xyzw)/float(SAMPLES_COUNT);        }    }    /*    // Shadow Mapping using opengl COMPARE_R_TO_TEXTURE    float visibility = 1.0;    vec4 shadowCoords = u_DepthMvp * fs_in.position;    shadowCoords.z -= 0.0005;    visibility = textureProj(samplers.DepthMap, shadowCoords.xyzw);    */    /*    // Shadow Mapping making my own comparision    vec4 shadowCoords = u_DepthMvp * fs_in.position;    shadowCoords = shadowCoords / shadowCoords.w;    float closestDepth = texture(samplers.DepthMap, shadowCoords.xy).r;    float currentDepth = shadowCoords.z - 0.0005;    visibility = currentDepth > closestDepth ? 0.2 : 1.0;    */    /* Set final pixel color */    color = fs_in.globalLight;    if (dot(cameraSpaceFragNormal, cameraSpaceViewerDir) > 0)        color += (material.ka * ambient) + (visibility * material.kd * diffuse + visibility * material.ks * specular) * katt * kspot;    color.w = 1;};/*COMMENTS    1.  Light.z check (cameraSpaceFragLightDir.z > 0?) was not done for the diffuse lighting and neither for the specular lighting,        because it was giving strange results on the sphere. Depending on the lighting of the sphere, there are        regions with LightDir.z < 0 that should receive light.*/