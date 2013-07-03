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

/*
 void main()
 {
 float pixelWidth = 1.0 / 512.0;
 float pixelHeight = 1.0 / 512.0;
 
 // Current texture coordinate
 vec2 texel = vTex;
 vec4 pixel = vec4(texture2D(textureUnit0, texel));
 
 // Larger constant = bigger glow
 float glow = 4.0 * ((pixelWidth + pixelHeight) / 2.0);
 
 // The vector to contain the new, "bloomed" colour values
 vec4 bloom = vec4(0);
 
 // Loop over all the pixels on the texture in the area given by the constant in glow
 float count = 0.0;
 for(float x = texel.x - glow; x < texel.x + glow; x += pixelWidth)
 {
 for(float y = texel.y - glow; y < texel.y + glow; y += pixelHeight)
 {
 // Add that pixel's value to the bloom vector
 bloom += (texture2D(textureUnit0, vec2(x, y)) - 0.4) * 30.0;
 // Add 1 to the number of pixels sampled
 count = count + 1.0;
 }
 }
 // Divide by the number of pixels sampled to average out the value
 // The constant being multiplied with count here will dim the bloom effect a bit, with higher values
 // Clamp the value between a 0.0 to 1.0 range
 vec4 b = bloom;// / (count * 30.0);
 b = clamp(b, 0.0, 1.0);
 
 // Set the current fragment to the original texture pixel, with our bloom value added on
 gl_FragColor = pixel + b;
 }
*/