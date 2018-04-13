## Sistema operacional

Ubuntu 16.04.1 (versão do kernel - 4.13.0)
<br/>
Manjaro deepin (versão do kernel - 4.14.31)

## Ambiente de desenvolvimento

gcc 7.3.1 e 5.4.0

## Compilação

No diretório do projeto, abra a pasta sockets e rode o script `make_all` com o comando
`$ bash make_all` ou `./make_all`.

## Execução

Execute o servidor com o comando `./bins/udp_server [ip] [porta]`. Para o cliente, rode
o comando `./bins/udp_client rtt [ip] [porta]` para o cálculo do RTT ou `./bins/udp_client rsv [url] [ip] [porta]` para resolução de nomes.

## Limitações

* A URL tem que possuir no máximo 2048 caracteres;
* Só suporta operações com o IPv4;

## Imagens

### Comportamento normal do cálculo do RTT

<img src="https://github.com/icaropires/fundamentals-computer-networks/blob/master/sockets/images/client_rtt.png" />

### Exemplo de timeout

<img src="https://github.com/icaropires/fundamentals-computer-networks/blob/master/sockets/images/client_rtt_error.png" />

### Comportamento normal da resolução de nomes

<img src="https://github.com/icaropires/fundamentals-computer-networks/blob/master/sockets/images/client_rsv.png" />

### Exemplo de falha na resolução

<img src="https://github.com/icaropires/fundamentals-computer-networks/blob/master/sockets/images/client_rsv_error.png" />

### Chamada incorreta do programa

<img src="https://github.com/icaropires/fundamentals-computer-networks/blob/master/sockets/images/input_error_treatment.png" />

### Servidor esperando requisições

<img src="https://github.com/icaropires/fundamentals-computer-networks/blob/master/sockets/images/server.png" />
