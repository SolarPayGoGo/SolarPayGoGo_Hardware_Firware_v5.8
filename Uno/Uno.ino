byte output_on_message  [3] [8] = {{0x01,0x43,0x90,0x1e,0x00,0x28,0x09,0x1d},{0x01,0x43,0x90,0x46,0x00,0x28,0x88,0xce},{0x01,0x05,0x00,0x02,0xff,0x00,0x2d,0xfa}};

byte output_off_message [3] [8] = {{0x01,0x43,0x90,0x1e,0x00,0x28,0x09,0x1d},{0x01,0x43,0x90,0x46,0x00,0x28,0x88,0xce},{0x01,0x05,0x00,0x02,0x00,0x00,0x6c,0x0a}};

byte obtain_data_message [5] [8] = {{0x01,0x04,0x32,0x00,0x00,0x03,0xbe,0xb3},{0x01,0x02,0x20,0x00,0x00,0x01,0xb2,0x0a},{0x01,0x43,0x31,0x00,0x00,0x1b,0x0a,0xf2},{0x01,0x04,0x33,0x1a,0x00,0x03,0x9e,0x88},{0x01,0x04,0x33,0x02,0x00,0x12,0xde,0x83}};

const int RS485_mode = 6;
const int data_request = 5;
const int load_connect = 2;

int last_load_connect_state = LOW;

void setup() {
  // put your setup code here, to run once:
  pinMode(load_connect,INPUT);
  pinMode(data_request,INPUT);
  pinMode(RS485_mode,OUTPUT);
  pinMode(13,OUTPUT);

  digitalWrite(RS485_mode,LOW);
  digitalWrite(13,HIGH);
  Serial.begin(115200);

  for (int k = 0; k<3 ; k++){
    digitalWrite(6,HIGH);
    delay(1);
    Serial.write(output_off_message [k],sizeof(output_off_message [k]));
    delay(1);
    digitalWrite(6,LOW);
    delay(100);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  int load_connect_state = digitalRead(load_connect);
  if (load_connect_state!= last_load_connect_state){
    last_load_connect_state = load_connect_state;
    if (load_connect_state == HIGH){
      delay(5);
      for (int k = 0; k<3 ; k++){
        digitalWrite(6,HIGH);
        delay(1);
        Serial.write(output_on_message [k],sizeof(output_on_message [k]));
        delay(1);
        digitalWrite(6,LOW);
        delay(100);
      }
    }else{
      delay(5);
      for (int k = 0; k<3 ; k++){
        digitalWrite(6,HIGH);
        delay(1);
        Serial.write(output_off_message [k],sizeof(output_off_message [k]));
        delay(1);
        digitalWrite(6,LOW);
        delay(100);
      }
    }
  }
  

  if (digitalRead(data_request)==HIGH){
    delay(5);
    for (int k = 0; k<5 ; k++){
      digitalWrite(6,HIGH);
      delay(1);
      Serial.write(obtain_data_message [k],sizeof(obtain_data_message [k]));
      delay(1);
      digitalWrite(6,LOW);
      delay(100);
    }
  }

  

}
