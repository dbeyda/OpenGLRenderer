#shader vertex#version 410 corelayout(location = 0) in vec4 position;void main(){    // pass a fullscreen quad to the fragment shader, without any transformations    gl_Position = vec4(position.xy, 0.0f, 1.0f);};#shader fragment#version 410 corelayout(location = 0) out vec4 color;// texture samplersuniform struct Samplers{    sampler2D MainSceneColor;               // Scene color buffer rendered offscreen    sampler2D MainSceneDepth;               // Scene depth buffer    sampler2D Coc;                          // Circle of Confusion texture} samplers;uniform vec2 viewportSize;                  // resolution of the ouput buffervoid main(){    vec2 uv = gl_FragCoord.xy / viewportSize;    vec2 pixelSize = vec2(1.0) / viewportSize;    float aspectRatio = viewportSize.x / viewportSize.y;    // octogonal samples    vec2 offsets[37] = vec2[37](                                vec2(-1, 3), vec2(0, 3), vec2(1, 3),                   vec2(-2, 2), vec2(-1, 2), vec2(0, 2), vec2(1, 2), vec2(2, 2),      vec2(-3, 1), vec2(-2, 1), vec2(-1, 1), vec2(0, 1), vec2(1, 1), vec2(2, 1), vec2(3, 1),      vec2(-3, 0), vec2(-2, 0), vec2(-1, 0), vec2(0, 0), vec2(1, 0), vec2(2, 0), vec2(3, 0),   vec2(-3, -1), vec2(-2, -1), vec2(-1, -1), vec2(0, -1), vec2(1, -1), vec2(2, -1), vec2(3, -1),                 vec2(-2, -2), vec2(-1, -2), vec2(0, -2), vec2(1, -2), vec2(2, -2),                               vec2(-1, -3), vec2(0, -3), vec2(1, 3)    );        vec4 centerColor = texture(samplers.MainSceneColor, uv);    float centerZ = texture(samplers.MainSceneDepth, uv).r;    float centerCoc = texture(samplers.Coc, uv).r;        vec4 finalColor = centerColor;    float totalWeight = 1.0;    for (int i = 0; i < 37; i++)    {        vec2 sampleOffset = 0.01 * offsets[i] / 3.0;        sampleOffset.x /= aspectRatio;         // account for aspect ratio        vec2 sampleCoords = uv + sampleOffset;                float dist = abs(distance(uv, sampleCoords));        float sampleCoc = texture(samplers.Coc, sampleCoords).r;        float sampleZ = texture(samplers.MainSceneDepth, sampleCoords).r;        float weight = 1;                if ((sampleCoc > dist)                  // Coc of samples reaches current pixel            &&(sampleCoc >= centerCoc)          // sample is more blurred than current pixel            &&(sampleZ <= centerZ + 0.003))     // sample is between current pixel and camera        {            finalColor += texture(samplers.MainSceneColor, sampleCoords);            totalWeight += weight;        }        else        {            finalColor += finalColor / totalWeight;            totalWeight += totalWeight / (i + 1);        }    }    finalColor /= totalWeight;    color = finalColor;};