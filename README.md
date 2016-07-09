Projeto MorphLineDetection 
--------------------------

O projeto MorphLineDetection "Detecção de linha usando técnicas de morfologia matemática" é uma implementação em Python e C.
O projeto é a implementação do projeto de mestrado "Detecção de ovos de S. Mansoni apartir da detecção de seus contornos".
Para maiores detalhes, imagens, vídeos segue o link do projeto: http://www.ime.usp.br/~edelgado/projeto.php


Requerimentos:
--------------

* [Python versão 2.5 ou superior](https://www.python.org/downloads/)
* [PIL (Python Imaging Library)](http://www.pythonware.com/products/pil/)
* [NumPy](http://www.numpy.org/) e [Scipy](https://www.scipy.org/)
* [SDC Morphology Toolbox for Python](http://www.mmorph.com/mmorph/download2/python.php3)

Para solicitar a bibliotéca SDC Morphology Toolbox for Python precisa-se seguir os seguintes passos:
  * solicitar uma licença
  * executar o script "~/morph4python16/morphinstall", e logo abrir a página "~/morph4python16/register.html"
  * preencher os campos e submeter o formulário
  * você receverá um e-mail com a licença "pymorph_license.txt"
  * copiar e colar a licença "pymorph_license.txt" na pasta "~/morph4python16/"


Configurações:
--------------

Precisa-se definir diretórios específicos devido a configurações em variáveis de ambiente e criação de links simbólicos.

Por exemplo colocar os scripts Python em /home/user/myDownloadedSoftware/
O pacote MMorph em /home/user/software/morph4python16/

As seguintes configurações devem ser realizadas:

* Incluir a licença do pacote MMorph na variável de ambiente PATH.
  ``` 
  export PATH = $PATH:/home/user/software/morph4python16
  ```

* Criar a variável de ambiente LD_LIBRARY_PATH, para que o computador encontre as bibliotecas compartilhadas do software e do pacote MMorph (.so).
  ```
  export LD_LIBRARY_PATH = $LD_LIBRARY_PATH:/home/user/software/morph4python16:/home/user/myDownloadedSoftware
  ```


Execução dos scripts:
---------------------

1. A través da GUI
------------------

Para executar pela Interface gráfica, usa-se o script projeto.py. 

```
python projeto.py
```


2. A través dum terminal
------------------------

Executar o script respectivo de cada processo. Por exemplo, num terminal deve escrever:

Para suavizado da imagem: 
```
python processoSuavizado.py '/home/user/folderWithImages/' '/home/user/folderWithImagesResult/' 8 60 30
```

Para aplicar o filtro de Ordem: 
```
python processoFiltroDeOrdem.py '/home/user/folderWithImages/' '/home/user/folderWithImagesResult/' 6 1 10
```

Para detecção de linhas: 

```
python processoDeteccaoDeContornos.py '/home/user/folderWithImages/' '/home/user/folderWithImagesResult/' 6 70 1 500
```

Para detecção de elipses: 

```
python processoDeteccaoDeElipses.py '/home/user/folderWithImages' '/home/user/folderWithImagesResult' 150 0.1 150 0.3 500 50 0.95
```


Observação: Os parâmetros são de exemplo, eles estão fortemente relacionados às características própias de cada tipo de imagens.


Autores
-------

* Edwin Delgado Huaynalaya <edelgado@ime.usp.br>
* Robert Ainsley McLaughlin <robert.mclaughlin@uwa.edu.au>
* Nina T. S. Hirata <nina@ime.usp.br>
