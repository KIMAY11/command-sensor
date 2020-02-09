#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

const char* ssid = "AndroidAP_6200";// Your wifi Name       
const char* password = "1234567890!";// Your wifi Password
const char* host2 = "technofarmers.000webhostapp.com"; 
const char* host = "192.168.43.149";// IP Server

const int httpPort = 80;

const char* Commands;  
const char* Commands1;  

int floatsensor = 2;
int buttonState;
String level;
String url;
boolean status = true;


void setup() {
  pinMode(floatsensor, INPUT_PULLUP); //Arduino Internal Resistor 10K
  Serial.begin(115200);                     // initialize serial:
  Serial.println("");
  Serial.println("Client-------------------------------");
  Serial.print("Connecting to Network");
  WiFi.mode(WIFI_STA);                      // Mode Station
  WiFi.begin(ssid, password);               // Matching the SSID and Password
  delay(1000);

  // Waiting to connect to wifi
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  send_commands();
  Serial.println("");
  Serial.println("Successfully Connecting");  
  Serial.println("Status : Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("-------------------------------------");
  Serial.println("");
}

void loop() {
    buttonState = digitalRead(floatsensor);  // read the value of float sensor
    if(buttonState == LOW)
    {
      if(Commands1 == "High") {
        status = true;  
      }
      Commands = "LED_Off";
      Commands1 = "Low";
      Serial.println(Commands);
      Serial.println( "LOW");
      Serial.println(digitalRead(floatsensor)); 
      send_commands(); 
    } else {
      if(Commands1 == "Low") {
        status = true;  
      }
      Commands = "LED_On";
      Commands1 = "High";
      Serial.println(Commands);
      Serial.println("HIGH");
      Serial.println(digitalRead(floatsensor));
      send_commands();
     }
  delay(3000);

  if (status) {
     send_data_to_host();
     status = false;
   }
}


void send_data_to_host(){
    //Sending data to database
   Serial.print("connecting to ");
   Serial.println(host2);
   WiFiClient client;
   if (!client.connect(host2, httpPort)) {
     Serial.println("connection failed");
     return;
   }

    if (Commands1 == "Low"){
      url = "/api/insert.php?status=" + String(Commands1);
   } else {
      url = "/api/update.php?status=" + String(Commands1);
   }
   
   Serial.print("Requesting URL: ");
   Serial.println(url);
   client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                "Host: " + host2 + "\r\n" +
                "Connection: close\r\n\r\n");
   delay(500);
   while (client.available()) {
     String line = client.readStringUntil('\r');
     Serial.print(line);
   }
   Serial.println();
   Serial.println("closing connection");
   delay(3000);
 }

void send_commands()
{
    Serial.println("Sending command...");
    Serial.println("Don't press the button for now...");
    Serial.println("");
    Serial.print("Connecting to ");
    Serial.println(host);
  
//    Use WiFiClient class to create TCP connections
    WiFiClient client;
    if (!client.connect(host, httpPort)) {
      Serial.println("Connection failed");
      return;
    }
  
    // We now create a URI for the request  
    Serial.print("Requesting URL : ");
    Serial.println(Commands);
  
    // This will send the request to the server
    client.print(String("GET ") + Commands + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: Close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {      
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    Serial.print("Server Reply = "); 
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
   }
  Serial.println("Now you can press the button ...");
  Serial.println("-------------------------------------");
  Serial.println("");
}
