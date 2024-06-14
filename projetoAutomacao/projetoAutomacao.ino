#include "DHT.h"
#include <WiFi.h>
#include <wifiSetup.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <openMeteo.h>
#include <realTime.h>
#include "SinricPro.h"
#include "SinricProTemperaturesensor.h"

//------------------------------------------------------------------------
TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;
TaskHandle_t Task4;
TaskHandle_t Task5;
TaskHandle_t Task6;

// //------------------------------------------------------------------------
// //CONFIGURACAO DISPLAY
#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// //-----------------------------------------------------------------------

// //------------------------------------------------------------------------
//PARAMETROS DO APP
#define APP_KEY "63d9ab40-e79f-465a-9b13-6f7df0575016"
#define APP_SECRET "cb3c30a8-7b00-4f95-a6d9-70671c9ac65c-5d7b7194-55e8-4eda-a9b5-04fd82266e3f"
#define TEMP_SENSOR_ID "66400d33fb874c7486d18587"
#define EVENT_WAIT_TIME 10000  // send event every 60 seconds
// //------------------------------------------------------------------------
//define pin dht11
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//------------------------------------------------------------------------
//CONFIG PORTAS
#define pino_sinal_analogico 35
#define pino_sinal_analogicoChuva 34
#define pinoboia 15
#define pinobuzzer 23
#define rele 18
//------------------------------------------------------------------------
//DECLARAÇÃO VARIAVEIS GLOBAIS
int valor_analogico = 0;
int valor_analogico_chuva = 0;
int valor_digital_cisterna = 0;
const int NUMERO_AMOSTRAS = 100;
const unsigned long tempIrrigacao = 60000;
int mediaSolo = 0;
int mediaChuva = 0;
int Porcento = 0;
String cisterna;
String chuva;
float h;
float t;
float lastTemperature;
float lastHumidity;

unsigned long lastEvent = (-EVENT_WAIT_TIME);  // last time event has been sent

//--------------------------------------------------------------------------


void setup() {
  Serial.begin(115200);
  setupWebServer();
  dht.begin();
  Config();
  setupSinricPro();
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();
  delay(100);


  pinMode(pino_sinal_analogico, INPUT);
  pinMode(pino_sinal_analogicoChuva, INPUT);
  pinMode(pinoboia, INPUT);
  pinMode(pinobuzzer, OUTPUT);
  pinMode(rele, OUTPUT);

  xTaskCreatePinnedToCore(
    Taskdht11,
    "Taskdht11",
    1024,
    NULL,
    1,
    &Task1,
    1);


  xTaskCreatePinnedToCore(
    TaskSolo,
    "TaskSolo",
    2048,
    NULL,
    1,
    &Task2,
    1);

  xTaskCreatePinnedToCore(
    Taskchuva,
    "Taskchuva",
    1024,
    NULL,
    1,
    &Task3,
    0);

  xTaskCreatePinnedToCore(
    Taskcisterna,
    "Taskcisterna",
    1024,
    NULL,
    1,
    &Task4,
    0);

  xTaskCreatePinnedToCore(
    OpenMeteoAPI,
    "OpenMeteoAPI",
    10000,
    NULL,
    1,
    &Task5,
    1);

  xTaskCreatePinnedToCore(
    Irrigation,
    "Irrigation",
    1024,
    NULL,
    1,
    &Task6,
    0);
}

//Task1code: le dht11
void Taskdht11(void* pvParameters) {

  for (;;) {
    h = dht.readHumidity();
    t = dht.readTemperature();
    if (isnan(t) || isnan(h)) {
      Serial.println("Failed to read from DHT");
    } else {
      // Serial.print("Umidade: ");
      // Serial.print(h);
      // Serial.print(" %t");
      // Serial.print("Temperatura: ");
      // Serial.print(t);
      // Serial.println(" *C");
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

//Task2code: le o sensor de umidade do solo
void TaskSolo(void* pvParameters) {
  for (;;) {
    int somatoria = 0;
    //Realiza a leitura do sensor de acordo com a variavel "NUMERO_AMOSTRAS"
    for (int i = 1; i <= NUMERO_AMOSTRAS; i++) {
      valor_analogico = analogRead(pino_sinal_analogico);  //Le o sensor
      somatoria = somatoria + valor_analogico;             //Soma a variavel "somatoria" a leitura atual do sensor com seu valor anterior
      float tensao = valor_analogico * (3.0 / 2048);       //Converte a leitura para tensao para referencia
    }
    //Calcula a media das leituras utilizando a variavel "somatoria" e a variavel "NUMERO_AMOSTRAS"
    mediaSolo = somatoria / NUMERO_AMOSTRAS;
    Porcento = map(valor_analogico, 4095, 1300, 0, 100);
    // Serial.println("");
    // Serial.print("Media solo: ");
    // Serial.println(mediaSolo);
    // Serial.print("Percentual Solo ");
    // Serial.println(Porcento);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

//Task2code: le o sensor de umidade do solo
void Taskchuva(void* pvParameters) {
  for (;;) {
    int somatoria = 0;
    //Realiza a leitura do sensor de acordo com a variavel "NUMERO_AMOSTRAS"
    for (int i = 1; i <= NUMERO_AMOSTRAS; i++) {
      valor_analogico_chuva = analogRead(pino_sinal_analogicoChuva);  //Le o sensor
      somatoria = somatoria + valor_analogico_chuva;                  //Soma a variavel "somatoria" a leitura atual do sensor com seu valor anterior
      float tensao = valor_analogico_chuva * (3.0 / 2048);            //Converte a leitura para tensao para referencia
    }
    mediaChuva = somatoria / NUMERO_AMOSTRAS;
    // Serial.print("Media chuva: ");
    // Serial.println(mediaChuva);

    if (mediaChuva >= 0 && mediaChuva < 200) {
      chuva = "Sem chuva";
    }
    if (mediaChuva > 200 && mediaChuva < 1000) {
      chuva = "Chuvisco";
    }
    if (mediaChuva > 1000 && mediaChuva < 1400) {
      chuva = " Chuva Moderada";
    }
    if (mediaChuva > 1400) {
      chuva = "Chuva Forte";
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

//Task2code: le o sensor de luz
void Taskcisterna(void* pvParameters) {
  for (;;) {
    valor_digital_cisterna = digitalRead(pinoboia);  //Le o sensor

    // Serial.print("Estado Sensor: ");
    // Serial.println(valor_digital_cisterna);

    if (valor_digital_cisterna == 1) {
      cisterna = "Sem Agua";
      digitalWrite(pinobuzzer, HIGH);
      vTaskDelay(50);
      digitalWrite(pinobuzzer, LOW);
    } else {
      cisterna = "Acima de 30%";
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void OpenMeteoAPI(void* pvParameters) {
  for (;;) {
    printLocalTime();
    // Serial.print("buffer: ");
    Serial.println(buffer);
    Serial.print("Horario api ");
    Serial.println(horarioAPI.c_str());
    if (strcmp(buffer, horarioAPI.c_str()) == 0) {
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      openMeteo();
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Irrigation(void* pvParameters) {
  for (;;) {
    digitalWrite(rele, LOW);
    Serial.print("solo config: ");
    Serial.println(percentualSolo);
    Serial.print("Media solo real: ");
    Serial.println(mediaSolo);
    Serial.print("Percentual Solo real: ");
    Serial.println(Porcento);
    Serial.print("chuva config: ");
    Serial.println(mediaLeituraSolo);
    Serial.print("Media chuva real: ");
    Serial.println(mediaChuva);
    Serial.print("openmeteo parametro: ");
    Serial.println(currentWeatherCode);
    Serial.print("openmeteo confog: ");
    Serial.println(currentWeatherCode);
    Serial.print("openmeteo api atual: ");
    Serial.println(current_weather_code); 
    Serial.print("Horario 1 ");
    Serial.println(horarioIrrigacao.c_str());
    Serial.print("Horario 2 ");
    Serial.println(horarioIrrigacaoTarde.c_str());
    if ((strcmp(buffer, horarioIrrigacao.c_str()) == 0 || strcmp(buffer, horarioIrrigacaoTarde.c_str()) == 0) && valor_digital_cisterna == 0) {
      bool conditionsIrrig = true;
      while (conditionsIrrig) {  // Continue enquanto as condições forem atendidas
        unsigned long start_time = millis();

        // Loop for the duration of tempIrrigacao
        for (unsigned long current_time = millis(); current_time - start_time < tempIrrigacao; current_time = millis()) {
          if (valor_digital_cisterna == 0 && Porcento < percentualSolo && mediaChuva < mediaLeituraSolo && current_weather_code <= currentWeatherCode) {
            digitalWrite(rele, HIGH);
            Serial.print("irrigando ");
          } else {
            digitalWrite(rele, LOW);
            conditionsIrrig = false;  // Marque que as condições não foram atendidas
            break;                    // Saia do loop for
          }
        }

        if (conditionsIrrig) {
          start_time = millis();  // Reiniciar o tempo
          // Verificação adicional usando for para a duração de tempIrrigacao
          for (unsigned long current_time = millis(); current_time - start_time < tempIrrigacao; current_time = millis()) {
            if (valor_digital_cisterna == 0 && t > 28 && h < 60 && mediaSolo < 40) {
              digitalWrite(rele, HIGH);
            } else {
              digitalWrite(rele, LOW);
              conditionsIrrig = false;  // Marque que as condições não foram atendidas
              break;                    // Saia do loop for
            }
          }
        }

        if (!conditionsIrrig) {
          break;  // Se as condições não foram atendidas, saia do loop infinito
        }
      }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Wait for 1 second before checking again
  }
}


//-----------------------------------------------------------
// setup function for SinricPro
void setupSinricPro() {
  // add device to SinricPro
  SinricProTemperaturesensor& mySensor = SinricPro[TEMP_SENSOR_ID];
  // setup SinricPro
  SinricPro.onConnected([]() {
    Serial.printf("Connected to SinricPro\r\n");
  });
  SinricPro.onDisconnected([]() {
    Serial.printf("Disconnected from SinricPro\r\n");
  });
  //SinricPro.restoreDeviceStates(true); // Uncomment to restore the last known state from the server.
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void handleTemperaturesensor() {
  unsigned long actualMillis = millis();
  if (actualMillis - lastEvent < EVENT_WAIT_TIME) return;  //only check every EVENT_WAIT_TIME milliseconds

  if (t == lastTemperature || h == lastHumidity) return;             // if no values changed do nothing...
  SinricProTemperaturesensor& mySensor = SinricPro[TEMP_SENSOR_ID];  // get temperaturesensor device
  bool success = mySensor.sendTemperatureEvent(t, h);                // send event
  if (success) {                                                     // if event was sent successfuly, print temperature and humidity to serial
    Serial.printf("Temperature: %2.1f Celsius\tHumidity: %2.1f%%\r\n", t, h);
  } else {  // if sending event failed, print error message
    Serial.printf("Something went wrong...could not send Event to server!\r\n");
  }

  lastTemperature = t;       // save actual temperature for next compare
  lastHumidity = h;          // save actual humidity for next compare
  lastEvent = actualMillis;  // save actual time for next compare
}

void loop() {
  Display();
  handleWebServer();
  delay(2);
  SinricPro.handle();
  handleTemperaturesensor();
}
//--------------------------------------------------
//mostra info no display
//--------------------------------------------------
void Display(void) {
  display.clearDisplay();

  display.setTextSize(2);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);
  display.cp437(true);  // Use full 256 char 'Code Page 437' font fixar
  display.println(F("Umidade"));
  display.print(F("Solo:"));
  display.print((Porcento));
  display.println(F("%"));
  display.print(F("Ar:"));
  display.print(h);
  display.println(F("%"));
  display.print(F("Temp:"));
  display.print(t);
  display.println(F("*C"));
  display.display();
  delay(2000);
  // Limpar a tela antes de imprimir o valor 3.141592
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(chuva);
  display.println(F("Cisterna:"));
  display.print(cisterna);

  display.display();
  delay(2000);
}
