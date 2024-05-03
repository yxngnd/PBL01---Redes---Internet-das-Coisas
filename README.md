# PBL01---Redes---Internet-das-Coisas
Problema 1 da matéria TEC502 - Concorrência e Conectividade

## Como executar:

Para executar a solução é necessário possuir um compilador C++ e a biblioteca [Boost](https://www.boost.org) instalada em seu computador. Além disso, é preciso ter o Docker em sua máquina para obter as imagens dos componentes da solução e utilizá-las.

### Servidor(Broker)
Execute o seguinte comando para obter a imagem do servidor:
```bash
docker pull yxngnd/server:latest
```
Execute a imagem:
```bash
docker run -it -p 8080:8080 yxngnd/server
```

### Dispositivo
Execute o seguinte comando para obter a imagem do server:
```bash
docker pull yxngnd/device:latest
```
Execute a imagem e substitua o campo *server_ip* pelo ip do server:
```bash
docker run -it -e server_ip={server_ip} yxngnd/device
```

### Aplicação
Entre na pasta *client* e execute o comando para compilar a aplicação:
```bash
g++ -o app clientApi.cpp
```
Agora execute o seguinte comando para executá-la:
```bash
./app
```

