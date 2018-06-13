Alunos: Ícaro Pires (15/0129815) e João Robson (15/0154003)

# Configuração NAT e DHCP

## Sistema operacional utilizado
FreeBSD 11.1

## Aplicações utilizadas para configuração da LAN
* dhcpd
* natd
* editor de texto

## Implementação da rede LAN

### Preparação das interfaces
* identificação entre os nomes das interfaces de rede e os barramentos físicos;
* ativação das interfaces de rede;
    * `ifconfig <nome_da_interface> up`
* configuração dos IPs desejados para cada interface;
* atribuir IP para interface externa, pode ser via DHCP (se estiver disponível), nesse caso foi utilizaod DHCP síncrono para evitar problemas com a ordem dos eventos;
* atribuir IP e máscara da interface de interna: *172.16.0.1/16*;
* trecho correspondente:
```
ifconfig_re0="SYNCDHCP"
ifconfig_rl0="inet 172.16.0.1 netmask 255.255.0.0"
```

### Configuração do DHCP
* Instalar o DHCPD:
    * Baixar e instalar dhcpd a partir do pacote ports ou net:
    ``` bash
    # Execute como sudo
    cd /usr
    fetch ftp://ftp.freebsd.org/pub/FreeBSD/ports/ports/ports.tar.gz
    tar -xvzf ports.tar.gz
    cd ports/net/isc-dhcpd4.4/
    make install clean
    ```
* Configurar dhcpd:
    * Editar o arquivo `/etc/rc.conf`:
        * adicionar as seguintes linhas ao arquivo:
        ```
        dhcpd_enable="YES"
        dhcpd_ifaces="<interface_e>"
        ```
    * Editar o arquivo `/usr/local/etc/dhcpd.conf` com informações sobre a rede:
        * endereço do serviço de nomes: *192.168.133.1*
        * trecho correspondente:
        ```
        option domain-name "rede_realmente_interna";
        option domain-name-servers 192.168.133.1;
        ```
    * Editar o arquivo `/usr/local/etc/dhcpd.conf` com informações sobre subrede:
        * podemos utilizar um prefixo para redes privadas e uma máscara de 16 bits conforme o solicitado: *172.16.0.0/16*;
        * tem-se que definir o range de IPs para que atenda o limite da nossa rede mais externa, que é de 8 bits livres, 254 máquinas com IPs diferentes + endereço broadcast + endereço da rede: *de 172.16.0.2 até 172.16.0.254*;
        * para endereço do serviço de nomes utilizamos o informado: 192.168.133.1;
        * para endereço de broadcast definimos o último disponível na rede: *172.16.0.255*;
        * e como default gateway selecionamos o endereço da própria interface interna a subrede recém-criada: *172.16.0.1*;
        * trecho correspondente as informações:
        ```
        subnet 172.16.0.0 netmask 255.255.0.0 {
          range 172.16.0.2 172.16.0.254;
          option domain-name "subnet_rede_realmente_interna";
          option domain-name-servers 192.168.133.1;
          option broadcast-address 172.16.0.255;
          default-lease-time 600;
          max-lease-time 7200;
          option routers 172.16.0.1;
        }
        ```
    * Editar o arquivo `/usr/local/etc/dhcpd.conf` com informações para que máquinas conhecidas obtenham sempre o mesmo ip:
        * adicionar MAC da máquina conhecida: *84:7b:eb:fe:be:59*
        * adicionar IP que sempre será atribuído a máquina (colocar um IP que atenda o range estipulado): *172.16.0.42*
        * trecho correspondente:
        ```
        host joao {
          hardware ethernet 84:7b:eb:fe:be:59;
          fixed-address 172.16.0.42;
        }
        ```

### Configuração do NAT

* Editar arquivo `/boot/loader.conf`:
    * Adicionar componentes do kernel para serem lidoss durante boot:
    * trecho correspondente:
    ```
    ipfw_load="YES"
    ipdivert_load="YES"
    net.inet.ip.fw.default_to_accept="1"  # Para tornar o firewall mais permissivo durante configuração
    ```

* Editar arquivo `rc.conf`:
    * adicionar linha para indicar ativação do natd
    * adicionar linha para especificação da interface que estará conctada a internet
    * ligar firewall
    * abrir firewall
    * fazer dispositivo agir como gateway
    * trecho correspondente:
    ```
    gateway_enable="YES"

    natd_enable="YES"
    natd_interface="re0"

    firewall_enable="YES"
    firewall_type="OPEN"
    ```

### Arquivos de configuração completos
Estão na pasta deste arquivo os arquivo de configuração utilizados na configuração da LAN, os arquivos e seus respectivos caminhos são:
```
dhcpd.conf: /usr/local/etc/
hosts: /etc/hosts
loader.conf: /boot/
rc.conf: /etc/
```

## Validação da configuração da rede LAN

### Validação DHCP
O funcionamento do DHCP pode ser validado atráves do uso das ferramentas: ifconfig, netstat e o comando cat.

#### Contribuição do ifconfig
Com `ifconfig` pode-se validar que ao conectar o cabo ethernet a um dispositivo e a interface de rede externa foi atribuído um IP na faixa correta (incluindo máscara) a um dispositivo desconhecido e o IP especificado para um dispositivo conhecido.

#### Contribuição do netstat
Com `netstat -r` pode ser validada a tabela de roteamento do novo dispositivo conectado, ele possuía o default gateway correto.

#### Contribuição do cat
Com `cat /etc/resolv.conf` foi possível verificar que endereço de DNS também foi atribuído corretamente.

### Validação NAT
O funcionamento do NAT foi validado observando o comportamento dos pacotes através das interfaces de rede, além da conexão funcional a internet.

#### Contribuição do tcpdump
Com `tcpdump -i <nome_interface>` foi possível observar que para a interface interna, a que interage com a subrede, os pacotes estavam chegando de um IP na faixa de endereçamento atribuída pelo DHCP e com destino a um endereço externo. E observando a interface de rede externa era possível verificar que agora o datagrama estava com o IP da interface externa, ou seja, o IP foi trocado no caminho, validando parte do NAT.

#### Contribuição do ping
Pingando computadores que estavam na rede LDS foi possível validar que os pacotes que deixavam a subrede realmente conseguiam sair de um computador na subrede, atravessar o roteador e alcançar um computador em outra rede, além de fazer o caminho inverso, validando o NAT.

## Limitações conhecidas
* Para servir um serviço para a rede LDS é necessário um encaminhamento de portas;
* O IP fixo por dispositivo só funciona com cadastro prévio do dispositivo.

## Log
Foi gerado um arquivo de log durante os testes, neste log constam a passagem de pacotes na interface interna do computador roteador enquanto outro computador (joao) acessava alguns serviços web. O arquivo é o `tcpdump_log`.


# Configuração da VPN

## Configuração do servidor

### Sistema operacional
FreeBSD 11.1

### Aplicações utilizadas
* openvpn

### Configuração do openvpn

#### Configurando estrutura dos diretórios

* Criando a pasta
```
# mkdir /usr/local/etc/openvpn
```

* Copiando arquivos de exemplo
```
# cp /usr/local/share/examples/openvpn/sample-config-files/server.conf /usr/local/etc/openvpn/openvpn.conf
```

* Copiando arquivos de configuração do easy-rsa
```
# cp -r /usr/local/share/easy-rsa /usr/local/etc/openvpn/easy-rsa
```

* Configurando Easy-RSA

    * Editando arquivo de configuração
		* Mude as seguintes variáveis para os valores corretos
			```
			set_var EASYRSA_REQ_COUNTRY "BR"
			set_var EASYRSA_REQ_PROVINCE "Distrito Federal"
			set_var EASYRSA_REQ_CITY "Gama"
			set_var EASYRSA_REQ_ORG	"University of Brasilia"
			set_var EASYRSA_REQ_EMAIL "me@example.net"
			set_var EASYRSA_REQ_OU "FGA"
			```

    * Pode-se personalizar mais alguns aspectos como o prazo para expiração do certificado, tamanho da chave, etc.

#### Gerando as chaves

* Para gerar as chaves, pode-se utilizar o script em sh  `easyrsa.real`.

* Inicializando shell sh

```
sh
```

* Executando init.pki

```
./easyrsa.real init-pki
```

* Gerando certificado de autoridade

```
./easyrsa.real build-ca
./easyrsa.real build-server-full openvpn-server
```

* Agora insira as informações solicitadas para que o certificado seja gerado

#### Gerando o certificado do cliente

```
./easyrsa.real build-client-full padrao
```
Onde "padrao" é o nome do cliente

#### Gerando parâmetros Diffie Hellman

```
./easyrsa.real gen-dh
```

#### Movendo as chaves

* Mova os arquivos `dh.pem` e `ca.crt` para a pasta `/usr/local/etc/openvpn/easy-rsa/pki/`

* Mova os arquivos `openvpn-server.crt` e `padrao.crt` para
`/usr/local/etc/openvpn/easy-rsa/issued`

* Mova os arquivos `openvpn-server.key`, `padrao.key` e `ca.key` para `/usr/local/etc/openvpn/easy-rsa/private`

* Crie o diretório para as chaves pois a máquina servidor é a mesma usada como a autoridade certificadora

```
mkdir /usr/local/etc/openvpn/keys/
```

* Copie os arquivos `dh.pem`, `ca.crt`, `openvpn-server.crt` e `openvpn-server.key` para a pasta `keys/`

### Editando arquivo de configuração do servidor

* No arquivo `/usr/local/etc/openvpn/openvpn.conf` descomente as linhas:

```
user nobody
group nobody
```
* Edite os caminhos onde as chaves estão localizadas:

```
askpass /usr/local/etc/openvpn/pass.pass # adicione esta linha
ca /usr/local/etc/openvpn/keys/ca.crt
cert /usr/local/etc/openvpn/keys/openvpn-server.crt
key /usr/local/etc/openvpn/keys/openvpn-server.key
```
Onde pass.pass é um arquivo com a senha que você criou


* Dê permissões de leitura e escrita ao arquivo da senha

`chmod 600 pass.pass`

* Também complete o caminho para o parâmetro Diffie Hellman

`dh /usr/local/etc/openvpn/keys/dh.pem`

### Redirecionando tráfego dos clientes para o túnel
* Descomente a seguinte linha no arquivo `/usr/local/etc/openvpn/openvpn.conf`

```
push "redirect-gateway def1 bypass-dhcp"
```


## Configuração do cliente

### Sistema operacional

Manjaro-deepin, distribuição não oficial baseada no Manjaro que é baseado no Arch Linux

### Instalando openvpn

* Execute o comando
```
# pacman -S openvpn
```
E prossiga com a instalação

### Copiando arquivos para o cliente

* Copie os arquivos `padrao.crt` e `padrao.key` para `/etc/openvpn/client/`

* Copie os arquivos `ca.crt`, `pass.pass` e `ta.key` para `/etc/openvpn/`

### Configuração do openvpn

* Copie o arquivo `/usr/share/openvpn/examples/client.conf` para `/etc/openvpn/client/`

### Editando arquivo de configuração do cliente

* Abra o arquivo `client.conf`

```
askpass /usr/local/etc/openvpn/pass.pass # adicione esta linha
ca /usr/local/etc/openvpn/keys/ca.crt
cert /usr/local/etc/openvpn/keys/openvpn-server.crt
key /usr/local/etc/openvpn/keys/openvpn-server.key
```
Onde pass.pass é um arquivo com a senha que você criou

* Edite a linha `remote my-server-1 1194` para `remote 192.168.133.216 1194` onde o `192.168.133.216` é o IP da interface externa do servidor e `1194` a porta
* Se a linha `dev tun` estiver descomentada, comente-a
* Altere a linha `dev tap` para `dev tap0` e a descomente

## Ativando openvpn

* Execute os comandos a seguir no servidor para que a VPN seja inicializada durante o boot
```
sysrc openvpn_enable="YES"
sysrc openvpn_if="tap"
```

## Configurando bridge
```
sysrc cloned_interfaces="bridge0 tap0"
sysrc ifconfig_bridge0="inet 172.16.0.1 netmask 255.255.0.0"
sysrc ifconfig_tap0="inet 172.16.0.4 netmask 255.255.0.0"
sysrc ifconfig_bridge0="addm rl0 addm tap0 up"
```
Onde rl0 é a interface do roteador da rede LAN

* Os argumentos de sysrc serão adicionados em `/etc/rc.conf`

* reinicie o serviço de internet com o comando `service netif restart`

* se as interfaces não estiverem na bridge, adicione-as manualmente com o comando
```
# ifconfig bridge0 addm rl0 addm tap0 up
```

## Configurando o logging

* Adicione
```
!openvpn
*.*              /var/log/openvpn.log
```
antes de `*!`

* Ativando rotação do logging

No arquivo `/etc/newsyslog.conf` adicione a linha:
```
/var/log/openvpn.log            600 300 *@00 ZC
```

No lista de arquivos de log. Há explicações sobre o que significa cada parâmetro no próprio arquivo.

* Execute o seguinte comando para habilitar o log no `rc.conf`:

```
# sysrc syslogd_enable="YES"
```


## Inicializando openvpn no servidor
* Execute o seguinte comando para inicializar o openvpn no servidor
```
# service openvpn start
```

* Agora o comando `ifconfig` já deve mostrar a interface tun0 (nome default) entre as interfaces de rede existentes

## Inicializando openvpn no cliente

* Para inicializar o openvpn no cliente o seguinte comando pode ser utilizado:

```
# systemctl start openvpn-client@client
```

* Se quiser que ela seja ativada ao inicializar o sistema, execute o comando

```
# systemctl enable openvpn-client@client
```

* Verifique se o serviço está executando com o comando:

```
# systemctl status openvpn-client@client
```

# Configuração do firewall

## Sistema operacional
FreeBSD 11.1

## Aplicações utilizadas para configuração do firewall

* ipfw


## Configuração inicial

* Adicione as seguintes linhas no aruivo `rc.conf`:

```
firewall_enable="YES"
firewall_logging="YES"
firewall_script="/etc/ipfw.rules"
```

## Bloqueando ping

* No arquivo `/etc/ipfw.rules` adicione as seguintes regras para funcionamento junto ao NAT:

```
ipfw -q -f flush

# Default NAT rules
ipfw -q add 00050 divert 8668 ip4 from any to any via re0
ipfw -q add 00100 allow ip from any to any via lo0
ipfw -q add 00200 deny ip from any to 127.0.0.0/8
ipfw -q add 00300 deny ip from 127.0.0.0/8 to any
```

* Para bloquear o ping, adicione a linha:

```
ipfw -q add 00100 deny log icmp from 172.16.0.0/16 to any via rl0
```
onde log é para habilitar o log e rl0 é a interface do gateway que interage com a LAN.

* Reinicialize o serviço com o comando:

```
# service ipfw restart
```

# Redirecionamento de portas de servidor com IP estático

## Sistema operacional
FreeBSD 11.1


## Aplicações utilizadas para configuração do firewall
* natd
* dhcpd

## Configuração inicial

* No arquivo `/etc/rc.conf` adicione a seguinte linha se já não existir:
```
natd_flags="-f /etc/natd.conf"
```

* No arquivo `/etc/natd.conf` adicione as seguintes linhas:

```
redirect_port tcp 172.16.0.3:8080 80
redirect_port tcp 172.16.0.3:443 443
```
onde 172.16.0.3 é o IP da máquina que está rodando o servidor, 8080 é a porta em que o servidor está disponibilizando um serviço HTTP, 80 é a porta que será acessada externamente e 443 é tanto a porta em que o servidor está disponibilizando um serviço HTTPS quanto a porta que será acessada externamente.

## Atribuindo IP estático para o servidor

* No arquivo `/usr/local/etc/dhcpd.conf` adicione as seguintes linhas:

```
host server {
    hardware ethernet 00:30:67:b8:fe:77;
    fixed-address 172.16.0.3;
}
```
onde 00:30:67:b8:fe:77 é o endereço MAC da máquina do servidor, 172.16.0.3 é o IP que está sendo atribuído como fixo e server é o nome que está sendo dado ao host

* Reinicie os serviços natd e dhcpd com os comandos:

```
# service natd restart
# service isc-dhcpd restart
```
## Validação do redirecionamento de portas

* Ao conectar o servidor na rede LAN, respeitando as interfaces corretas, foi possível validar que as portas solicitadas estavam conectadas ao se fazer uma requisição utilizando o comando curl ou acessando via browser a URL `http://192.168.133.216` de um equipamento conectado na rede "LDS_Experimentos" por meio da visualização de um HTML simples com a frase "It works!"

* Para validação do redirecionamento da porta que server o serviço HTTPS, foi validado de forma semelhante apenas alterando a URL para `https://192.168.133.216`

## Validação do firewall (Pinging)

* Através de testes com computadores conectados na LAN, foi possível verificar a possibilidade de pingar computadores de dentro da LAN, mas nenhum de fora, incluindo os roteadore das redes "LDS_Experimentos" e "LDS_UED". Além disso, também era possível pingar através de computadores externos o gateway da LAN.

* Para uma validação mais concreta, também foram monitorados o arquivo de log configurado anteriormente em `/var/log/security` utilizando o comando `tail`

## Validação da VPN

* No server é possível verificar que serviço da vpn está sendo executado com o comando
```
# service openvpn status
```

* No client é possível verificar que o serviço da vpn está sendo executado com o comando e a ocorrência de erros com o comando
```
# systemctl status openvpn-client@client
```

* o cliente estava conectado na rede 192.168.133.0, rede em que a interface externa do roteador estava conectado

* o client recebeu uma interface virtual `tap0` e um IP no range correto (172.16.0.2)

* do cliente era possível pingar a interface interna do computador roteador (172.16.0.1), que possui o mesmo endereço da bridge

* do computador roteador era possível pingar a interface interna do cliente (172.16.0.2)

* do cliente era possível acessar o serviço de um servidor presente na LAN através de seu IP interno a LAN (172.16.0.3)

Portante, a rede virtual criada pela vpn permitiu que um cliente externo se conectasse diretamente na rede LAN interna através apenas do IP público (visível da rede onde o cliente estava) e acessasse todos os dispositivos lá presentes. Como pode ser validado nos testes
