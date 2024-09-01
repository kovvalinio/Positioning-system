
#define SIM_RST 5              ///< SIM808 RESET
#define SIM_RX 6               ///< SIM808 RXD
#define SIM_TX 7               ///< SIM808 TXD
#define SIM_PWR 8              ///< SIM808 PWRKEY
#define SIM_STATUS 3           ///< SIM808 STATUS
#define SIM808_BAUDRATE 38400  ///< Controls the serial baudrate between the arduino and the SIM808 module
#define SERIAL_BAUDRATE 38400  ///< Controls the serial baudrate between the arduino and the computer
#define BUFFER_SIZE 512        ///< Size of the buffer on the main program side
#define POSITION_STR_SIZE 24


char buffer[BUFFER_SIZE];
char position[POSITION_STR_SIZE];
int timeout = 1000;

bool readNext(bool line = false) {
  char c;
  char *p = buffer;
  memset(p, 0, BUFFER_SIZE);

  do {
    while (!Serial.available()) delay(100);

    c = Serial.read();
    if ((!line && isspace(c)) || c == '\n') break;

    *p = c;
    p++;
  } while (true);

  *p = '\n';
  Serial1.print(buffer);
  Serial.println("");
  Serial.print(buffer);
  return c == '\n';
}

void readNext_sim808(char *buffer = buffer, size_t size = BUFFER_SIZE, uint16_t timeout = 500, char stop = 0) {
  size_t i = 0;
  bool exit = false;
  memset(buffer, 0, BUFFER_SIZE);
  Serial.println("odczytywanie...");

  do {
    while (!exit && i < size - 1 && Serial1.available()) {
      char c = Serial1.read();
      buffer[i] = c;
      i++;

      exit |= stop && c == stop;
    }

    if (timeout) {
      if (timeout) {
        delay(1);
        (timeout)--;
      }

      if (!(timeout)) break;
    }
  } while (!exit && i < size - 1);

  buffer[i] = '\0';

  if (i) {
    Serial.print(F("<--"));
    Serial.print(buffer);
  }
}

void readNext_sim808_silent(char *buffer = buffer, size_t size = BUFFER_SIZE, uint16_t timeout = 500, char stop = 0) {
  size_t i = 0;
  bool exit = false;
  memset(buffer, 0, BUFFER_SIZE);
  // Serial.println("odczytywanie...");

  do {
    while (!exit && i < size - 1 && Serial1.available()) {
      char c = Serial1.read();
      buffer[i] = c;
      i++;

      exit |= stop && c == stop;
    }

    if (timeout) {
      if (timeout) {
        delay(1);
        (timeout)--;
      }

      if (!(timeout)) break;
    }
  } while (!exit && i < size - 1);

  buffer[i] = '\0';

  if (i) {
    // Serial.print(F("<--"));
    // Serial.print(buffer);
  }
}

void blink_for_ready(){
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(6, HIGH);
  delay(2000);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(6, LOW);
}

void parse_location_send_via_tcp(){
  for(int i = 0 ; i< BUFFER_SIZE; i++){
      if(buffer[i] == ','){
        for(int k = 0 ; k < POSITION_STR_SIZE ; k++){
          position[k] = buffer[i+k+1];
        }
        position[23] = '\0';
        break;
      }
  }
  delay(2000);
  Serial1.print("AT+CIPSEND=23\n");
  Serial.print("AT+CIPSEND=23\n");
  readNext_sim808();
  delay(2000);
  Serial1.print(position);
  Serial.println(position);
  readNext_sim808();
}


void change_states(){
  // int received_no = parse();
  // if(received_no >=0 && received_no <=15){
  //   if(received_no & 1){ 
  //     digitalWrite(2, HIGH);
  //   }
  //   else{
  //     digitalWrite(2, LOW);
  //   }
  //   if(received_no & 2){ 
  //     digitalWrite(3, HIGH);
  //   }
  //   else{
  //     digitalWrite(3, LOW);
  //   }
  //   if(received_no & 4){ 
  //     digitalWrite(4, HIGH);
  //   }
  //   else{
  //     digitalWrite(4, LOW);
  //   }
  //   if(received_no & 8){ 
  //     digitalWrite(6, HIGH);
  //   }
  //   else{
  //     digitalWrite(6, LOW);
  //   }
  // }
}


void setup() {
  // put your setup code here, to run once:
  Serial1.begin(SIM808_BAUDRATE);
  Serial.begin(SERIAL_BAUDRATE);
  pinMode(SIM_PWR, OUTPUT);
  delay(2000);
  digitalWrite(SIM_PWR, HIGH);
  delay(2000);
  digitalWrite(SIM_PWR, LOW);


  delay(2000);
  Serial1.print("AT+CGNSPWR=1\n");
  Serial.print("AT+CGNSPWR=1\n");
  readNext_sim808();
  
  delay(2000);
  Serial1.print("AT+CGNSSEQ=\"RMC\"\n");
  Serial.print("AT+CGNSSEQ=\"RMC\"\n");
  readNext_sim808();

  delay(2000);
  Serial1.print("AT+CGNSCMD=1,\"24504D544B3030302A33320D0A\"\n");
  Serial.print("AT+CGNSCMD=1,\"24504D544B3030302A33320D0A\"\n");
  readNext_sim808();

  // tcp must be on 
  for(;;)
  {
    delay(2000);
    Serial1.print("AT+CGPSSTATUS?\n");
    Serial.print("AT+CGPSSTATUS?\n");
    readNext_sim808();

    if(strstr(buffer,"Location Not Fix") == NULL)
    {
      Serial.println("Location found!");
      break;
    }
    else
    {
      Serial.println("Searching for location...");
    }
  }
}
void atach_tcp(){
  delay(2000);
  Serial1.print("AT\n");
  Serial.print("AT\n");
  readNext_sim808();
  delay(5000);
  Serial1.print("AT+CSTT=\"internet\",\"internet\",\"internet\"\n");
  Serial.print("AT+CSTT=\"internet\",\"internet\",\"internet\"\n");
  readNext_sim808();
  delay(10000);
  Serial1.print("AT+CIICR\n");
  Serial.print("AT+CIICR\n");
  readNext_sim808();
  delay(5000);
  Serial1.print("AT+CIFSR\n");
  Serial.print("AT+CIFSR\n");
  readNext_sim808();
  delay(5000);
  Serial1.print("AT+CIPSTART=\"TCP\",\"<ip>\",\"<port>\"\n");
  Serial.print("AT+CIPSTART=\"TCP\",\"<ip>\",\"<port>\"\n");
  readNext_sim808();
}

void loop() {

  while (true) {
    readNext();
    if (buffer[0] == '9') {
      Serial.println("koniec kontroli");
      atach_tcp();
      break;
    }
    delay(1000);
    readNext_sim808();
  }
  while (true) {
    Serial1.print("AT+CGPSINF=0\n");
    Serial.println("AT+CGPSINF=0");
    delay(2000);
    readNext_sim808();
    parse_location_send_via_tcp();
    readNext_sim808();
  }
}
