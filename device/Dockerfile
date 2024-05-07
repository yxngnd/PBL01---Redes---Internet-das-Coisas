# Use uma imagem base mais leve
FROM alpine:latest as builder

# Atualize os pacotes e instale as dependências
RUN apk update && \
    apk add --no-cache g++ libstdc++ boost-dev
    
# Instale a biblioteca json3
RUN apk add --no-cache nlohmann-json

# Configure o ambiente de trabalho
WORKDIR /.

# Copie o código fonte para o diretório de trabalho no container
COPY . .

RUN g++ -o device deviceInterface.cpp device.cpp -pthread -lboost_system && \
    strip --strip-all device
    # Remova símbolos de depuração e informações adicionais

# Agora, crie uma imagem final, copiando apenas os artefatos necessários
FROM alpine:latest

# Instale as bibliotecas necessárias
RUN apk update && \
    apk add --no-cache libstdc++ boost-system

# Copie o binário compilado do estágio anterior
COPY --from=builder /device /device

# Comando padrão a ser executado quando o container for iniciado
CMD ["./device"]
