#shader vertex#version 410 corelayout(location = 0) in vec4 position;void main(){    gl_Position = vec4(position.xy, 0.0f, 1.0f);};#shader fragment#version 410 corelayout(location = 0) out vec4 color;// texture samplersuniform struct Samplers{    sampler2D MainSceneColor;    sampler2D MainSceneDepth;} samplers;uniform vec2 viewportSize;void main(){    vec2 uv = gl_FragCoord.xy / viewportSize;    color = texture(samplers.MainSceneColor, uv);};