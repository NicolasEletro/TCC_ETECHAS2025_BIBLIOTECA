//configurações iniciais
  #include <WiFi.h>           //biblioteca do wifi esp32
  #include <WebServer.h>      //cria servidor WEB HTTP
  #include <DNSServer.h>      //redireciona qualquer acesso DNS no navegador para o esp

//Configuração AP
  const char* AP_SSID = "BIBLIOTECAHASTCC";   //SSID
  const char* AP_PASS = "0123456789";         //PSW
  IPAddress apIP(192,168,4,1);                //IP estático
  IPAddress netMsk(255,255,255,0);            //MAC

// Objetos principais
  DNSServer dnsServer;    //cria o servidor WEB
  WebServer server(80);   //canal de comunicação http (80)
  const byte DNS_PORT = 53;   //porta de redirecionamento ao servidor do esp

//variáveis globais:
  int quantidadeDeAcessos = 0;
// Página HTML (histórico de acesso)
  String sendPage() {
    String quantAccess = String(quantidadeDeAcessos);
    
    String page =
      "<!DOCTYPE html>"
      "<html>"
      "<head>"
        "<meta charset=\"UTF-8\">"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
        "<meta name=\"author\" content=\"Nicolas Alves Araujo\">"
        "<title>Quantidade de acesso à biblioteca</title>"
      "</head>"
      "<body>"
        "<h2>Quantidade de acessos:</h2>"
        "<p>No total " + quantAccess + " pessoas entraram aqui.</p>"
      "</body>"
      "</html>";
    
    return page;
}



void startCaptivePortal() {
    WiFi.mode(WIFI_MODE_AP);                //inicia o modo Access Point
    WiFi.softAPConfig(apIP, apIP, netMsk);  //configuração do AP
    WiFi.softAP(AP_SSID, AP_PASS);          //lança a rede
    
    //debbuger 
      Serial.println("=== Captive Portal Iniciado ===");
      Serial.print("SSID: "); Serial.println(AP_SSID);
      Serial.print("Senha: "); Serial.println(AP_PASS);
      Serial.print("IP do AP: "); Serial.println(WiFi.softAPIP());

      dnsServer.start(DNS_PORT, "*", apIP);   //redirecionar DNS para o esp

    // Página principal
      server.on("/", []() { //quando a raíz (http://espIP/) for acessado, ativar uma função anônima/lambda
        server.send(200, "text/html", sendPage());    //enviar a página HTML com resposta 200
      });

    //qualquer outro endereço → redireciona pra página principal
    server.onNotFound([]() {
        server.sendHeader("Location", String("http://") + apIP.toString() + "/", true);
        server.send(302, "text/plain", "");
    });

  server.begin();
  Serial.println("Servidor Web iniciado.");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  startCaptivePortal();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}

não criou a rede
