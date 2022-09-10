
/* Implemente aqui sua versão em C da função soma_n
 * presente no arquivo ex2.o
 *
 * Chame sua função de soma_n_solucao */

long soma_n_solucao(int a) {
    long ret = 0, b = 0;
    long c;
    while (ret < a) {
        c = ret;
        b += c;
        ret += 1;
    }
    ret = b;
    return ret;
}
