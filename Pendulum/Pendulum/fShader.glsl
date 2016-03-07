

in  vec4 color;
in  vec2 texCoord;

out vec4 fColor;

uniform sampler2D texName;

void main() 
{ 
    fColor = color * texture2D( texName, texCoord );
} 