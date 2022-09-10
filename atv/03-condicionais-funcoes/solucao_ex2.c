long vezes2(long a);

/* Implementar função solucao_ex2 */
long solucao_ex2(long a, long b) {
    long c2 = a;
    a = b;
    long c1 = vezes2(a);
    if ((c1 - c2) <= 0) {
        c1 += c2;
        return c1;
    }
    c2 += c2;
    c1 += c2;
    return c1;
}