varying vec2 vTex;
uniform sampler2D textureUnit0;
uniform vec4 vColor;

void main(void)
{
    vec4 sum = vec4(0);
    vec2 texcoord = vTex;
    int j;
    int i;
    
    for( i= -4 ;i < 4; i++)
    {
        for (j = -3; j < 3; j++)
        {
            sum += texture2D(textureUnit0, texcoord + vec2(j, i)*0.004) * 0.25;
        }
    }
    if (texture2D(textureUnit0, texcoord).r < 0.3)
    {
        gl_FragColor = sum*sum*0.012 + texture2D(textureUnit0, texcoord);
    }
    else
    {
        if (texture2D(textureUnit0, texcoord).r < 0.5)
        {
            gl_FragColor = sum*sum*0.009 + texture2D(textureUnit0, texcoord);
        }
        else
        {
            gl_FragColor = sum*sum*0.0075 + texture2D(textureUnit0, texcoord);
        }
    }

    //gl_FragColor = vColor * texture2D(textureUnit0, vTex);
}