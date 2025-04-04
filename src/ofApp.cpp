#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofDisableArbTex();  // ✅ leave this first

    sceneFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA8);

    // Setup bloom effect
    bloomEffect.setup(ofGetWidth(), ofGetHeight());
    
    //======= GUI Addon for Shader==========
    gui.setup();
    gui.add(frequency.set("Frequency", 2.0, 0.1, 10.0));
    gui.add(amplitude.set("Amplitude", 0.2, 0.0, 1.0));
    gui.add(intensity.set("Intensity", 10.0, 1.0, 10.0));
    gui.add(pointCount.set("Point Count", 130, 3, 200));
    gui.add(thresholdSlider.setup("Threshold", 0.01, 0.0, 1.0));  // Initial value, min, max
    gui.add(blurRadiusSlider.setup("Blur Radius", 3.7, 0.1, 10.0));  // Adjust blur radius
    gui.add(intensitySlider.setup("Intensity1", 5.664, 0.1, 10.0));  // Adjust bloom intensity
    
    ofEnableAlphaBlending();
    ofSetCircleResolution(100);
    lastUpdateTime = ofGetElapsedTimef();
    
    //======= RNBO Library ==========
    bufferSize = 512;
    sampleRate = 44100;

        lAudio.assign(bufferSize, 0.0f);
        rAudio.assign(bufferSize, 0.0f);

        ofSoundStreamSettings settings;
        settings.setOutListener(this);
        settings.sampleRate = sampleRate;
        settings.numOutputChannels = 2;
        settings.numInputChannels = 0;
        settings.bufferSize = bufferSize;
        soundStream.setup(settings);
}

//--------------------------------------------------------------
void ofApp::update() {
    
    float currentTime = ofGetElapsedTimef();
    float deltaTime = currentTime - lastUpdateTime;
    lastUpdateTime = currentTime;

    for (int i = circles.size() - 1; i >= 0; --i) {
        circles[i].update(deltaTime);
        if (circles[i].isDead()) {
            circles.erase(circles.begin() + i);
        }
    }
}

void ofApp::draw() {
    // Draw into scene FBO
    
    sceneFbo.begin();
    ofClear(0, 0, 0, 255);  // Ensure clear before drawing
    for (auto& c : circles) {
        c.draw();
    }
    sceneFbo.end();

    // ✅ Bypass bloom and test direct output
    sceneFbo.draw(0, 0);  // ← This should draw your translucent circles!

    // 👇 Comment out bloom temporarily
     bloomEffect.setThreshold(thresholdSlider);
     bloomEffect.setBlurRadius(blurRadiusSlider);
     bloomEffect.setIntensity(intensitySlider);
     bloomEffect.apply(sceneFbo);

    gui.draw();
}


void ofApp::mousePressed(int x, int y, int button) {
    int midiNote = getRandomAeolianNote();
    int midiVelocity = ofRandom(40, 127);  // or randomize this if you like

        circles.push_back(Circle(glm::vec2(x, y), midiVelocity));

        sendMidiNoteOn(midiNote, midiVelocity);
        lastMidiNote = midiNote;
    
}

void ofApp::mouseReleased(int x, int y, int button) {
    circles.push_back(Circle(glm::vec2(x, y)));

    if (lastMidiNote != -1) {
        sendMidiNoteOff(lastMidiNote);
        lastMidiNote = -1; // reset
    }
}

void ofApp::audioOut(ofSoundBuffer & buffer){
    rnboObject.prepareToProcess(sampleRate, buffer.getNumFrames());

    RNBO::SampleValue** inputs = nullptr;

    // Allocate stereo output buffer
    RNBO::SampleValue** outputs = new RNBO::SampleValue*[2];
    outputs[0] = new RNBO::SampleValue[buffer.getNumFrames()];
    outputs[1] = new RNBO::SampleValue[buffer.getNumFrames()];

    rnboObject.process(inputs, 0, outputs, 2, buffer.getNumFrames());

    for (unsigned long i = 0; i < buffer.getNumFrames(); i++) {
        float outL = static_cast<float>(outputs[0][i]);
        float outR = static_cast<float>(outputs[1][i]);

        lAudio[i] = buffer[i * 2    ] = outL;
        rAudio[i] = buffer[i * 2 + 1] = outR;
    }

    delete[] outputs[0];
    delete[] outputs[1];
    delete[] outputs;
}

int ofApp::getRandomAeolianNote() {
    int base = rootNote;
    
    // Random interval from Aeolian scale
    int interval = aeolianIntervals[int(ofRandom(aeolianIntervals.size()))];

    // Random octave transposition: -1, 0, +1, or +2
    int octaveOffset = 12 * int(ofRandom(-1, 3));  // -1 to +2 octaves

    return base + interval + octaveOffset;
}

void ofApp::sendMidiNoteOn(uint8_t pitch, uint8_t velocity) {
    uint8_t status = 0x90 | midiChannel;  // 0x90 = note on
    uint8_t midiBytes[3] = { status, pitch, velocity };

    RNBO::MidiEvent noteOnEvent(
        rnboObject.getCurrentTime(), // current RNBO time
        0,                           // port
        midiBytes,
        3
    );

    rnboObject.scheduleEvent(noteOnEvent);
}

void ofApp::sendMidiNoteOff(uint8_t pitch) {
    uint8_t status = 0x80 | midiChannel;  // 0x80 = note off
    uint8_t midiBytes[3] = { status, pitch, 0 };

    RNBO::MidiEvent noteOffEvent(
        rnboObject.getCurrentTime(),
        0,
        midiBytes,
        3
    );

    rnboObject.scheduleEvent(noteOffEvent);
}
