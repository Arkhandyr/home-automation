#include <Adafruit_LiquidCrystal.h>
#include <Keypad.h>

const int red = 5;
const int blue = 4;
const int green = 3;
Adafruit_LiquidCrystal lcd(0);

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {A0, A1, 11, 10}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

float temp = 0;
int luminosidade = 0;
int presenca = 0;
int gas = 0;

String receivedJsonString = "";

void setup()
{
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT); 
  lcd.begin(16, 2);
  Serial.begin(9600);
}

void loop()
{
  getValueFromKeypad();
    
  while (Serial.available() > 0) {
    char incomingChar = Serial.read();
    if (incomingChar == '}') {
      receivedJsonString += incomingChar;

      processJsonString(receivedJsonString);

      receivedJsonString = "";
    } else {
      receivedJsonString += incomingChar;
    }
  }
}

void processJsonString(const String& jsonString) {
  int temperaturaPos = jsonString.indexOf("\"temperatura\":");
  int luminosidadePos = jsonString.indexOf("\"luminosidade\":");
  int presencaPos = jsonString.indexOf("\"presenca\":");
  int gasPos = jsonString.indexOf("\"gas\":");

  temp = extractValue(jsonString, temperaturaPos);
  luminosidade = extractValue(jsonString, luminosidadePos);
  presenca = extractValue(jsonString, presencaPos);
  gas = extractValue(jsonString, gasPos);
  
  setLedColor(temp);
}

float extractValue(const String& jsonString, int position) {
  int start = jsonString.indexOf(':', position) + 1;
  int end = jsonString.indexOf(',', start);
  if (end == -1) {
    end = jsonString.indexOf('}', start);
  }
  return jsonString.substring(start, end).toFloat();
}

void printTemp(float temp) {
  int intPart = int(temp);
  int decPart = int((temp - intPart) * 10);
  
  lcd.print(intPart);
  lcd.print(".");
  lcd.print(decPart);
  lcd.print("C");
}

void getValueFromKeypad() {
  
  char key = keypad.getKey();
  
  if (key == 'A'){
    lcd.clear();
    lcd.print("Temperatura: ");
    lcd.setCursor(0,1);
    printTemp(temp);
  	delay(1000);
  	lcd.clear();
  }
  else if (key == 'B'){
    lcd.clear();
    lcd.print("Luminosidade: ");
    lcd.setCursor(0,1);
    lcd.print(luminosidade);
  	delay(1000);
  	lcd.clear();
  }
  else if (key == 'C'){
    lcd.clear();
    lcd.print("Presenca: ");
    lcd.setCursor(0,1);
    lcd.print(presenca);
  	delay(1000);
  	lcd.clear();
  }
  else if (key == 'D'){
    lcd.clear();
    lcd.print("Gas: ");
    lcd.setCursor(0,1);
    lcd.print(gas);
  	delay(1000);
  	lcd.clear();
  }
  else if (key == '*') {
    lcd.clear();
    lcd.print("Temperatura:");
    lcd.setCursor(0,1);
    String gatilhoTemp = "";
    for (int i = 0; i < 2; ++i) {
      char tempValue = keypad.waitForKey();
      lcd.print(tempValue);
      gatilhoTemp += String(tempValue);
      delay(100);
  	}
    
    lcd.clear();
    
    lcd.print("Luminosidade:");
    lcd.setCursor(0,1);
    String gatilhoLuz = "";
    for (int i = 0; i < 3; ++i) {
      char luzValue = keypad.waitForKey();
      lcd.print(luzValue);
      gatilhoLuz += String(luzValue);
      delay(100);
  	}
    
    lcd.clear();
    
    lcd.print("Gas:");
    lcd.setCursor(0,1);
    String gatilhoGas = "";
    for (int i = 0; i < 3; ++i) {
      char gasValue = keypad.waitForKey();
      lcd.print(gasValue);
      gatilhoGas += String(gasValue);
      delay(100);
  	}
    
    lcd.clear();
    
    lcd.print("Configuracao");
    lcd.setCursor(0,1);
    lcd.print("completa...");
    delay(1000);
    lcd.clear();
    
    String jsonString = String(
      "{\"gatilhoTemp\": " + gatilhoTemp +
      ", \"gatilhoLuz\": " + gatilhoLuz +
      ", \"gatilhoGas\": " + gatilhoGas + "}");
  
  	Serial.println(jsonString);
  }
}