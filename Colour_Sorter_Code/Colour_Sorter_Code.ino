#include <Servo.h> 

 

#define S0 2 

#define S1 3 

#define S2 4 

#define S3 5 

#define SENSOR_OUT 6 

 

Servo topServo; 

Servo bottomServo; 

 

int frequency = 0; 

int detectedColor = 0; 

int lastDetectedColor = -1; // Store the last detected color 

bool bottomServoMoved = false; // Track if bottom servo has moved 

 

// Function prototypes 

int readColor(); 

void moveServo(Servo &servo, int start, int end, int step, int delayTime); 

int getColorFrequency(int s2State, int s3State); 

int detectedColorToPosition(int color); 

 

void setup() { 

  pinMode(S0, OUTPUT); 

  pinMode(S1, OUTPUT); 

  pinMode(S2, OUTPUT); 

  pinMode(S3, OUTPUT); 

  pinMode(SENSOR_OUT, INPUT); 

 

  // Setting frequency-scaling to 20% 

  digitalWrite(S0, HIGH); 

  digitalWrite(S1, LOW); 

 

  topServo.attach(7); 

  bottomServo.attach(8); 

  Serial.begin(9600); 

  Serial.println("Setup Complete"); 

} 

 

void loop() { 

  topServo.write(115); 

  delay(500); 

 

  bottomServoMoved = false; // Reset the movement flag 

 

  // Increased forward movement of top servo (115° to 10°) 

  for (int pos = 115; pos >= 10; pos--) { 

    topServo.write(pos); 

    delay(10); // Smooth movement 

 

    // Move bottom servo only once when top servo reaches 90° 

    if (!bottomServoMoved && pos <= 90) { 

      detectedColor = readColor(); 

      delay(10); 

 

      Serial.print("Detected Color: "); 

      Serial.println(detectedColor); 

 

      if (detectedColor != lastDetectedColor) {  

        int targetPosition = detectedColorToPosition(detectedColor); 

        Serial.print("Moving bottom servo to: "); 

        Serial.println(targetPosition); 

         

        moveServo(bottomServo, bottomServo.read(), targetPosition,  

                  (bottomServo.read() < targetPosition) ? 1 : -1, 1); 

        lastDetectedColor = detectedColor;  

        bottomServoMoved = true; // Ensure it moves only once 

        delay(1000); 

      } 

    } 

  } 

   

  delay(500); 

 

  moveServo(topServo, 10, 45, 1, 10);  // Return to start position 

  detectedColor = 0; 

}  

 

// Function to move a servo smoothly 

void moveServo(Servo &servo, int start, int end, int step, int delayTime) { 

  if (start == end) return; 

  for (int pos = start; (step > 0) ? (pos <= end) : (pos >= end); pos += step) { 

    servo.write(pos); 

    delay(delayTime); 

  } 

} 

 

// Function to map detected color to *unique* servo positions 

int detectedColorToPosition(int color) { 

  switch (color) { 

    case 1: return 80;  // Red (Left side) 

    case 2: return 120; // Blue (Middle position) 

    case 3: return 160; // Green (Right side) 

    default: return 180; // Default position for unrecognized colors 

  } 

} 

 

// Function to read color from the sensor 

int readColor() { 

  int R = getColorFrequency(LOW, LOW);   // Red 

  int G = getColorFrequency(HIGH, HIGH); // Green 

  int B = getColorFrequency(LOW, HIGH);  // Blue 

 

  Serial.print("R= "); Serial.print(R); 

  Serial.print("  G= "); Serial.print(G); 

  Serial.print("  B= "); Serial.println(B); 

   

  if (R > G && R > B) return 1; // Red 

  if (B > R && B > G) return 2; // Blue 

  if (G > R && G > B) return 3; // Green 

 

  return 0; // No valid color detected 

} 

 

// Function to get color frequency from sensor 

int getColorFrequency(int s2State, int s3State) { 

  digitalWrite(S2, s2State); 

  digitalWrite(S3, s3State); 

  delay(50); 

  return pulseIn(SENSOR_OUT, LOW); 

} 