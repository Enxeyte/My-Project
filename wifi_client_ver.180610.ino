
#include <ESP8266WiFi.h>

const char* ssid     = "_SSID_";     // ssid of wifi
const char* password = "_PASSWORD_"; // password of wifi

const char* host     = "_host_ip_";  // host ip
const int   port     = _port_;

// serial connect & connect wifi
void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// st 0 : IDLE, st 1 : 급수, st 2 : 급수끝,
// st 3 : 탈수, st 4 : 탈수끝 st 5 : 최종 탈수
// 급수->세탁->물버림
// 급수->헹굼->물버림->탈수
// 급수->헹굼->물버림->탈수
// 급수->헹굼->물버림->최종 탈수
int prev_st = 0;
int st = 0;
int st_cnt = 0;

void loop() {
   // state 0 : available, state 1 : running, state 2 : spin_dry
  char* state = "idle";

  delay(1000);
  get_state();

  // st에 따른 state 출력
  switch(st){     // num  state
    case 0 : state = "0 available"; break;
    case 1 : state = "0 running";   break;
    case 2 : state = "0 running";   break;
    case 3 : state = "0 running";   break;
    case 4 : state = "0 running";   break;
    case 5 : state = "0 spin_dry";  break;
    default: state = "0 error";     break;
  }
  Serial.print("STATE : ");
  Serial.println(state);
  Serial.println();
  
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  
  // This will send the request to the server
  client.print(state);
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  // disconnect client
  client.stop();
  Serial.println();
  Serial.println("closing connection");
}

// read sensor val & get present state
void get_state() {
  double val;
 
  int eff = 0;    // 1000회 중 유효 진동 횟수
  int avgeff = 0; // 평균 유효 진동 횟수
  int numeff = 0; // 유효 진동이 잡힌 횟수
  int num0   = 0; // 유효 진동이 잡히지 않은 횟수

  int sampleNum = 1000;  // sample 횟수
  int i = 0; // loop 변수
  
  // ----------check avg eff val----------------
while(1) {
    eff = 0;
    // read 1000 time, check eff num
    for(i=0; i<sampleNum; i++)
    {
      val = analogRead(0);
      if (val<900)
      {
        eff++;
      }
    }
    // 유효 진동이 있는 경우, error 제외
    if ((eff>0)&(eff<1000)) {
      avgeff += eff;
      numeff++;
      num0 = 0;
      if (numeff == 50) { // 유효 진동 횟수가 50번이 되면
        avgeff = avgeff/numeff;
        break;            // avgeff 계산 후 while 탈출
      }
    }
    // 유효 진동이 없는 경우
    else if (eff == 0) {
      num0++;
      if (num0 == 100) {  // 10초 연속으로 진동이 없을 경우
        avgeff = 0;
        break;  
      }
    }
    delay(100);
  }
 
  // print avgEff
  Serial.print("\t average efficient num = ");
  Serial.println(avgeff, DEC);

  //-------------state ---------------------
  // save previous state
  prev_st = st;

  // when IDLE
  if (st == 0){
    if (avgeff > 100) st = 1; // st -> 급수
    else              st = 0; // st -> IDLE
  }
  // when 급수
  else if (st == 1){
    if (avgeff == 0)  st = 2; // st -> 급수 끝
    else              st = 1; // st -> 급수
  }
  // when 급수 끝
  else if (st == 2){
    if (avgeff > 100){
      st_cnt++; // n 번째 탈수
      if (st_cnt < 3) st = 3; // st -> 탈수
      else            st = 5; // st -> 최종 탈수 (3번째 탈수)
    }
    else              st = 2; // st -> 급수 끝
  }
  // when 탈수
  else if (st == 3){
    if (avgeff == 0)  st = 4; // st -> 탈수 끝
    else              st = 3; // st -> 탈수
  }
  // when 탈수 끝
  else if (st == 4){
    if (avgeff > 100) st = 1; // st -> 급수
    else              st = 4; // st -> 탈수 끝
  }
  // when 최종 탈수
  else if (st == 5){
    if (avgeff == 0)  st = 0; // st -> IDLE
    else              st = 5; // st -> 최종 탈수
    st_cnt = 0;
  }
  
  Serial.print("st : ");
  Serial.print(st, DEC);
  switch(st){
   case 0 : Serial.println(" IDLE"); break;
   case 1 : Serial.println(" 급수"); break;
   case 2 : Serial.println(" 급수 끝"); break;
   case 3 : Serial.println(" 탈수"); break;
   case 4 : Serial.println(" 탈수 끝"); break;
   case 5 : Serial.println(" 최종 탈수"); break;
   default : Serial.println(" Error!"); break;
  }
}

