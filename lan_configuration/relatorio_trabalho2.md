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
