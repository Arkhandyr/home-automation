const int tmpSensor = A0;
const int motorExaustPin = A1;
const int luzSensor = A3;
const int gasSensor = A4;
const int lampada = 7;
const int movSensor = 8;
const int buzzer = 9;
const int motorVentPin = 10;

bool estadoSensor = false;

int gatilhoTemp = 0;
int gatilhoLuz = 0;
int gatilhoGas = 306;

float temperatura = 0;
int luminosidade = 0;
int presenca = 0;
int gas = 0;

String receivedJsonString = "";

void setup() {
  pinMode(motorExaustPin, OUTPUT);
  pinMode(motorVentPin, OUTPUT);
  pinMode(lampada, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  temperatura = verificarTemperatura();
  luminosidade = verificarLuminosidade();
  presenca = verificarPresenca();
  gas = verificarGas();

  String jsonString = String(
      "{\"temperatura\":") + temperatura + ", " +
  	  "\"luminosidade\":" + luminosidade + ", " +
  	  "\"presenca\":" + presenca + ", " +
  	  "\"gas\":" + gas + "}";
  
  Serial.println(jsonString);
  
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
  
  delay(1000);
}

void processJsonString(const String& jsonString) {
  int temperaturaPos = jsonString.indexOf("\"gatilhoTemp\":");
  int luminosidadePos = jsonString.indexOf("\"gatilhoLuz\":");
  int gasPos = jsonString.indexOf("\"gatilhoGas\":");

  gatilhoTemp = extractValue(jsonString, temperaturaPos);
  gatilhoLuz = extractValue(jsonString, luminosidadePos);
  gatilhoGas = extractValue(jsonString, gasPos);
}

float extractValue(const String& jsonString, int position) {
  int start = jsonString.indexOf(':', position) + 1;
  int end = jsonString.indexOf(',', start);
  if (end == -1) {
    end = jsonString.indexOf('}', start);
  }
  return jsonString.substring(start, end).toFloat();
}

float verificarTemperatura() {
  int tmpSensorValue = analogRead(tmpSensor);
  float celsiusTemp = (((tmpSensorValue / 1024.0) * 5) - 0.5) * 100;
  turnMotorOnIfHot(celsiusTemp);
  
  return celsiusTemp;
}

void turnMotorOnIfHot(float celsiusTemp){
  if (celsiusTemp > gatilhoTemp + 30 && presenca == 1) {
  	analogWrite(motorVentPin, 255);
  }
  else if ((gatilhoTemp + 10 < celsiusTemp && celsiusTemp < gatilhoTemp + 30) && presenca == 1) {
  	analogWrite(motorVentPin, 170);
  }
  else if ((gatilhoTemp < celsiusTemp && celsiusTemp < gatilhoTemp + 10) && presenca == 1) {
  	analogWrite(motorVentPin, 85);
  }
  else {
  	analogWrite(motorVentPin, 0);
  }
}

int verificarLuminosidade() {
  int luzValue = analogRead(luzSensor);
  turnLightOnIfDark(luzValue);
  
  return luzValue;
}

void turnLightOnIfDark(int luzValue){
  if (luzValue > gatilhoLuz && presenca == 1) {
  	digitalWrite(lampada, HIGH);
  }
  else {
  	digitalWrite(lampada, LOW);
  }
}

int verificarPresenca() {
  int presenca = digitalRead(movSensor);
  
  if (presenca == 1) {
    if (estadoSensor == false) {
      estadoSensor = true;
    }
  } else {
    if (estadoSensor == true) {
      estadoSensor = false;
    }
  }
  
  return presenca;
}

int verificarGas() {
  int gasValue = analogRead(A4);
  
  if (gasValue > gatilhoGas) {
    tone(buzzer, 550);
    delay(500);
    noTone(buzzer);
    digitalWrite(motorExaustPin, HIGH);
    digitalWrite(lampada, LOW);
  } 
  else {
    noTone(buzzer);
    digitalWrite(motorExaustPin, LOW);
  }
  
  return gasValue; 
}
