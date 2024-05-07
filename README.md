# PBL01---Redes---Internet-das-Coisas
Problema 1 da matéria TEC502 - Concorrência e Conectividade

### Sumário 
+ [Como Executar](#como-executar-a-solução)
+ [Introdução](#introdução)
+ [Produto Desenvolvido](#produto-desenvolvido)
+ [Arquitetura da Solução](#arquitetura-da-solução)
+ [Protocolo de Comunicação](#protocolo-de-comunicação)
+ [API Restful](#api-restful)
+ [Conexões e Dados](#conexoes-e-dados)
+ [Gerenciamento do Dispositivo](#gerenciamento-do-dispositivo)
+ [Desempenho e Confiabilidade](#desempenho-e-confiabilidade)

## Como Executar:

A solução para o problema, assim como requerida, está dividida em três sistemas, os quais estão disponilibizados no formate de imagens Docker. Assim, para que seja feito o uso da solução é necessário possuir o Docker instalado na máquina e executar os seguintes comandos:

### Servidor(Broker)
Execute o seguinte comando para obter a imagem do servidor:
```bash
docker pull yxngnd/server:latest
```
Execute a imagem:
```bash
docker run -it --network=host yxngnd/server
```

### Dispositivo
Execute o seguinte comando para obter a imagem do server:
```bash
docker pull yxngnd/device:latest
```
Execute a imagem e substitua o campo *server_ip* pelo ip do server:
```bash
docker run -it --network=host -e HOST={server_ip} yxngnd/device
```

### Aplicação
Execute o seguinte comando para obter a imagem do server:
```bash
docker pull yxngnd/client:latest
```
Execute a imagem e substitua o campo *server_ip* pelo ip do server:
```bash
docker run -it --network=host -e HOST={server_ip} yxngnd/client
```

## Introdução

A evolução tenoclogica tem se tornado cada vez mais evidente, permitindo que um maior número de pessoas esteja conectada a rede nas mais diversas partes do planeta, com isso, cresceu também a busca por tornar mais conectados os objetos e dispositivos do dia a dia. A Internet das Coisas (IoT) foi concebida por Kevin Ashton, um pesquisador britânico, como um sistema de sensores ubíquos e um padrão global para RFID. Isso permitiu a identificação de objetos e a conexão de informações do mundo físico à Internet. Tendo isso em mente, foi requisitado o desenvolvimento de um produto que agisse como um *middleware* entre os dispositivos IoT e uma aplicação.

Esse *middleware* solicitado deveria ser um *broker* que permitiisse a troca de mensagens entre os dispositivos e a aplicação, através dele a aplicação deveria mandar comandos para interagir com suas funcionalidades, e também, deveria gerenciar os dados enviados por um ou mais dispositivos conectados para a aplicação.

A solução consiste em três partes: um dispositivo simulado, possuindo uma interface que permita alterar seus dados, uma aplicação que consuma uma API Restful desenvolvida e um *broker* que faz a mediação dos dois primeiros.

O sistema foi desenvolvido por completo utilizando a linguagem de programação C++, fazendo uso da biblioteca Beast do conjunto de bibliotecas Boost para desenvolver a API e fazer a comunicação entre a aplicação e o *broker*.

## Produto Desenvolvido

A solução é dividida em três partes: aplicação, broker e dispositivo, ilustrados na Figura 1.
![DiagramaGeral](IMG/produto.png)
- **Figura 1:** *Diagrama da solução do problema.*

### Aplicação
A aplicação é uma interface acessada por meio da linha de comando (CLI), usada para gerenciar, tanto receber e exibir os dados dos dispositivos, quanto enviar comandos de alteração dos atributos do dispositivo. A aplicação faz requisições HTTP para a API do servidor.

### Server
O servidor é onde está contido a parte principal do produto, se comunica com a aplicação lidando com as requisições feitas e com o dispositivo através de comunicação TCP, para enviar comando ao dispositivo, e UDP para receber os dados do dispositivo em formato JSON e armazená-los num buffer para que os dados de todos os dispositivos conectados ao servidor sejam enviados à aplicação quando ela faz uma requisição.

### Dispositivo
O dispositivo funciona como um simulador de uma lâmpada inteligente, possuindo os atributos de ligado/desligado, intensidade da luz e um número de identificação, que é único e não pode ser alterado. O dispositivo se comunica com o broker, recebendo comandos através de comunicação TCP, tratando os comandos recebidos e alterando seus atributos, além disso, está a todo momento enviando seus dados ao broker via comunicação UDP.

## Arquitetura da Solução


## Protocolo de Comunicação


## API Restful


## Conexões e Dados


## Gerenciamento do Dispositivo


## Desempenho e Confiabilidade

