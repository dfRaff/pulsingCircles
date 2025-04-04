#pragma once

#include "ofMain.h"
#include "pulseCircle.h"
#include "RNBO.h"
#include "BloomEffect.hpp"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
        void mousePressed(int x, int y, int button) override;
        void mouseReleased(int x, int y, int button) override;
        void audioOut(ofSoundBuffer &outBuffer) override;
        void sendMidiNoteOn(uint8_t pitch, uint8_t velocity);
        void sendMidiNoteOff(uint8_t pitch);
        int getRandomAeolianNote();

    std::vector<Circle> circles;
    float lastUpdateTime;
    
    int bufferSize;
    int sampleRate;
    
    RNBO::CoreObject rnboObject;
    ofSoundStream soundStream;

    std::vector<float> lAudio;
    std::vector<float> rAudio;
    
    //Midi values.
    uint8_t midiChannel = 0;    // MIDI channel 0
    uint8_t note = 60;          // Middle C
    uint8_t velocity = 100;
    
    std::vector<int> aeolianIntervals = { 0, 2, 3, 5, 7, 8, 10 };
    int rootNote = 57; // A3
    int lastMidiNote = -1;
    
    ofFbo sceneFbo;
    BloomEffect bloomEffect;
    
    ofxPanel gui;
    ofParameter<float> frequency, amplitude, intensity;
    ofParameter<int> pointCount;
    ofxFloatSlider thresholdSlider;
    ofxFloatSlider blurRadiusSlider;
    ofxFloatSlider intensitySlider;

};
