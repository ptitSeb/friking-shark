#version 150

uniform sampler2D Texture0;
uniform sampler2D Texture1;

in vec4 g_Color;
in vec2 g_TexCoord0;
in vec2 g_TexCoord1;
out vec4 oColor;

void main (void) 
{
  vec4 texcolor=g_Color;
  vec4 finalcolor;
  
#ifdef ENABLE_TEXTURES
    texcolor*= texture2D(Texture0, g_TexCoord0.xy);
	#if TEXTURE_UNITS > 1
		texcolor*= texture2D(Texture1, g_TexCoord1.xy);
	#endif
#endif
	
	finalcolor.rgb=texcolor.rgb;
	finalcolor.a=texcolor.a;

 #ifdef RENDERING_POINTS
	float tempDist=length(gl_PointCoord-vec2(0.5))*2.0;
	float dist=1.0-smoothstep(0.8,1.0,tempDist);
	
 	oColor=finalcolor;
	oColor.a=dist;
 #else
 	oColor=finalcolor;
 #endif
}

