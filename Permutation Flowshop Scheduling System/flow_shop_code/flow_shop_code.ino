#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <limits.h>

// Define the LCD using I2C address
LiquidCrystal_I2C lcd(0x27, 16, 2);

// LED pins
const int ledPins[] = {2, 3, 4, 5, 6};

// IR sensor pin
const int irSensorPin = A1; // Connect the OUT pin of the IR sensor to A1

// Keypad setup
const byte ROWS = 5; // five rows
const byte COLS = 4; // four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'} // Added '*' for the enter key
};
byte rowPins[ROWS] = {11, 12, 13, A0}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 8, 9, 10}; // connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int maxJobs = 40;
const int maxMachines = 5;
int numJobs;
int processingTimes[maxJobs][maxMachines]; // Assuming max 5 jobs and 5 machines
int dueDates[maxJobs];
char selectedHeuristic;

unsigned long maxJobExecutionTime = 60000; // Maximum job execution time in milliseconds (1 minute)

void setup() {
  lcd.init();  // Initialize the LCD
  lcd.backlight();
  lcd.print(" *_* Welcome *_* ");
  pinMode(irSensorPin, INPUT);
  while (digitalRead(irSensorPin) == HIGH) { // Wait until an object is NOT detected
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Waiting for IR");
    lcd.setCursor(0, 1);
    lcd.print("Object Detected");
    delay(500); // Add a small delay to avoid rapid screen updates
  }
  lcd.clear();
  lcd.print(" *_* Welcome *_* ");
  delay(2000); // Give some time for the user to see the welcome message
  lcd.clear(); // Clear the screen before proceeding

  for (int i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  
}

void loop() {
  while (digitalRead(irSensorPin) == HIGH) { // Wait until an object is NOT detected
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Waiting for IR");
    lcd.setCursor(0, 1);
    lcd.print("Object Detected");
    delay(500); // Add a small delay to avoid rapid screen updates
  }
  
  // Display "Permutation Flowshop" message
  lcd.clear();
  lcd.print("  Permutation ");
  lcd.setCursor(0, 1);
  lcd.print("    Flowshop  ");
  delay(3000);
   lcd.clear();
  for (int i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  // Object not detected, proceed with the main functionality
  lcd.clear();
  lcd.print("Enter no of jobs:");
  numJobs = readIntFromKeypad();
  numJobs = constrain(numJobs, 1, maxJobs); // Ensure numJobs is within 1 to maxJobs
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("No of jobs: ");
  lcd.print(numJobs);
  delay(2000);
  collectJobData();
  selectHeuristic();
  int jobSequence[maxJobs];
  applySelectedHeuristic(jobSequence);
  displayJobSequence(jobSequence);
  executeJobSequence(jobSequence);
}

int readIntFromKeypad() {
  int num = 0;            // Initialize the number to 0
  char key;               // Variable to store the key pressed

  while (true) {          // Infinite loop to keep reading keys
    key = keypad.getKey(); // Read the key pressed on the keypad

    // Check if the key is a digit between '0' and '9'
    if (key >= '0' && key <= '9') {
      lcd.print(key);     // Print the digit on the LCD screen
      num = (key - '0') + (0); // Update the number (assign the digit directly)
    }

    // Check if the key is the enter key ('#')
    if (key == '#') {
      lcd.clear();        // Clear the LCD screen
      return num;         // Return the number entered
    }
  }
}

void collectJobData() {
  for (int i = 0; i < numJobs; i++) {
    lcd.clear();
    lcd.print("Job ");
    lcd.print(i + 1);
    for (int j = 0; j < maxMachines; j++) {
      lcd.setCursor(0, 1);
      lcd.print("M");
      lcd.print(j + 1);
      lcd.print(" time:");
      processingTimes[i][j] = readIntFromKeypad();
      lcd.setCursor(0, 1);
      lcd.print("                ");
    }
    lcd.setCursor(0, 1);
    lcd.print("Due date:");
    dueDates[i] = readIntFromKeypad();
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
  lcd.clear();
}

void selectHeuristic() {
  lcd.print("Select Heuristic:");
  lcd.setCursor(0, 1);
  lcd.print("1:J 2:NEH 3:EDD");
  while (true) {
    char key = keypad.getKey();
    if (key == '1' || key == '2' || key == '3') {
      selectedHeuristic = key;
      break;
    }
  }
  lcd.clear();
}

void applySelectedHeuristic(int* sequence) {
  switch (selectedHeuristic) {
    case '1':
      applyJohnsonsRule(sequence);
      break;
    case '2':
      applyNEH(sequence);
      break;
    case '3':
      applyEDD(sequence);
      break;
  }
}

void applyJohnsonsRule(int* sequence) {
  // Placeholder implementation for Johnson's Rule
  // Replace with the actual Johnson's Rule algorithm
  int numOperations = numJobs * 2;
  int jobOrder[numOperations];
  bool used[maxJobs];
  memset(used, false, sizeof(used));

  // Collect all processing times in pairs
  struct Job {
    int id;
    int time1;
    int time2;
  } jobs[maxJobs];
  
  for (int i = 0; i < numJobs; i++) {
    jobs[i].id = i;
    jobs[i].time1 = processingTimes[i][0];
    jobs[i].time2 = processingTimes[i][1];
  }
  
  int index = 0;
  while (index < numJobs) {
    int minTime = INT_MAX;
    int selectedJob = -1;

    for (int i = 0; i < numJobs; i++) {
      if (!used[i]) {
        if (jobs[i].time1 < minTime) {
          minTime = jobs[i].time1;
          selectedJob = i;
        }
        if (jobs[i].time2 < minTime) {
          minTime = jobs[i].time2;
          selectedJob = i;
        }
      }
    }

    sequence[index++] = selectedJob;
    used[selectedJob] = true;
  }
}

void applyNEH(int* sequence) {
  // Placeholder implementation for NEH heuristic
  // Replace with the actual NEH algorithm
  
  // For now, just sort the jobs based on processing time sum
  int jobOrder[maxJobs];
  for (int i = 0; i < numJobs; i++) {
    jobOrder[i] = i;
  }
  
  // Simple bubble sort for demonstration purposes
  for (int i = 0; i < numJobs - 1; i++) {
    for (int j = 0; j < numJobs - i - 1; j++) {
      int sum1 = 0, sum2 = 0;
      for (int k = 0; k < maxMachines; k++) {
        sum1 += processingTimes[jobOrder[j]][k];
        sum2 += processingTimes[jobOrder[j + 1]][k];
      }
      if (sum1 < sum2) {
        int temp = jobOrder[j];
        jobOrder[j] = jobOrder[j + 1];
        jobOrder[j + 1] = temp;
      }
    }
  }
  
  // Copy the sorted job order to the sequence array
  for (int i = 0; i < numJobs; i++) {
    sequence[i] = jobOrder[i];
  }
}

void applyEDD(int* sequence) {
  // Placeholder implementation for EDD heuristic
  // Replace with the actual EDD algorithm
  for (int i = 0; i < numJobs; i++) {
    sequence[i] = i;
  }
  for (int i = 0; i < numJobs - 1; i++) {
    for (int j = i + 1; j < numJobs; j++) {
      if (dueDates[sequence[i]] > dueDates[sequence[j]]) {
        int temp = sequence[i];
        sequence[i] = sequence[j];
        sequence[j] = temp;
      }
    }
  }
}

void displayJobSequence(int* sequence) {
  lcd.print("Job Sequence:");
  for (int i = 0; i < numJobs; i++) {
    lcd.setCursor(i % 16, 1);
    lcd.print(sequence[i] + 1);
    if (i % 16 == 15) delay(1000); // Pause to allow reading if sequence exceeds LCD width
  }
  delay(5000);
  lcd.clear();
}

void executeJobSequence(int* sequence) {
  unsigned long startTime = millis();
  unsigned long jobStartTime[maxJobs];
  unsigned long jobEndTime[maxJobs];
  unsigned long machineEndTime[maxMachines] = {0, 0, 0, 0, 0};
  int completionTimes[maxMachines] = {0, 0, 0, 0, 0};
  unsigned long idleTime = 0;

  for (int j = 0; j < numJobs; j++) {
    jobStartTime[j] = millis() - startTime;
   for (int m = 0; m < maxMachines; m++) {
  // Display job and machine information on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Job ");
  lcd.print(sequence[j] + 1);
  lcd.print(" M");
  lcd.print(m + 1);
  lcd.setCursor(0, 1);
  lcd.print("Start:");
  lcd.print((millis() - startTime) / 1000);

  // Turn on the LED for the current machine
   digitalWrite(ledPins[m], HIGH);
      unsigned long jobExecutionStartTime = millis();
      unsigned long jobExecutionEndTime = millis();
      while ((jobExecutionEndTime - jobExecutionStartTime) < maxJobExecutionTime) {
        jobExecutionEndTime = millis();
        if (jobExecutionEndTime - jobExecutionStartTime < processingTimes[sequence[j]][m] * 1000) {
          delay(10); // Small delay for stability
        } else {
          break;
        }
      }
      digitalWrite(ledPins[m], LOW);
      
      completionTimes[m] = max(completionTimes[m], (m > 0 ? completionTimes[m - 1] : 0)) + processingTimes[sequence[j]][m];
      machineEndTime[m] = millis() - startTime;
    }
    jobEndTime[j] = millis() - startTime;
  }

  // Calculate makespan
  unsigned long makespan = jobEndTime[numJobs - 1] / 1000;

  // Calculate total flow time
  unsigned long totalFlowTime = 0;
  for (int j = 0; j < numJobs; j++) {
    totalFlowTime += jobEndTime[j] / 1000;
  }

  // Calculate idle time of machines
  for (int m = 0; m < 5; m++) {
    idleTime += (machineEndTime[m] / 1000) - (completionTimes[m] / 1000);
  }

  // Calculate lateness values
  int lateness[numJobs];
  for (int j = 0; j < numJobs; j++) {
    lateness[j] = (jobEndTime[j] / 1000) - dueDates[sequence[j]];
  }

  // Calculate total lateness
  int totalLateness = 0;
  for (int j = 0; j < numJobs; j++) {
    totalLateness += lateness[j];
  }

  // Display results
  lcd.clear();
  lcd.print("Makespan: ");
  lcd.print(makespan);
  delay(3000);
  lcd.clear();
  lcd.print("Total Flow: ");
  lcd.print(totalFlowTime);
  delay(3000);
  lcd.clear();
  lcd.print("Idle Time: ");
  lcd.print(idleTime);
  delay(3000);
  for (int j = 0; j < numJobs; j++) {
    lcd.clear();
    lcd.print("Job ");
    lcd.print(sequence[j] + 1);
    lcd.print(" Late: ");
    lcd.print(lateness[j]);
    delay(3000);
  }
  lcd.clear();
  lcd.print("Total Lateness: ");
  lcd.setCursor(0, 1);
  lcd.print(totalLateness);
  delay(5000);
  lcd.clear();
}
