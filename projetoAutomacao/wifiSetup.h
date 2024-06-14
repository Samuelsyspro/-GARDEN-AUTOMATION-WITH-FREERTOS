// wifiSetup.h

#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <WiFi.h>
#include <NetworkClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid = "TP-Link_7DA6";
const char* password = "03720158";

WebServer server(80);
// Parametros iniciais irrigação
String horarioAPI = "08:00:00";
String horarioIrrigacao = "08:00:00";
String horarioIrrigacaoTarde = "08:00:00";
int percentualSolo = 30;
int mediaUmidadeSolo = 3200;
int mediaLeituraSolo = 200;
int currentWeatherCode = 3;

const char* index_html = R"rawliteral(
  <!DOCTYPE HTML>
  <html>
    <head>
      <title>Irrigação</title>
      <meta charset="UTF-8">
      <style>
        body {
          font-family: Arial, sans-serif;
          text-align: center;
          margin-top: 50px;
        }
        h1 {
          color: #333;
        }
        form {
          display: inline-block;
          text-align: left;
          background: #f4f4f4;
          padding: 20px;
          border-radius: 10px;
          box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        label {
          display: block;
          margin-top: 10px;
          font-weight: bold;
        }
        input[type="time"], input[type="text"] {
          display: block;
          margin-top: 5px;
          padding: 5px;
          font-size: 16px;
        }
        input[type="submit"] {
          display: block;
          margin-top: 20px;
          padding: 10px 20px;
          background: #4CAF50;
          color: white;
          border: none;
          border-radius: 5px;
          cursor: pointer;
          font-size: 16px;
        }
        input[type="submit"]:hover {
          background: #45a049;
        }
        .error {
          color: red;
          margin-top: 5px;
        }
      </style>
    </head>
    <body>
      <h1>Configurar Irrigação</h1>
      <form action="/irrigacao" method="POST">
        <label for="horarioAPI">Horário API OPEN METEO:</label>
        <input type="time" id="horarioAPI" name="horarioAPI" step="1">
        <div id="errorAPI" class="error"></div>
        <input type="submit" value="Atualizar Horário API">
      </form>
      <form action="/irrigacao" method="POST">
        <label for="horarioIrrigacao">Horário Irrigação Manhã:</label>
        <input type="time" id="horarioIrrigacao" name="horarioIrrigacao" step="1">
        <div id="errorIrrigacao" class="error"></div>
        <label for="horarioIrrigacaoTarde">Horário Irrigação Tarde:</label>
        <input type="time" id="horarioIrrigacaoTarde" name="horarioIrrigacaoTarde" step="1">
        <div id="errorIrrigacaoTarde" class="error"></div>
        <input type="submit" value="Atualizar Horário Irrigação">
      </form>
      <form action="/irrigacao" method="POST">
        <label for="percentualSolo">Percentual Umidade Solo</label>
        <input type="text" name="percentualSolo" id="percentualSolo" oninput="this.value = this.value.replace(/[^0-9]/g, '') + '%';" maxlength="3">
        <div id="errorPercentualSolo" class="error"></div>
  
        <label for="mediaUmidadeSolo">Média Umidade Solo</label>
        <input type="text" name="mediaUmidadeSolo" id="mediaUmidadeSolo" oninput="this.value = this.value.replace(/[^0-9]/g, '');" maxlength="4">
        <div id="errorMediaUmidadeSolo" class="error"></div>
  
        <label for="mediaLeituraSolo">Média Leitura Chuva</label>
        <input type="text" name="mediaLeituraSolo" id="mediaLeituraSolo" oninput="this.value = this.value.replace(/[^0-9]/g, '');" maxlength="4">
        <div id="errorMediaLeituraSolo" class="error"></div>
  
        <label for="currentWeatherCode">Codigo Previsão Tempo</label>
        <input type="text" name="currentWeatherCode" id="currentWeatherCode" oninput="this.value = this.value.replace(/[^0-9]/g, '');" maxlength="3">
        <div id="errorCurrentWeatherCode" class="error"></div>
  
        <input type="submit" value="Parâmetros de Controle da Irrigação">
      </form>
      <script>
        document.querySelectorAll("form").forEach(function(form) {
          form.addEventListener("submit", function(event) {
            var inputs = this.querySelectorAll("input[type='time']");
            var errors = this.querySelectorAll(".error");
            var valid = true;
            
            // Validação de campos de tempo
            inputs.forEach(function(input, index) {
              if (input.value === "") {
                event.preventDefault();
                errors[index].innerHTML = "Por favor, informe um horário válido.";
                valid = false;
              } else {
                errors[index].innerHTML = "";
              }
            });
            
            // Validação de campos de texto
            var percentualSolo = document.getElementById('percentualSolo');
            var mediaUmidadeSolo = document.getElementById('mediaUmidadeSolo');
            var mediaLeituraSolo = document.getElementById('mediaLeituraSolo');
            var currentWeatherCode = document.getElementById('currentWeatherCode');
  
            if (percentualSolo.value.replace('%', '') === "" || percentualSolo.value.replace('%', '') < 1 || percentualSolo.value.replace('%', '') > 100) {
              event.preventDefault();
              document.getElementById('errorPercentualSolo').innerHTML = "Por favor, informe um valor entre 1 e 100% para Percentual Umidade Solo.";
              valid = false;
            } else {
              document.getElementById('errorPercentualSolo').innerHTML = "";
            }
  
            if (mediaUmidadeSolo.value === "" || mediaUmidadeSolo.value < 1 || mediaUmidadeSolo.value > 4500) {
              event.preventDefault();
              document.getElementById('errorMediaUmidadeSolo').innerHTML = "Por favor, informe um valor entre 1 e 4500 para Média Umidade Solo.";
              valid = false;
            } else {
              document.getElementById('errorMediaUmidadeSolo').innerHTML = "";
            }
  
            if (mediaLeituraSolo.value === "" || mediaLeituraSolo.value < 1 || mediaLeituraSolo.value > 4500) {
              event.preventDefault();
              document.getElementById('errorMediaLeituraSolo').innerHTML = "Por favor, informe um valor entre 1 e 4500 para Média Leitura Chuva.";
              valid = false;
            } else {
              document.getElementById('errorMediaLeituraSolo').innerHTML = "";
            }
  
            if (currentWeatherCode.value === "" || currentWeatherCode.value < 1 || currentWeatherCode.value > 100) {
              event.preventDefault();
              document.getElementById('errorCurrentWeatherCode').innerHTML = "Por favor, informe um valor entre 1 e 100 para Código Previsão Tempo.";
              valid = false;
            } else {
              document.getElementById('errorCurrentWeatherCode').innerHTML = "";
            }
  
            if (!valid) event.preventDefault();
          });
        });
      </script>
    </body>
  </html>
  
)rawliteral";

void setupWebServer() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/plain", "hello, acesse /irrigacao");
  });

  server.on("/irrigacao", HTTP_GET, []() {
    server.send(200, "text/html", index_html);
  });

  server.on("/irrigacao", HTTP_POST, []() {
    if (server.hasArg("horarioAPI")) {
      horarioAPI = server.arg("horarioAPI");
      String response_html = "<!DOCTYPE HTML><html><head><title>Irrigação</title></head><meta charset=\"UTF-8\"><body><script>alert('Horário API OPEN METEO atualizado para: " + horarioAPI + "'); window.location.href='/irrigacao';</script></body></html>";
      server.send(200, "text/html", response_html);
      Serial.println("Horário API OPEN METEO recebido: " + horarioAPI);
    } else if (server.hasArg("horarioIrrigacao") && server.hasArg("horarioIrrigacaoTarde")) {
      horarioIrrigacao = server.arg("horarioIrrigacao");
      horarioIrrigacaoTarde = server.arg("horarioIrrigacaoTarde");
      String response_html = "<!DOCTYPE HTML><html><head><title>Irrigação</title></head><meta charset=\"UTF-8\"><body><script>alert('Horários de Irrigação atualizados para: " + horarioIrrigacao + " e " + horarioIrrigacaoTarde + "'); window.location.href='/irrigacao';</script></body></html>";
      server.send(200, "text/html", response_html);
      Serial.println("Horário Irrigação recebido: " + horarioIrrigacao);
      Serial.println("Horário Irrigação Tarde recebido: " + horarioIrrigacaoTarde);
    } else if (server.hasArg("percentualSolo") && server.hasArg("mediaUmidadeSolo") && server.hasArg("mediaLeituraSolo") && server.hasArg("currentWeatherCode")) {
      percentualSolo = server.arg("percentualSolo").toInt();
      mediaUmidadeSolo = server.arg("mediaUmidadeSolo").toInt();
      mediaLeituraSolo = server.arg("mediaLeituraSolo").toInt();
      currentWeatherCode = server.arg("currentWeatherCode").toInt();

      bool valid = true;
      String errorMsg = "";

      if (percentualSolo < 1 || percentualSolo > 100) {
        valid = false;
        errorMsg += "Erro: Percentual Umidade Solo deve estar entre 1 e 100. ";
      }
      if (mediaUmidadeSolo < 1 || mediaUmidadeSolo > 4500) {
        valid = false;
        errorMsg += "Erro: Média Umidade Solo deve estar entre 1 e 4500. ";
      }
      if (mediaLeituraSolo < 1 || mediaLeituraSolo > 4500) {
        valid = false;
        errorMsg += "Erro: Média Leitura Chuva deve estar entre 1 e 4500. ";
      }
      if (currentWeatherCode < 1 || currentWeatherCode > 100) {
        valid = false;
        errorMsg += "Erro: Código Previsão Tempo deve estar entre 1 e 100. ";
      }

      if (valid) {
        String response_html = "<!DOCTYPE HTML><html><head><title>Irrigação</title></head><meta charset=\"UTF-8\"><body><script>alert('Parâmetros de controle da irrigação atualizados com sucesso!'); window.location.href='/irrigacao';</script></body></html>";
        server.send(200, "text/html", response_html);
        Serial.println("Percentual Umidade Solo recebido: " + String(percentualSolo));
        Serial.println("Média Umidade Solo recebida: " + String(mediaUmidadeSolo));
        Serial.println("Média Leitura Chuva recebida: " + String(mediaLeituraSolo));
        Serial.println("Código Previsão Tempo recebido: " + String(currentWeatherCode));
      } else {
        String response_html = "<!DOCTYPE HTML><html><head><title>Irrigação</title></head><meta charset=\"UTF-8\"><body><script>alert('" + errorMsg + "'); window.location.href='/irrigacao';</script></body></html>";
        server.send(400, "text/html", response_html);
      }
    } else {
      server.send(400, "text/plain", "Erro: Parâmetros não recebidos.");
    }
  });

  server.begin();
  Serial.println("HTTP server started");
}

void handleWebServer() {
  server.handleClient();
}

#endif
