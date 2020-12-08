#version 440 core
#define PI 3.14159265359
uniform float offsetX = 0.0f;
uniform float offsetY = 0.0f;
uniform int columns = 5;
uniform int rows = 5;
uniform float scaleX = 1.0;
uniform float scaleY = 1.0;
uniform int rotationAngle = 0;
uniform float randPosition = 0.0;
uniform float randRotation = 0.0;
uniform float randScale = 0.0;
uniform float maskStrength = 0.0;
uniform bool keepProportion = false;
uniform int inputCount = 1;
uniform bool useMask = false;
uniform sampler2D textureTile;
uniform sampler2D tile1;
uniform sampler2D tile2;
uniform sampler2D tile3;
uniform sampler2D tile4;
uniform sampler2D tile5;
uniform sampler2D randomTexture;
uniform sampler2D maskTexture;

in vec2 texCoords;

out vec4 FragColor;

mat2 rotate2d(float _angle){
    return mat2(cos(_angle),-sin(_angle),
                sin(_angle),cos(_angle));
}

void main()
{
    vec2 coords = texCoords;
    vec2 scaledUV = coords*vec2(columns, rows);
    vec2 offsetForProportion = vec2(0, 0);
    vec2 scaleForProportion = vec2(1.0, 1.0);
    if(keepProportion) {
        if(columns < rows) {
            offsetForProportion = vec2(1.0 - columns/rows, 0.0)*0.5;
            scaleForProportion = vec2(float(rows)/float(columns), 1.0);
        }
        else {
            offsetForProportion = vec2(0.0, 1.0 - rows/columns)*0.5;
            scaleForProportion = vec2(1.0, float(columns)/float(rows));
        }
    }
    vec2 cell = floor(scaledUV);
    vec2 offset = fract(scaledUV);
    vec2 randomUV = cell*vec2(0.037, 0.119);
    vec4 random = texture(randomTexture, randomUV);    
    vec4 image;
    vec4 color = vec4(0.0);
    for(int i = -3; i <= 2; ++i) {
        for(int j = -4; j <= 2; ++j) {
            vec2 cell_t = cell + vec2(i, j);
            cell_t = mod(cell_t, vec2(columns, rows));
            vec2 offset_t = offset - vec2(i, j);
            randomUV = cell_t*vec2(0.037, 0.119);
            random = texture(randomTexture, randomUV);
            randomUV += vec2(0.03, 0.17);

            vec2 coords = (offset_t - vec2(mod(cell_t.y, 2.0)*offsetX, mod(cell_t.x, 2.0)*offsetY) +
                           random.xy*randPosition);
            if(keepProportion) {
                coords -= offsetForProportion;
                coords *= scaleForProportion;
                coords += offsetForProportion;
            }
            coords -= 0.5;
            coords *= rotate2d(rotationAngle*PI/180.0 + PI*random.z*randRotation);
            coords /= (vec2(scaleX, scaleY) - vec2(random.w*randScale));
            coords += 0.5;           
            if((coords.x < 0.0 || coords.y < 0.0 || coords.x > 1.0 || coords.y > 1.0)) continue;
            int textureNumber = int(floor((random.z + random.w)*0.5*(inputCount - 0.1)));
            if(textureNumber == 0) {
                image = texture(textureTile, coords);
            }
            else if(textureNumber == 1) {
                image = texture(tile1, coords);
            }
            else if(textureNumber == 2) {
                image = texture(tile2, coords);
            }
            else if(textureNumber == 3) {
                image = texture(tile3, coords);
            }
            else if(textureNumber == 4) {
                image = texture(tile4, coords);
            }
            else if(textureNumber == 5) {
                image = texture(tile5, coords);
            }
            float maskS = 1.0 - (random.x + random.y)*0.5*maskStrength;

            if(image.a > 0) {
                image.rgb = max(color.rgb, image.rgb*maskS);
                color = image*image.a + color*(1.0 - image.a);
            }
        }
    }

    if(useMask) {
        vec4 maskColor = texture(maskTexture, texCoords);
        float mask = 0.33333*(maskColor.r + maskColor.g + maskColor.b);
        color *= mask;
    }
    FragColor = color;
}