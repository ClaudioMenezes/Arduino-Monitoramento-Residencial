#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

/*##################################
  # FINGERPRINT SETTINHGS
  ##################################*/
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
bool fingerPrintValid = true;

/*##################################
  # PIR SETTINHGS
  ##################################*/

int PIRSala = 4;
int PIRCozinha = 5;
int PIRDormitorio_casal = 6;
int PIRDormitorio_1 = 7;
int calibrationTime = 5;
bool enviouSala = false;
bool enviouCozinha = false;
bool enviouDormitorioCasal = false;
bool enviouDormitorio1 = false;

/*##################################
  # ETHERNET SETTINHGS
  ##################################*/

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// IPAddress server(45, 55, 185, 215);
char server[] = "www.brunotome.com"; // Change to your server here
String data;

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);

/**
   Send a post to my server with the data parameters
*/
void sendPost(String typeAlert, String location) {
  /* Initialize the Ethernet client library with the IP address and port of the server
    that you want to connect to (port 80 is default for HTTP): */
  EthernetClient client;
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }

  delay(1000);

  /*
     id = 1 // pk of the user Bruno Tome
     &typeAlert = arrombamento || movimentacao
     &location = sala || cozinha || dormitorio_1 || dormitorio_2 || dormitorio_casal
  */
  data = "id=1&typeAlert=" + typeAlert + "&location=" + location;

  if (client.connect(server, 80)) {
    client.println("POST /projects/embarcados/update.php HTTP/1.1"); // Change to your post address here
    client.println("Host: www.brunotome.com");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);
    Serial.println("POST enviado: " + data);
  } else {
    Serial.println("Falhou ao conectar");
  }
}

/**
   Check movements on any PIR sensor and call the sendPost method with
   the alertType and location
*/
void checkMovement() {
  if ((digitalRead(PIRSala) == HIGH) && enviouSala == false) {
    enviouSala = true;
    sendPost("movimentacao", "sala");
  }

  if ((digitalRead(PIRCozinha) == HIGH) && enviouCozinha == false) {
    enviouCozinha = true;
    sendPost("movimentacao", "cozinha");
  }

  if ((digitalRead(PIRDormitorio_casal) == HIGH) && enviouDormitorioCasal == false) {
    enviouDormitorioCasal = true;
    sendPost("movimentacao", "dormitorio_casal");
  }

  if ((digitalRead(PIRDormitorio_1) == HIGH) && enviouDormitorio1 == false) {
    enviouDormitorio1 = true;
    sendPost("movimentacao", "dormitorio_1");
  }
}

/**
   Check if inputed finger match with the id of the owner
*/
bool getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return false;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return false;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return false;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return true;
}

void setup() {
  while (!Serial);
  // Open serial communications and wait for port to open:
  Serial.begin(250000);
  Serial.println("Inicializando...");
  pinMode(PIRSala, INPUT);
  pinMode(PIRCozinha, INPUT);
  pinMode(PIRDormitorio_casal, INPUT);
  pinMode(PIRDormitorio_1, INPUT);
  digitalWrite(PIRSala, LOW);
  digitalWrite(PIRCozinha, LOW);
  digitalWrite(PIRDormitorio_casal, LOW);
  digitalWrite(PIRDormitorio_1, LOW);
  //give the sensor some time to calibrate
  Serial.print("Calibrando sensores PIR ");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" pronto");
  // set the data rate for the sensor serial port, must be 57600
  finger.begin(57600);
  delay(1000);
}

void loop() {
  if (fingerPrintValid) {
    Serial.println("Aguardando uma digital valida para ativar o sistema");
    if (getFingerprintIDez()) {
      Serial.println("Sistema ativado");
      fingerPrintValid = false;
      enviouSala = false;
      enviouCozinha = false;
      enviouDormitorioCasal = false;
      enviouDormitorio1 = false;
    }
  } else {
    checkMovement();
    if (getFingerprintIDez()) {
      fingerPrintValid = true;
      Serial.println("Sistema desativado");
    }
  }

  // Do again after 1 second
  delay(1000);
}
