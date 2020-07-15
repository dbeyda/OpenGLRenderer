#shader vertex#version 410 corelayout(location = 0) in vec4 position;void main(){    gl_Position = vec4(position.xy, 0.0f, 1.0f);};#shader fragment#version 410 corelayout(location = 0) out vec4 color;// texture samplersuniform struct Samplers{    sampler2D MainSceneColor;    sampler2D MainSceneDepth;    sampler2D Coc;} samplers;uniform vec2 viewportSize;uniform float bleedingMult;uniform float bleedingBias;uniform float focalPlane;void main(){    vec2 uv = gl_FragCoord.xy / viewportSize;    vec2 pixelSize = vec2(1.0) / viewportSize;    float aspectRatio = viewportSize.x / viewportSize.y;    // octogonal samples    vec2 offsets[37] = vec2[37](            vec2(-1, 3), vec2(0, 3), vec2(1, 3),        vec2(-2, 2), vec2(-1, 2), vec2(0, 2), vec2(1, 2), vec2(2, 2),    vec2(-3, 1), vec2(-2, 1), vec2(-1, 1), vec2(0, 1), vec2(1, 1), vec2(2, 1), vec2(3, 1),    vec2(-3, 0), vec2(-2, 0), vec2(-1, 0), vec2(0, 0), vec2(1, 0), vec2(2, 0), vec2(3, 0),    vec2(-3, -1), vec2(-2, -1), vec2(-1, -1), vec2(0, -1), vec2(1, -1), vec2(2, -1), vec2(3, -1),        vec2(-2, -2), vec2(-1, -2), vec2(0, -2), vec2(1, -2), vec2(2, -2),            vec2(-1, -3), vec2(0, -3), vec2(1, 3)    );        vec4 centerColor = texture(samplers.MainSceneColor, uv);    float centerZ = texture(samplers.MainSceneDepth, uv).r;    float centerCoc = texture(samplers.Coc, uv).r;        vec4 finalColor = centerColor;    float totalWeight = 1.0;    //for (int i = 0; i < 37; i++)    //{    //    vec2 direction = offsets[i]/3.0;    //    float distance = centerCoc;    //    float sampleCoc = texture(samplers.Coc, uv + distance * direction).r;    //    float sampleZ = texture(samplers.MainSceneDepth, uv + distance * direction).r;    //        //    float weight = sampleZ < centerZ ? sampleCoc * bleedingMult : 1.0;    //    weight = (centerCoc > sampleCoc + bleedingBias) ? weight : 1.0;    //    weight = clamp(weight, 0.0, 1.0);    //        //    finalColor += texture(samplers.MainSceneColor, uv + distance * direction) * weight;    //    totalWeight += weight;    //}    //finalColor /= totalWeight;    for (int i = 0; i < 37; i++)    {        vec2 sampleOffset = 0.01 * offsets[i] / 3.0;        // account for aspect ratio        sampleOffset.x /= aspectRatio;        vec2 sampleCoords = uv + sampleOffset;        float dist = abs(distance(uv, sampleCoords));        float sampleCoc = texture(samplers.Coc, sampleCoords).r;        float sampleZ = texture(samplers.MainSceneDepth, sampleCoords).r;        float weight = 1;            /*((focalPlane <= sampleZ) && (sampleCoc > dist))            || ((focalPlane > sampleZ) (sampleZ <= centerZ))*/        if ((sampleCoc > dist)            &&(sampleCoc >= centerCoc)            &&(sampleZ <= centerZ + 0.003))        {            finalColor += texture(samplers.MainSceneColor, sampleCoords);            totalWeight += weight;        }        else        {            finalColor += finalColor / totalWeight;            totalWeight += totalWeight / (i + 1);        }        //finalColor = finalColor + (sampleCoc > dist ? texture(samplers.MainSceneColor, sampleCoords) : finalColor/(i+1));        /*        float weight = sampleZ < centerZ ? sampleCoc * bleedingMult : 1.0;        weight = (centerCoc > sampleCoc + bleedingBias) ? weight : 1.0;        weight = clamp(weight, 0.0, 1.0);        */    }    finalColor /= totalWeight;    color = finalColor;    // for debugging, to see the Coc    //color = vec4(vec3(centerCoc), 1.0);};