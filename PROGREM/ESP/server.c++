#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

//configurações do AP
  const char* AP_SSID = "biblioteca JCL TCC";
  const char* AP_PASS = "EuSouEletronica";

//Serviror de redirecionamento DNS
  const byte DNS_PORT = 53;
  DNSServer dnsServer;

//servidor WEB
  ESP8266WebServer server(80);

//contadores
  volatile unsigned long contD = 0; 
  volatile unsigned long contE = 0;
  volatile unsigned long contS = 0;
  volatile unsigned long contB = 0;

//mensagens
  String ultimaAcao = "Nenhuma ação registrada ainda."; 
  //estado inicial


String paginaInicial() {
  String html =
  "<!DOCTYPE html>"
  "<html lang = 'pt-br'>"
    "<head>"
      "<meta charset='UTF-8'>"
      "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
      "<title>Estatística - TCC biblioteca 2025</title>"
      "<meta name='author' content='Nicolas Alves Araujo'>"
     "</head>"
    "<body>"
      "<header>"
        "<h1>AUTOMAÇÃO DA BIBLIOTECA HAS - TCC2025</h1>"
        "<p>Página inicial do projeto de automação - Estatística e ações recentes<p><hr><br>"
      "</header>"
      "<main>"
        "<h2>Contadores</h2>"
        "<ul>"
        "<li>Quantidade de livros devolvidos: <b id='D'>0</b></li>"
        "<li>Quantidade de livros emprestados: <b id='E'>0</b></li>"
        "<li>Quantidade de livros que saíram: <b id='S'>0</b></li>"
        "<li>Quantidade de livros barrados: <b id='B'>0</b></li>"
        "</ul>"
        "<hr>"
        "<h2>Ações recentes</h2>"
        "<p><b>Última ação:</b> <span id='acao'>Esperando resposta do servidor</span></p>"
      "</main>"
      "<footer>"
        "<br><hr>"
        "<p>Escrito por: <i>ARAUJO, Nicolas Alves. 2025</i><br>" 
        "Última edição em: <time datetime='2025-11-26'>26/11/2025</time>.</p>"
        "<p>Para mais informações, entre em contato com o desenvolvedor:<br>"
        "<address>"
          "*E-mail: <a href='https://mail.google.com/mail/?view=cm&fs=1&to=nicolasalvesdearaujo007@gmail.com&su=Sobre%20o%20sistema&body=Digite%20aqui%20sua%20duvida' target='_blank'> nicolasalvesdearaujo007@gmail.com </a>"
          "*Telegram: <a href='https://t.me/oTalDoRusso' target='_blank'>@oTalDoRusso</a></il>"
        "</address>" 
      "</footer>"
      "<script>"
        "function atualizar(){"
          "fetch('/valores').then(r=>r.json()).then(v=>{"
            "document.getElementById('D').innerText=v.D;"
            "document.getElementById('E').innerText=v.E;"
            "document.getElementById('S').innerText=v.S;"
            "document.getElementById('B').innerText=v.B;"
            "document.getElementById('acao').innerText=v.acao;"
          "});"
        "}"
        "setInterval(atualizar,1000);"
        "atualizar();"
      "</script>"
    "</body>"
  "</html>";

  return html;
}


void serialInput() {
  while(Serial.available()){
  delay(50); //esperar buffer encher
  //receber do atmega o tipo de função 
  char funcao = Serial.read(); 
    
  //incremento
    if (funcao == 'D') contD++;
    else if (funcao == 'E') contE++;
    else if (funcao == 'S') contS++;
    else if (funcao == 'B') contB++;
    
  //criação da string para servidor
    if (funcao == 'E') {
      String rm = Serial.readStringUntil(',');
      String codigo = Serial.readStringUntil('\n');
      ultimaAcao = "O livro " + codigo + " foi emprestado para " + rm;
    }

    else if (funcao == 'D') {
      String rm = Serial.readStringUntil(',');
      String codigo = Serial.readStringUntil('\n');
      ultimaAcao = rm + " devolveu o livro " + codigo;
    }

    else if (funcao == 'S') {
      String valor = Serial.readStringUntil('\n');
      ultimaAcao = "O livro " + valor + " saiu da biblioteca";
    }

    else if (funcao == 'A') {
      String rm = Serial.readStringUntil('\n');
      ultimaAcao = rm + " acessou a biblioteca";
    }

    else if (funcao == 'C') {
      String rm = Serial.readStringUntil(',');
      String codigo = Serial.readStringUntil('\n');
      ultimaAcao = rm + " consultou o livro " + codigo;
    }
  }
}


void setup() {
  Serial.begin(115200);
  
  //configuração de wifi
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);

  //inicia servidor
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  //resposta para o cliente
  server.on("/", [](){
    server.send(200, "text/html", paginaInicial());
  });
  
  //criação do JSON dos valores
  server.on("/valores", [](){
    String json = "{";
    json += "\"D\":" + String(contD) + ",";
    json += "\"E\":" + String(contE) + ",";
    json += "\"S\":" + String(contS) + ",";
    json += "\"B\":" + String(contB) + ",";
    json += "\"acao\":\"" + ultimaAcao + "\"";
    json += "}";
    server.send(200, "application/json", json);
  });

  // Captive portal
  server.onNotFound([](){
    server.sendHeader("Location","/",true);
    server.send(302,"text/plain","");
  });

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  serialInput();
}
