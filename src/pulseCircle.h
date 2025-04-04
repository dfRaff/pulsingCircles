#include "ofMain.h"

class Circle {
public:
    glm::vec2 pos;
    float baseRadius;
    float scale;
    float speed;
    float alpha;
    ofColor color;
    float age;
    float lifespan;

    Circle(glm::vec2 position, int midiValue = 64) {
        pos = position;
        baseRadius = ofRandom(30, 100);
        scale = 3.0;
        speed = ofRandom(0.1, 0.4);
        alpha = 0;
        age = 0;
        lifespan = ofRandom(3.0, 5.0);

        // Map MIDI value [0, 127] to brightness [50, 255] (for subtlety)
        float brightness = ofMap(midiValue, 0, 127, 50, 255);
        float hue = ofRandom(255);
        float saturation = 200;  // keep fairly saturated
        color = ofColor::fromHsb(hue, saturation, brightness, alpha);
    }

    void update(float deltaTime) {
        age += deltaTime;

        // Scale pulse
        scale = 1.0 + 0.5 * sin(age * speed * TWO_PI);

        float maxAlpha = 30;
        
        // Fade in/out
        float fadeDuration = 1.0;
        if (age < fadeDuration) {
            alpha = ofMap(age, 0, fadeDuration, 0, maxAlpha);
        } else if (age > lifespan - fadeDuration) {
            alpha = ofMap(age, lifespan - fadeDuration, lifespan, maxAlpha, 0);
        } else {
            alpha = maxAlpha;
        }
        color.a = alpha;
    }

    void draw() {

        ofPushStyle();                      // Protect style state
            ofEnableBlendMode(OF_BLENDMODE_ADD);  // Glow overlaps

            // Rebuild the color with bright hue & alpha
            float hue = color.getHue();  // Keep same hue
            ofColor drawColor = ofColor::fromHsb(hue, 255, 255, alpha);  // Full sat, bright

            ofSetColor(drawColor);
            ofDrawCircle(pos, baseRadius * scale);

            ofDisableBlendMode();  // Restore default alpha blending
            ofPopStyle();
    }

    bool isDead() const {
        return age > lifespan;
    }
};
