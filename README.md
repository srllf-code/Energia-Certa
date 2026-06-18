# Energia-Certa

Disjuntor Eletrônico Inteligente
Este projeto consiste em um sistema de Disjuntor Eletrônico Inteligente controlado por Arduino com monitoramento e configuração em tempo real via script Python.
O hardware monitora a corrente elétrica de uma carga, calcula a potência consumida e desarma um relé caso a corrente ultrapasse um limite de segurança pré-configurado. O script Python atua como uma interface de controle, permitindo alterar dinamicamente o limite de corrente e visualizar os dados coletados.

# Funcionalidades
Monitoramento em Tempo Real: Medição de corrente (A) e cálculo de potência (kW).
Proteção contra Sobrecarga: Desarme automático do relé caso o limite de corrente seja violado por 4 leituras consecutivas (filtro contra picos rápidos).
Interface Física: Display LCD I2C para exibição de status e LEDs indicadores (Verde: Normal, Amarelo: Inicializando, Vermelho Piscando: Disjuntor Desarmado).
Integração Serial (Python): Menu interativo para alterar o limite de corrente, monitorar os dados e simular quedas de energia por sobrecarga.

# Componentes (Arduino)
Sensor de Corrente ACS712

Módulo Relé

Leds (Verde, amarelo, vermelho)

# Requisitos do Sistema

No Arduino
Certifique-se de ter as seguintes bibliotecas instaladas na sua Arduino IDE:
Wire.h (Nativa)
LiquidCrystal_I2C.h

# No Python

O script requer a biblioteca pyserial. Instale-a executando:
pip install pyserial

# Como Executar o Projeto

Montagem do Hardware: Conecte os componentes conforme a tabela de pinagem.
Upload do Arduino: Carregue o código fornecido para a sua placa Arduino.
Configuração do Python: Abra o arquivo Python e altere a variável PORTA para a porta COM correta do seu Arduino:

========================================
 # Monitor Arduino
========================================

  Simular Sobrecarga, Definir Sobrecarga, Monitorar Dados do Arduino, Sair.
  
  digitando: 1, 2, 3 e 0 respectivamente
========================================
