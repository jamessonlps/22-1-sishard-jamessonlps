# K-Nearest Neighbors Classifier

KNN(K — Nearest Neighbors) é um dos muitos algoritmos de aprendizado supervisionado usado em machine learning e é um classificador onde o aprendizado é baseado no quão "parecido" é um dado (um vetor) do outro. O treinamento é formado por vetores de n dimensões. O intuito dessa atividade, no entando, não é entender esse algoritmo, mas implementá-lo utilizando _threads_ em linguagem C.

Antes de executar, é necessário que você siga o passo-a-passo para instalação da biblioteca [raylib](https://www.raylib.com/index.html) de acordo com o seu sistema operacional.

Após isso, você deverá executar:

```powershell
make build
```

E depois:

```powershell
make start
```

Você pode alterar os parâmetros da execução no arquivo `makefile`. Por exemplo, as tags e seus argumentos indicam:

- `k`: número de neights
- `d`: caminho do arquivo de treino
- `t`: caminho do arquivo de teste
- `n` (opcional): número de threads executando simultaneamente

Nesse projeto, a pasta `csv` contém dados de treino e teste de exemplos.

Link do vídeo: https://youtu.be/ZrqWkTFUkJM
