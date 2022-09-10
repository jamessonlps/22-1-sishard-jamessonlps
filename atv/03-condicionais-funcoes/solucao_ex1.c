/* Implementar função solucao_ex1 */
int solucao_ex1(long a, long b, long rdx) {
    long retval = a + b;
    long c = retval + (4 * rdx);
    a *= a;
    retval = a + (2 * b);
    rdx += retval;
    retval = ((c - rdx) >= 0);
    return retval;
}