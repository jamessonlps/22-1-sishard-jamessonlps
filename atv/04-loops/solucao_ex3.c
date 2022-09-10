/* Implemente aqui sua versão em C da função ex5
 * presente no arquivo ex3.o
 *
 * Chame sua função de ex3_solucao */

long ex3_solucao(long a, long b) {
    long d = 0;
    long e = 0;
    long ret, c;
    while (d < a) {
        ret = d / b;
        c = d % b;
        if (c == 0) {
            e += d;
        }
        d += 1;
    }
    ret = e;
    return ret;
}