#include "BloomEffect.hpp"

void BloomEffect::setup(int width, int height) {
    // Allocate FBOs
    brightPassFbo.allocate(width, height, GL_RGBA8);
    blurFboHorizontal.allocate(width, height, GL_RGBA8);
    blurFboVertical.allocate(width, height, GL_RGBA8);
    combineFbo.allocate(width, height, GL_RGBA8);

    // Load shaders
    brightPassShader.load("shaders/simpleVertex.vert", "shaders/brightPass.frag");
    blurShaderHorizontal.load("shaders/simpleVertex.vert", "shaders/blurHorizontal.frag");
    blurShaderVertical.load("shaders/simpleVertex.vert", "shaders/blurVertical.frag");
    combineShader.load("shaders/simpleVertex.vert", "shaders/combine.frag");
}

void BloomEffect::setThreshold(float threshold) {
    brightnessThreshold = threshold;
}

void BloomEffect::setBlurRadius(float radius) {
    blurRadius = radius;
}

void BloomEffect::setIntensity(float intensity) {
    this->intensity = intensity;
}

void BloomEffect::apply(ofFbo& sceneFbo) {
    // Bright pass
    brightPassFbo.begin();
    ofClear(0, 0, 0, 255);
    brightPassShader.begin();
    brightPassShader.setUniformTexture("sceneTex", sceneFbo.getTexture(), 0);
    brightPassShader.setUniform1f("brightnessThreshold", brightnessThreshold);
    drawQuad();  // Render full-screen quad instead of sceneFbo.draw()
    brightPassShader.end();
    brightPassFbo.end();

    // Horizontal blur
    blurFboHorizontal.begin();
    ofClear(0, 0, 0, 255);
    blurShaderHorizontal.begin();
    blurShaderHorizontal.setUniformTexture("brightTex", brightPassFbo.getTexture(), 0);
    blurShaderHorizontal.setUniform2f("resolution", brightPassFbo.getWidth(), brightPassFbo.getHeight());
    blurShaderHorizontal.setUniform1f("radius", blurRadius);
    drawQuad();  // Render full-screen quad
    blurShaderHorizontal.end();
    blurFboHorizontal.end();

    // Vertical blur
    blurFboVertical.begin();
    ofClear(0, 0, 0, 255);
    blurShaderVertical.begin();
    blurShaderVertical.setUniformTexture("brightTex", blurFboHorizontal.getTexture(), 0);
    blurShaderVertical.setUniform2f("resolution", blurFboHorizontal.getWidth(), blurFboHorizontal.getHeight());
    blurShaderVertical.setUniform1f("radius", blurRadius);
    drawQuad();  // Render full-screen quad
    blurShaderVertical.end();
    blurFboVertical.end();

    // Combine pass
    combineFbo.begin();
    ofClear(0, 0, 0, 255);
    combineShader.begin();
    combineShader.setUniformTexture("sceneTex", sceneFbo.getTexture(), 0);
    combineShader.setUniformTexture("bloomTex", blurFboVertical.getTexture(), 1);
    combineShader.setUniform1f("intensity", intensity);
    drawQuad();  // Render full-screen quad
    combineShader.end();
    combineFbo.end();

    // Output final result
    combineFbo.draw(0, 0, ofGetWidth(), ofGetHeight());  // Now this draws the full screen
}

void BloomEffect::drawQuad() {
    ofMesh quad;
    quad.clear();

    // Define quad vertices (in normalized device coordinates)
    quad.addVertex(glm::vec3(-1.0, -1.0, 0.0));  // Bottom-left corner
    quad.addTexCoord(glm::vec2(0.0, 1.0));       // Texture coordinate (0, 1)

    quad.addVertex(glm::vec3(1.0, -1.0, 0.0));   // Bottom-right corner
    quad.addTexCoord(glm::vec2(1.0, 1.0));       // Texture coordinate (1, 1)

    quad.addVertex(glm::vec3(1.0, 1.0, 0.0));    // Top-right corner
    quad.addTexCoord(glm::vec2(1.0, 0.0));       // Texture coordinate (1, 0)

    quad.addVertex(glm::vec3(-1.0, 1.0, 0.0));   // Top-left corner
    quad.addTexCoord(glm::vec2(0.0, 0.0));       // Texture coordinate (0, 0)

    quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    quad.draw();
}

