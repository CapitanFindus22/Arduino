//SINCE I USED PORT MANIPULATION THIS WILL ONLY WORK ON ARDUINO UNO

//Store the data sent from the sensor
volatile bool arr[40];

//Used to save the bits in the array
volatile int counter = 0;

//Used to synchronize the communication
volatile unsigned long time;

//Temperature and humidity
float temp, hum;

void setup() {

  Serial.begin(9600);

  delay(1000);
}

void loop() {

  read();

  delay(2000);
}

//Read from the DHT11, the timing can be seen at
//https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf
void read() {

  //Port d2 as output and high
  DDRD |= (1 << 2);
  PORTD |= (1 << 2);
  wait(100, true);

  //Port d2 low
  PORTD &= !(1 << 2);
  wait(18, true);

  //Port d2 HIGH
  PORTD |= (1 << 2);
  wait(30, false);

  //Port d2 as input and wait
  DDRD &= !(1 << 2);
  wait(160, false);

  //Wait for all the data
  attachInterrupt(digitalPinToInterrupt(2), save, RISING);
  while (counter < 40) { asm(""); }
  detachInterrupt(digitalPinToInterrupt(2));

  //Reset the counter
  counter = 0;

  temp = 0;
  hum = 0;

  //Humidity integer part
  for (; counter < 8; counter++) {

    hum += arr[counter] * pow(2, 8 - counter);
  }

  //Humidity decimal part
  for (; counter < 16; counter++) {

    hum += arr[counter] * pow(2, -counter % 9);
  }

  //Temperature integer part
  for (; counter < 24; counter++) {

    temp += arr[counter] * pow(2, 8 - counter % 8);
  }

  //Temperature decimal part
  for (; counter < 32; counter++) {

    temp += arr[counter] * pow(2, -counter % 9);
  }

  /*The checksum should be verified but i didn't find a way to make it work,
  could be a problem with my sensor*/

  print();

  counter = 0;

  return;
}

//Save the stream of bits in the array
void save() {

  time = micros();

  while (PIND & 4) {}

  arr[counter] = (micros() - time > 28) ? 1 : 0;

  counter++;
}

//Non blocking wait function, pass true if milliseconds or false for microseconds
void wait(unsigned int t, bool milliseconds) {

  if (milliseconds) {

    time = millis();

    while (millis() - time < t) {}

  }

  else {

    time = micros();

    while (micros() - time < t) {}
  }
}

//Print the values
void print() {

  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print("°C - ");
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println("% ");
}