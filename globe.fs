#version 330 core
out vec4 FragColor;
//in vec4 TexCoord;
//uniform vec2 u_resolution;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	//vec2 lnglat = vec2((atan(TexCoord.y, TexCoord.x)/3.141592 + 1.0) * 0.5,\
	//	(asin(TexCoord.z) / 3.141592 + 0.5));
	//vec2 coord = gl_FragCoord.xy/u_resolution;
    //coord = coord*2. - 1.;
	//coord.x *= u_resolution.x/u_resolution.y;
	//gl_FragCoord = coord;
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
	//FragColor = texture(texture1, lnglat);

	//FragColor = vec4(1.0f,1.0f,1.0f,1.0f);
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 1.0f);

//	FragColor = texture(texture1, TexCoord);
		
//		float offset = 0.50f;
//		if(offset+0.00f <= TexCoord.x && TexCoord.x <= offset+0.01f){
//			FragColor = vec4(0.0f,1.0f,0.0f,1.0f);
//		}

//		float reverse = 1.0f;
//		if(reverse-0.10f<=TexCoord.y && TexCoord.y <= reverse - 0.09f){
//			FragColor = vec4(1.0f,0.0f,0.0f,1.0f);
//		}
	
}