// ######################
//         CONFIG
// ######################

#define LED LED_BUILTIN
// pin definition for every channel
static const uint8_t pin[] = { A0, A1, A2, A3, A4, A5, A6, A7 };

int PIN_AMOUNT = 7;

int MINIMUM_OUTPUT_VELOCITY = 50;
int MAXIMUM_OUTPUT_VELOCITY = 127;  // 0 - 127

int MINUM_NOTE_DURATION = 50;
// amount of times 0 has to be measured in order to end a note
int ZERO_MEASURE_THRESHOLD = 2;

// minimum input value to trigger a note for each channel
int threshold[10] = { 300, 800, 100, 800, 1000, 1000, 600, 600, 100, 100 };

int minimumNoteDuration[10] = { 200, 200, 50, 1000, 200, 200, 200, 200, 100, 100 };

// the pitch for every channel
int pitch[10] = {
  36,  // 1 Kick
  40,  // 2 Snare
  42,  // 3 Hi-Hat
  49,  // 4 Crash
  48,  // 5 Tom 1
  47,  // 6 Tom 2
  41,  // - Tom 3
  51,  // 7 Ride
  80,  //
  85   //
};

// contains the millis() when a note has been started
unsigned long notes[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int timesZero[10] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

// ######################

void setup() {
  Serial.begin(31250);
  Serial1.begin(31250);
  pinMode(LED, OUTPUT);
}

int lastone = 0;

void loop() {
  for (int i = 0; i < PIN_AMOUNT; i++) {
    // first read the value measured by the piezo
    int value = analogRead(pin[i]);
    value = value > 10 ? value : 0;
    unsigned long time = millis();

    if (notes[i] > 0) {
      if (time > notes[i] + minimumNoteDuration[i]) {  // if last note is far enough in the past
        notes[i] = 0;
        timesZero[i] = 0;
        noteOff(pitch[i]);
      }
    }
    // before playing a new note, the value has to measure 0 a few times
    else if (timesZero[i] < ZERO_MEASURE_THRESHOLD) {
      if (value == 0) {
        timesZero[i]++;
      } else {
        timesZero[i] = 0;
      }
    }

    // if measured value is high enough
    // and no note is still in the pipe
    // and it has been measured zero long enough
    // (for most notes this is irrelevant, since their ZERO_MEASUREMENT_VALUE is -1)
    if (value >= threshold[i] && notes[i] == 0 && timesZero[i] == ZERO_MEASURE_THRESHOLD) {
      // filtering out certain drums, when kick is played
      if ((i == 1 || i == 4 || i == 5) && notes[0] - time < 200) {

      } else {
        notes[i] = time;
        digitalWrite(LED, HIGH);
        noteOn(pitch[i], map(value, 0, 1023, MINIMUM_OUTPUT_VELOCITY, MAXIMUM_OUTPUT_VELOCITY));
        digitalWrite(LED, LOW);
      }
    }
  }
  delay(5);
}

void noteOn(int pitch, int velocity) {
  sendNote(0x90, pitch, velocity);
}

void noteOff(int pitch) {
  sendNote(0x80, pitch, 0);
}

void sendNote(int cmd, int pitch, int velocity) {
  Serial1.write(cmd);
  Serial1.write(pitch);
  Serial1.write(velocity);
}
