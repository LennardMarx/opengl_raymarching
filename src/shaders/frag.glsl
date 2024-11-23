precision mediump float;                            
varying vec2 texCoord;                              
uniform sampler2D texSampler;                       
void main()                                         
{                                                   
    gl_FragColor = texture2D(texSampler, texCoord); 
}                                                   
