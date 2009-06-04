README IP.TV Client 0.2.3 BETA - Linux
======================================

Executando o client IP.TV
-------------------------
Execute o arquivo "iptv_client", localizado no diretório para o qual a aplicação foi extraída.

Requisitos
----------
- Uma versão relativamente recente do Linux, com kernel versão 2.6.
- Um servidor X (XFree86/X.org).
- Uma placa de som que suporte ALSA.
- Uma placa de vídeo capaz de executar OpenGL com aceleração por hardware, com um mínimo de 16 MB de RAM de vídeo. É recomendado utilizar placas GeForce.

Além disso, é necessário ter os seguintes pacotes e suas dependências instaladas no sistema para executar o programa:

- wxWidgets 2.8/GTK+ (libwxgtk2.8-0)
- libx264 (libx264-54)
- O codec de áudio Speex (libspeex1)
- Mesa OpenGL (libgl1-mesa-glx, libglu1-mesa)

É necessário também instalar a biblioteca ChartDirector, que não possui pacote nos repositórios oficiais do Ubuntu.

Para instalar a biblioteca Chart Director, siga os seguintes passos:

1. Faça o download da biblioteca no seguinte link: http://download2.advsofteng.com/chartdir_cpp_linux.tar.gz;
2. Extraia o arquivo para um diretório temporário;
3. Copie todo o conteúdo da pasta lib para o diretório /usr/local/lib. (sudo cp -a * /usr/local/lib);
4. Execute o seguinte comando: sudo ldconfig.

Observações
-----------
As dependências dos pacotes acima podem ser resolvidas pelo gerenciador de pacotes.

Os nomes dos pacotes mencionados entre parênteses referem-se a sistemas Debian/Ubuntu. Eles podem ser instalados com o seguinte comando:
"sudo apt-get install <pacote>", onde <pacote> deve ser substituído pelo nome do pacote que se deseja instalar.

Caso seja utilizada outra distribuição de Linux (Slackware, Fedora etc.), adapte os nomes dos pacotes e utilize o gerenciador de pacotes apropriado.
