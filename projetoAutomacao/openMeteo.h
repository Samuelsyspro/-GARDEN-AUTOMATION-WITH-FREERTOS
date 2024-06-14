#include <ArduinoJson.h>
#include <WEATHER_CODES.h>

//URL Endpoint for the API
String URL = "https://api.open-meteo.com/v1/forecast?";
String Parameters = "&current=temperature_2m,relative_humidity_2m,precipitation,rain,weather_code,surface_pressure&hourly=temperature_2m,relative_humidity_2m,precipitation_probability,precipitation,rain,weather_code&daily=weather_code,temperature_2m_max,temperature_2m_min,precipitation_hours,precipitation_probability_max&forecast_days=3";


// Replace with your location Credentials
String lat = "";
String lon = "";
String descricao = "";

int current_weather_code = 0;

void openMeteo() {
  // wait for WiFi connection
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    //Set HTTP Request Final URL with Location and API key information
    http.begin(URL + "latitude=" + lat + "&longitude=" + lon + Parameters);

    // start connection and send HTTP Request
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {

      //Read Data as a JSON string
      String JSON_Data = http.getString();
      Serial.println(JSON_Data);

      //Retrieve some information about the weather from the JSON format
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, JSON_Data);
      JsonObject obj = doc.as<JsonObject>();
      //busca descriçao dos codigos
      StaticJsonDocument<512> weatherCodes;
      DeserializationError error = deserializeJson(weatherCodes, weatherCodesJSON);
      //Display the Current Weather Info

      JsonObject current = doc["current"];
      const char* current_time = current["time"];                          // "2024-05-06T02:00"
      int current_interval = current["interval"];                          // 900
      float current_temperature_2m = current["temperature_2m"];            // 19.9
      int current_relative_humidity_2m = current["relative_humidity_2m"];  // 77
      int current_precipitation = current["precipitation"];                // 0
      current_weather_code = current["weather_code"];                      // 0


      JsonObject daily = doc["daily"];

      const char* daily_time_0 = daily["time"][0];  // "2024-05-06"

      int daily_weather_code_0 = daily["weather_code"][0];  // 1

      float daily_temperature_2m_max_0 = daily["temperature_2m_max"][0];  // 27.6

      float daily_temperature_2m_min_0 = daily["temperature_2m_min"][0];  // 18.1

      //verifica se existe o codigo  no json
      if (weatherCodes.containsKey(String(current_weather_code))) {
        Serial.println(current_weather_code);
        // Se existir, retorne a descrição correspondente
        descricao = weatherCodes[String(current_weather_code)].as<String>();
      } else {
        // Se não existir, retorne uma mensagem de erro
        descricao = "Código de tempo não encontrado";
      }

      Serial.print("current_precipitation: ");
      Serial.println(current_precipitation);

      Serial.print("current_weather_code: ");
      Serial.print(current_weather_code);
      Serial.println(+" " + descricao);

      Serial.print("daily_weather_code_0: ");
      Serial.println(daily_weather_code_0);

    } else {
      Serial.println("Error!");
    }

    http.end();
  }
}
