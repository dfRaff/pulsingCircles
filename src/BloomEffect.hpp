
#pragma once
#include "ofMain.h"

class BloomEffect {
public:
    void setup(int width, int height);
    void apply(ofFbo& sceneFbo);
    void setThreshold(float threshold);
    void setBlurRadius(float radius);
    void setIntensity(float intensity);

    void drawQuad();
    // FBOs and Shaders
    ofFbo brightPassFbo, blurFboHorizontal, blurFboVertical, combineFbo;
    ofShader brightPassShader, blurShaderHorizontal, blurShaderVertical, combineShader;

private:
    float brightnessThreshold = 0.2;
    float blurRadius = 1.0;
    float intensity = 1.0;
};


