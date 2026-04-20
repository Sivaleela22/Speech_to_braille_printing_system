#include<SoftwareSerial.h>
SoftwareSerial ble(10,11);
const int dirPin1 = 7;
const int stepPin1 = 6;
const int dirPin2 =5;
const int stepPin2 = 4;

int ylmt = 2;
int xlmt = 3;
int dot_pin1 = 8;
int dot_pin2 = 9;

// Configuration
const int stepsPerRevolution = 200;
int stdly = 5000;  // Homing speed

// Braille cell dimensions in steps
const int dotSpacingX = 50;    // Horizontal spacing between dots
const int dotSpacingY = 50;    // Vertical spacing between dots
const int charSpacingX = 200;  // Spacing between characters
const int charSpacingY = 200;  // Spacing between lines

// Current position tracking
int currentX = 0;
int currentY = 0;
const int maxX = 1000;  // Max printable area
const int maxY = 1000;

// ================= HOMING FUNCTIONS =================
void yhome()
{
// Home Y axis
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, HIGH);
  while(digitalRead(ylmt) == 1) {
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(stdly);
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(stdly);
  }
}
void go_home() {
  Serial.println("Homing...");
  
  // Home Y axis
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, HIGH);
  while(digitalRead(ylmt) == 1) {
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(stdly);
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(stdly);
  }
  delay(1000);

  // Home X axis
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, LOW);
  while(digitalRead(xlmt) == 1) {
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(stdly);
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(stdly);
  }
  
  // Reset position tracking
  currentX = 0;
  currentY = 0;
  Serial.println("Homing complete!");
}

// ================= MOVEMENT FUNCTIONS =================
void movx_frw(int steps) {
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, HIGH);
  
  for(int x = 0; x < steps; x++) {
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(2000);
  }
}

void movx_bkw(int steps) {
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, LOW);
  
  for(int x = 0; x < steps; x++) {
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(2000);
  }
}

void movy_frw(int steps) {
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, LOW);
  
  for(int x = 0; x < steps; x++) {
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(2000);
  }
}

void movy_bkw(int steps) {
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, HIGH);
  
  for(int x = 0; x < steps; x++) {
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(2000);
  }
}

// ================= IMPROVED MOVEMENT WITH POSITION TRACKING =================
void moveToPosition(int targetX, int targetY) {
  int dx = targetX - currentX;
  int dy = targetY - currentY;
  
  // Move in X direction
  if (dx > 0) {
    digitalWrite(dirPin1, HIGH);
    digitalWrite(dirPin2, HIGH);
    for(int i = 0; i < abs(dx); i++) {
      digitalWrite(stepPin1, HIGH);
      digitalWrite(stepPin2, HIGH);
      delayMicroseconds(2000);
      digitalWrite(stepPin1, LOW);
      digitalWrite(stepPin2, LOW);
      delayMicroseconds(2000);
    }
    currentX += dx;
  } 
  else if (dx < 0) {
    digitalWrite(dirPin1, LOW);
    digitalWrite(dirPin2, LOW);
    for(int i = 0; i < abs(dx); i++) {
      digitalWrite(stepPin1, HIGH);
      digitalWrite(stepPin2, HIGH);
      delayMicroseconds(2000);
      digitalWrite(stepPin1, LOW);
      digitalWrite(stepPin2, LOW);
      delayMicroseconds(2000);
    }
    currentX += dx;  // dx is negative
  }
  
  // Move in Y direction
  if (dy > 0) {
    digitalWrite(dirPin1, HIGH);
    digitalWrite(dirPin2, LOW);
    for(int i = 0; i < abs(dy); i++) {
      digitalWrite(stepPin1, HIGH);
      digitalWrite(stepPin2, HIGH);
      delayMicroseconds(2000);
      digitalWrite(stepPin1, LOW);
      digitalWrite(stepPin2, LOW);
      delayMicroseconds(2000);
    }
    currentY += dy;
  } 
  else if (dy < 0) {
    digitalWrite(dirPin1, LOW);
    digitalWrite(dirPin2, HIGH);
    for(int i = 0; i < abs(dy); i++) {
      digitalWrite(stepPin1, HIGH);
      digitalWrite(stepPin2, HIGH);
      delayMicroseconds(2000);
      digitalWrite(stepPin1, LOW);
      digitalWrite(stepPin2, LOW);
      delayMicroseconds(2000);
    }
    currentY += dy;  // dy is negative
  }
}

// ================= DOT PRINTING =================
void dot() {
  delay(50);
  digitalWrite(dot_pin1, HIGH);
  digitalWrite(dot_pin2, LOW);
  delay(50);
  digitalWrite(dot_pin1, LOW);
  digitalWrite(dot_pin2, LOW);
  delay(50);
}

// ================= BRAILLE CHARACTER DEFINITIONS =================
// Braille pattern: 3x2 grid
// Positions: 1 4
//            2 5
//            3 6

// Letters A-J
bool brailleA[] = {1, 0, 0, 0, 0, 0};  // Dot 1
bool brailleB[] = {1, 1, 0, 0, 0, 0};  // Dots 1-2
bool brailleC[] = {1, 0, 0, 1, 0, 0};  // Dots 1-4
bool brailleD[] = {1, 0, 0, 1, 1, 0};  // Dots 1-4-5
bool brailleE[] = {1, 0, 0, 0, 1, 0};  // Dots 1-5
bool brailleF[] = {1, 1, 0, 1, 0, 0};  // Dots 1-2-4
bool brailleG[] = {1, 1, 0, 1, 1, 0};  // Dots 1-2-4-5
bool brailleH[] = {1, 1, 0, 0, 1, 0};  // Dots 1-2-5
bool brailleI[] = {0, 1, 0, 1, 0, 0};  // Dots 2-4
bool brailleJ[] = {0, 1, 0, 1, 1, 0};  // Dots 2-4-5

// Letters K-T
bool brailleK[] = {1, 0, 1, 0, 0, 0};  // Dots 1-3
bool brailleL[] = {1, 1, 1, 0, 0, 0};  // Dots 1-2-3
bool brailleM[] = {1, 0, 1, 1, 0, 0};  // Dots 1-3-4
bool brailleN[] = {1, 0, 1, 1, 1, 0};  // Dots 1-3-4-5
bool brailleO[] = {1, 0, 1, 0, 1, 0};  // Dots 1-3-5
bool brailleP[] = {1, 1, 1, 1, 0, 0};  // Dots 1-2-3-4
bool brailleQ[] = {1, 1, 1, 1, 1, 0};  // Dots 1-2-3-4-5
bool brailleR[] = {1, 1, 1, 0, 1, 0};  // Dots 1-2-3-5
bool brailleS[] = {0, 1, 1, 1, 0, 0};  // Dots 2-3-4
bool brailleT[] = {0, 1, 1, 1, 1, 0};  // Dots 2-3-4-5

// Letters U-Z
bool brailleU[] = {1, 0, 1, 0, 0, 1};  // Dots 1-3-6
bool brailleV[] = {1, 1, 1, 0, 0, 1};  // Dots 1-2-3-6
bool brailleW[] = {0, 1, 0, 1, 1, 1};  // Dots 2-4-5-6
bool brailleX[] = {1, 0, 1, 1, 0, 1};  // Dots 1-3-4-6
bool brailleY[] = {1, 0, 1, 1, 1, 1};  // Dots 1-3-4-5-6
bool brailleZ[] = {1, 0, 1, 0, 1, 1};  // Dots 1-3-5-6

// Numbers 1-9 (Braille numbers use same patterns as A-J with number prefix ⠼)
// Number prefix: dots 3-4-5-6 (⠼)
// Then follow with letters A-J for 1-0
bool brailleNumPrefix[] = {0, 0, 1, 1, 1, 1};  // Number prefix ⠼

// Numbers 1-9 (same as A-I with number prefix)
bool braille1[] = {0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0};  // ⠼ + A
bool braille2[] = {0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0};  // ⠼ + B
bool braille3[] = {0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0};  // ⠼ + C
bool braille4[] = {0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0};  // ⠼ + D
bool braille5[] = {0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0};  // ⠼ + E
bool braille6[] = {0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0};  // ⠼ + F
bool braille7[] = {0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0};  // ⠼ + G
bool braille8[] = {0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0};  // ⠼ + H
bool braille9[] = {0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0};  // ⠼ + I
bool braille0[] = {0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0};  // ⠼ + J

// Special characters
bool brailleSpace[] = {0, 0, 0, 0, 0, 0};           // Space
bool braillePeriod[] = {0, 1, 0, 0, 1, 1};          // Period (dots 2-5-6)
bool brailleComma[] = {0, 1, 0, 0, 0, 0};           // Comma (dot 2)
bool brailleQuestion[] = {0, 1, 1, 0, 0, 1};        // Question mark (dots 2-3-6)

// ================= SIMPLIFIED BRAILLE PRINTING =================
void printBrailleCharacter(bool* pattern, int patternLength = 6) {
  Serial.print("Printing character at X:");
  Serial.print(currentX);
  Serial.print(" Y:");
  Serial.println(currentY);
  
  int startX = currentX;  // Save starting position
  int startY = currentY;
  
  // For numbers (12 dots: prefix + letter)
  if (patternLength == 12) {
    // Print number prefix (dots 3-4-5-6)
    int prefixPositions[4][2] = {
      {startX, startY + (2 * dotSpacingY)},         // Dot 3 (bottom-left)
      {startX + dotSpacingX, startY},               // Dot 4 (top-right)
      {startX + dotSpacingX, startY + dotSpacingY}, // Dot 5 (middle-right)
      {startX + dotSpacingX, startY + (2 * dotSpacingY)} // Dot 6 (bottom-right)
    };
    
    // Print prefix dots (positions 3,4,5,6 in pattern array)
    for (int i = 0; i < 4; i++) {
      if (pattern[i]) {
        moveToPosition(prefixPositions[i][0], prefixPositions[i][1]);
        dot();
      }
    }
    
    // Move right for the number character
    startX += charSpacingX;
    moveToPosition(startX, startY);
    
    // Now print the number character (dots 7-12 in pattern array)
    pattern = &pattern[6];  // Point to the character part
    patternLength = 6;      // Now it's a regular 6-dot pattern
  }
  
  // Dot positions relative to character start (for 6-dot patterns)
  int dotPositions[6][2] = {
    {startX, startY},                    // Dot 1 (top-left)
    {startX, startY + dotSpacingY},      // Dot 2 (middle-left)
    {startX, startY + (2 * dotSpacingY)}, // Dot 3 (bottom-left)
    {startX + dotSpacingX, startY},      // Dot 4 (top-right)
    {startX + dotSpacingX, startY + dotSpacingY}, // Dot 5 (middle-right)
    {startX + dotSpacingX, startY + (2 * dotSpacingY)} // Dot 6 (bottom-right)
  };
  
  // Print each dot that should be raised
  for (int i = 0; i < patternLength; i++) {
    if (pattern[i]) {
      moveToPosition(dotPositions[i][0], dotPositions[i][1]);
      dot();
    }
  }
  
  // Move to next character position (right of current character)
  currentX = startX + charSpacingX;
  currentY = startY;
   yhome();
   movx_frw(50);
  Serial.print("Next character will start at X:");
  Serial.print(currentX);
  Serial.print(" Y:");
  Serial.println(currentY);
}

void printBrailleString(String text, int startX, int startY) {
  // Start at the given position
  currentX = startX;
  currentY = startY;
  moveToPosition(startX, startY);
  
  Serial.println("=================================");
  Serial.print("Printing: \"");
  Serial.print(text);
  Serial.println("\"");
  Serial.print("Starting at X:");
  Serial.print(startX);
  Serial.print(" Y:");
  Serial.println(startY);
  
  // Track if we're in number mode
  bool numberMode = false;
  
  for (int i = 0; i < text.length(); i++) {
    char c = text.charAt(i);
    bool* pattern;
    int patternLength = 6;  // Default to 6 dots
    
    switch (toupper(c)) {
      // Letters A-Z
      case 'A': pattern = brailleA; break;
      case 'B': pattern = brailleB; break;
      case 'C': pattern = brailleC; break;
      case 'D': pattern = brailleD; break;
      case 'E': pattern = brailleE; break;
      case 'F': pattern = brailleF; break;
      case 'G': pattern = brailleG; break;
      case 'H': pattern = brailleH; break;
      case 'I': pattern = brailleI; break;
      case 'J': pattern = brailleJ; break;
      case 'K': pattern = brailleK; break;
      case 'L': pattern = brailleL; break;
      case 'M': pattern = brailleM; break;
      case 'N': pattern = brailleN; break;
      case 'O': pattern = brailleO; break;
      case 'P': pattern = brailleP; break;
      case 'Q': pattern = brailleQ; break;
      case 'R': pattern = brailleR; break;
      case 'S': pattern = brailleS; break;
      case 'T': pattern = brailleT; break;
      case 'U': pattern = brailleU; break;
      case 'V': pattern = brailleV; break;
      case 'W': pattern = brailleW; break;
      case 'X': pattern = brailleX; break;
      case 'Y': pattern = brailleY; break;
      case 'Z': pattern = brailleZ; break;
      
      // Numbers 0-9
      case '0': pattern = braille0; patternLength = 12; break;
      case '1': pattern = braille1; patternLength = 12; break;
      case '2': pattern = braille2; patternLength = 12; break;
      case '3': pattern = braille3; patternLength = 12; break;
      case '4': pattern = braille4; patternLength = 12; break;
      case '5': pattern = braille5; patternLength = 12; break;
      case '6': pattern = braille6; patternLength = 12; break;
      case '7': pattern = braille7; patternLength = 12; break;
      case '8': pattern = braille8; patternLength = 12; break;
      case '9': pattern = braille9; patternLength = 12; break;
      
      // Special characters
      case ' ': pattern = brailleSpace; break;
      case '.': pattern = braillePeriod; break;
      case ',': pattern = brailleComma; break;
      case '?': pattern = brailleQuestion; break;
      
      // Number mode toggle (not standard Braille, but useful for testing)
      case '#': 
        numberMode = !numberMode;
        Serial.println(numberMode ? "Entering NUMBER mode" : "Exiting NUMBER mode");
        continue;
        
      default:
        // Unknown character - treat as space
        pattern = brailleSpace;
        break;
    }
    
    Serial.print("Character ");
    Serial.print(i);
    Serial.print(": '");
    Serial.print(c);
    Serial.print("' at X:");
    Serial.print(currentX);
    Serial.print(" Y:");
    Serial.println(currentY);
    
    // Print this character
    printBrailleCharacter(pattern, patternLength);
    
    // Move to the position for next character
    moveToPosition(currentX, currentY);
    
    // Check for line wrap
    if (currentX > maxX) {
      currentX = startX;
      currentY += charSpacingY;
      moveToPosition(currentX, currentY);
      Serial.println("LINE WRAP to next line");
    }
    
    delay(50);  // Small pause between characters
  }
  
  Serial.println("Printing complete!");
  Serial.println("=================================");
}

// ================= TEST FUNCTIONS =================
void printAlphabetTest() {
  Serial.println("=== PRINTING ALPHABET A-Z ===");
  printBrailleString("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 100, 100);
  go_home();
  Serial.println("Alphabet test complete!");
}

void printNumbersTest() {
  Serial.println("=== PRINTING NUMBERS 0-9 ===");
  printBrailleString("0123456789", 100, 100);
  go_home();
  Serial.println("Numbers test complete!");
}

void printMixedTest() {
  Serial.println("=== PRINTING MIXED TEXT ===");
  printBrailleString("HELLO WORLD 123", 100, 100);
  go_home();
  Serial.println("Mixed test complete!");
}

void printAllCharacters() {
  Serial.println("=== PRINTING ALL CHARACTERS ===");
  
  // Print alphabet
  currentX = 100;
  currentY = 100;
  printBrailleString("ABCDEFGHIJ", 100, 100);
  
  currentX = 100;
  currentY += charSpacingY;
  printBrailleString("KLMNOPQRST", 100, currentY);
  
  currentX = 100;
  currentY += charSpacingY;
  printBrailleString("UVWXYZ", 100, currentY);
  
  // Print numbers
  currentX = 100;
  currentY += charSpacingY * 2;  // Extra space
  printBrailleString("0123456789", 100, currentY);
  
  go_home();
  Serial.println("All characters printed!");
}

// ================= SETUP =================
void setup() {
  Serial.begin(9600);
  Serial.println("===========================================");
  Serial.println("      BRAILLE PRINTER - FULL ALPHABET");
  Serial.println("===========================================");
  ble.begin(9600);
  // Setup pins
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(dot_pin1, OUTPUT);
  pinMode(dot_pin2, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(xlmt, INPUT);
  pinMode(ylmt, INPUT);
  
  digitalWrite(dot_pin1, LOW);
  digitalWrite(dot_pin2, LOW);
  
  delay(2000);
  
  // Home the printer
  go_home();
  
  Serial.println("\nREADY FOR PRINTING!");
  Serial.println("Available commands via Serial Monitor:");
  Serial.println("1. Send any text to print");
  Serial.println("2. 'TEST1' - Print alphabet A-Z");
  Serial.println("3. 'TEST2' - Print numbers 0-9");
  Serial.println("4. 'TEST3' - Print 'HELLO WORLD 123'");
  Serial.println("5. 'TEST4' - Print all characters");
  Serial.println("6. 'HOME' - Return to home position");
  Serial.println("\nExamples: 'HELLO', 'ABC123', 'TEST 456'");
  Serial.println("\nWaiting for input...");
}

// ================= MAIN LOOP =================
void loop() {
  if (ble.available() > 0) {
    String input = ble.readStringUntil('\n');
    input.trim();
    
    if (input.length() > 0) {
      Serial.print("\nCommand received: ");
      Serial.println(input);
      
      if (input.equalsIgnoreCase("TEST1")) {
        printAlphabetTest();
      }
      else if (input.equalsIgnoreCase("TEST2")) {
        printNumbersTest();
      }
      else if (input.equalsIgnoreCase("TEST3")) {
        printMixedTest();
      }
      else if (input.equalsIgnoreCase("TEST4")) {
        printAllCharacters();
      }
      else if (input.equalsIgnoreCase("HOME")) {
        go_home();
      }
      else {
        // Print the text directly
        Serial.print("Printing: ");
        Serial.println(input);
        
        printBrailleString(input, 100, 100);
        go_home();
        
        Serial.println("Printing complete!");
      }
      
      Serial.println("\nReady for next command...");
    }
  }
  delay(100);
}
