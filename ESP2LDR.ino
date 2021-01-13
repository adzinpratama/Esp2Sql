#include <ESP8266WiFi.h>
#include <DHT.h> 

#define DHTPIN D2                                            // Digital pin connected to DHT11
#define DHTTYPE DHT11                                        // Initialize dht type as DHT 11
DHT dht(DHTPIN, DHTTYPE); 
//Konfigurasi WiFi
const char *ssid = "Hapra-Networking";
const char *password = "Mkn123siap";

//IP Address Server yang terpasang XAMPP
const char *host = "192.168.1.101";

              

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Jika koneksi berhasil, maka akan muncul address di serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

int value = 0;

void loop() {
  // Proses Pengiriman -----------------------------------------------------------
  delay(1000);
  ++value;

  // Membaca Sensor DHT dan LDR -------------------------------------------------------
  float h = dht.readHumidity();                                 // Read Humidity
  float t = dht.readTemperature();                              // Read temperature
  String ldr;
  int adcvalue = analogRead(A0); //Read Analog value of LDR
  ldr = String(adcvalue);   //String to interger conversion
  
  if (isnan(h) || isnan(t))                                     // Checking sensor working
  {                                   
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print("connecting to ");
  Serial.println(host);

// Mengirimkan ke alamat host dengan port 80 -----------------------------------
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

// Isi Konten yang dikirim adalah alamat ip si esp -----------------------------
  String url = "/esptosql/save.php?suhu=";
  url = url + h + "&kelembapan=";
  url = url + t + "&ldr=";
  url += ldr; 

  Serial.print("Requesting URL: ");
  Serial.println(url);

// Mengirimkan Request ke Server -----------------------------------------------
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 1000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

// Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");
}
