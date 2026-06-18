"""
Monitor Arduino - Comunicação Serial Simples
Requisitos: pip install pyserial
"""

import serial
import time

# ─── Configuração da porta serial ──────────────────────────────────────────────
PORTA     = "COM3"   # Altere para a porta correta (Linux: "/dev/ttyUSB0")
BAUD_RATE = 9600
# ───────────────────────────────────────────────────────────────────────────────


def conectar():
    """Abre a conexão com o Arduino e aguarda a inicialização."""
    print(f"Conectando em {PORTA} a {BAUD_RATE} baud...")
    ser = serial.Serial(PORTA, BAUD_RATE, timeout=1)
    time.sleep(2)  # Aguarda o Arduino reiniciar após a conexão
    print("Conexão estabelecida!\n")
    return ser


def enviar_limite(ser, valor):
    """Envia um valor float de limite de corrente para o Arduino."""
    mensagem = f"{valor}\n"
    ser.write(mensagem.encode("utf-8"))


def simular_sobrecarga(ser):
    """Opção 1 – Envia um limite mínimo para forçar o desligamento do relé."""
    enviar_limite(ser, 0.1)
    print("Sobrecarga simulada com sucesso.\n")


def definir_limite(ser):
    """Opção 2 – Pede um valor ao usuário e o envia ao Arduino."""
    try:
        valor = float(input("Digite o novo limite de corrente (A): "))
        enviar_limite(ser, valor)
        print(f"Novo limite de {valor}A enviado ao Arduino.\n")
    except ValueError:
        print("Valor inválido. Digite um número, ex: 1.5\n")


def monitorar(ser):
    """Opção 3 – Lê e exibe continuamente os dados enviados pelo Arduino."""
    print("Monitorando... Pressione CTRL+C para parar.\n")
    try:
        while True:
            linha = ser.readline().decode("utf-8").strip()
            if not linha:
                continue  # Ignora linhas vazias (timeout)

            # Formato esperado: POTENCIA_KW,STATUS
            partes = linha.split(",")
            if len(partes) == 2:
                potencia = partes[0]
                status   = "DESARMADO" if partes[1] == "1" else "NORMAL"
                print(f"Potência: {float(potencia):.3f} kW | Status: {status}")
            else:
                # Exibe qualquer outra mensagem do Arduino sem processar
                print(f"[Arduino] {linha}")

    except KeyboardInterrupt:
        print("\nMonitoramento encerrado.\n")


def menu():
    """Exibe o menu principal e retorna a opção escolhida."""
    print("=" * 40)
    print("  Monitor Arduino")
    print("=" * 40)
    print("  1 - Simular Sobrecarga")
    print("  2 - Definir Novo Limite de Corrente")
    print("  3 - Monitorar Dados do Arduino")
    print("  0 - Sair")
    print("=" * 40)
    return input("Escolha uma opção: ").strip()


def main():
    ser = conectar()

    while True:
        opcao = menu()

        if opcao == "1":
            simular_sobrecarga(ser)
        elif opcao == "2":
            definir_limite(ser)
        elif opcao == "3":
            monitorar(ser)
        elif opcao == "0":
            print("Encerrando...")
            ser.close()  # Fecha a porta serial corretamente
            break
        else:
            print("Opção inválida. Tente novamente.\n")


if __name__ == "__main__":
    main()