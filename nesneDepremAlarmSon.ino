#include <Wire.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define WIFI_SSID "telefonmk"
#define WIFI_PASSWORD "12345678"
// Defining the WiFi channel speeds up the connection:
#define WIFI_CHANNEL 6

MPU6050 MPU;
int16_t GyroX, GyroY, GyroZ;
int buzzer = 14;
int buzzerState = 0;


const char *authorizedUsername = "admin";  // Web arayüzüne erişim için kullanıcı adı
const char *authorizedPassword = "password123";  // Web arayüzüne erişim için parola

WebServer server(80);

const int LED1 = 33;

void sendHtml() {
  String response = R"(
  <!DOCTYPE html>
  <html lang='tr'>

  <head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <style>
      body {
        background-color: green; 
        margin: 0;
        padding: 0;
        font-family: Arial, sans-serif;
      }

      .warning-text {
        text-align: center;
        margin-top: 50px;
        font-size: 18px;
        color: white; /* Yazı rengini beyaz yap */
        text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.5); /* Yazıya kabartma efekti ekle */
      }

      #notification-message {
        text-align: center;
        margin-top: 300px;
        font-size: 40px;
      }

      #notification-message1 {
        text-align: center;
        margin-top: 100px;
        font-size: 40px;
      }

    </style>
    <script>
      function showWarningMessage() {
        document.getElementById('notification-message').innerText = 'Deprem Olduğunda Bildirim Alacaksınız';
        document.getElementById('notification-message1').innerText = 'Sarsıntı Olduğunda Bildirim Alacaksanız';
      }
    </script>
  </head>

  <body>
    <div id='notification-message' class='warning-text'>DEPREM UYARI SİSTEMİ</div>
    <div id='notification-message1' class='warning-text'>Sarsıntı Olduğunda Bildirim Alacaksanız</div>
  </body>

  </html>
 )";
  server.send(200, "text/html", response);
}

void setup(void) {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  Wire.begin();
  MPU.initialize();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", sendHtml);

  server.begin();
  Serial.println("HTTP server started");
}

void showWarningMessage() {
  String response = R"(
    <!DOCTYPE html>
    <html lang='tr'>
    <head>
      <meta charset='UTF-8'>
      <meta name='viewport' content='width=device-width, initial-scale=1.0'>
      <style>
        body {
          background-color: red;
          margin: 0;
          padding: 0;
          font-family: Arial, sans-serif;
          display: flex;
          align-items: center;
          justify-content: center;
          height: 100vh;
        }

        h1 {
          color: black;
          text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.5);
          font-size: 40px;
        }

        script {
          background-color: red;
        }
      </style>
      <script>

          alert('DEPREM ALARMI!');
      
      </script>
    </head>
    <body>
      <h1>!!!!!!!!!!!! DEPREM DİKKATT !!!!!!!!!!!</h1>
    </body>
    </html>
  )";
  server.send(200, "text/html", response);
}

void loop(void) {
  server.handleClient();
  MPU.getRotation(&GyroX, &GyroY, &GyroZ);
  
  if (GyroX < -1000 || GyroX > 1000 || GyroY > 1000 || GyroY < -1000 || GyroZ > 1000 || GyroZ < -1000) {
    for (int i = 0; i < 10; i++) {
      tone(buzzer, 1000);
      digitalWrite(33, HIGH);
      
      showWarningMessage();

      Serial.print(GyroX);
      Serial.print("-");
      Serial.print(GyroY);
      Serial.print("-");
      Serial.println(GyroZ);
      delay(50);
      noTone(buzzer);
            
      digitalWrite(33, LOW);
      delay(50);
    }
    Serial.println("Deprem");
 
  } else {
    noTone(buzzer);
    digitalWrite(33, LOW);
    
    }  
}