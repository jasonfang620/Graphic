#version 150 core

uniform sampler2D depthTex;
uniform sampler2D depthTex_Shadow;
uniform sampler2D normTex;

uniform float has_shadow = 0.0;
uniform mat4 lightprojView;

uniform vec2 pixelSize;
uniform vec3 cameraPos;

uniform float lightRadius;
uniform vec3 lightPos;
uniform vec4 lightColour;

in mat4 inverseProjView;
out vec4 gl_FragColor[2];

void main ( void ) {
  vec3 pos = vec3 ((gl_FragCoord.x * pixelSize.x) ,(gl_FragCoord.y * pixelSize.y), 0.0);
  pos.z = texture (depthTex , pos.xy).r;

  vec3 normal = normalize (texture(normTex,pos.xy).xyz *2.0 - 1.0);

  vec4 clip = inverseProjView * vec4 (pos * 2.0-1.0,1.0);
  pos = clip.xyz/clip.w;

  float dist = length (lightPos-pos);
  float atten = 1.0 - clamp(dist/lightRadius,0.0,1.0);

  if(atten== 0.0) {
    discard;
  }

  float shadow = 1.0f;
  if (has_shadow > 0.0)
  {
    vec4 shadow_pos = lightprojView * vec4(pos.xyz, 1.0f);
    vec3 posSh = shadow_pos.xyz / shadow_pos.w;
    posSh = posSh * 0.5f + 0.5f;

    float shadow_depth = texture(depthTex_Shadow, posSh.xy).x;
    if (raw_depth < posSh.z)
    {
      shadow = 0.0f;
    }
  }



  vec3 incident = normalize ( lightPos - pos );
  vec3 viewDir = normalize ( cameraPos - pos );
  vec3 halfDir = normalize ( incident + viewDir );

  float lambert = clamp (dot(incident,normal) ,0.0 ,1.0);
  float rFactor = clamp (dot(halfDir,normal) ,0.0 ,1.0);
  float sFactor = pow (rFactor,33.0);

  gl_FragColor [0] = vec4 (vec3(shadow), 1.0f);//lightColour.xyz * lambert * atten * shadow, 1.0);
  gl_FragColor [1] = vec4 (0);//lightColour.xyz * sFactor * atten *0.33 * shadow,1.0);
}
